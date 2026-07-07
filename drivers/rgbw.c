#include <xc.h>
#include <stdbool.h>
#include <stddef.h>
#include "FreeRTOS.h"
#include "task.h"
#include "utils.h"
#include "rgbw.h"

#include "mcc_generated_files/pwm/sccp1.h"
#include "mcc_generated_files/pwm/sccp2.h"
#include "mcc_generated_files/pwm/sccp3.h"
#include "mcc_generated_files/pwm/sccp4.h"

/* Channels ride the MCC SCCP PWM blocks (custom names PWM_LEDR..PWM_LEDW,
 * SCCP1..SCCP4); MCC owns the pin mapping and clocking, this module owns
 * the period and duty. */

/* SCCP period in timer counts; rgbw_init() pushes this through the MCC
 * API so the duty math below is self-consistent (2 kHz at Fp = 100 MHz). */
#define LED_PWM_PERIOD_COUNTS 50000u

/* per-channel duty ceilings in the raw 0..255 domain */
#define LED_MAX_R           150u
#define LED_MAX_G           100u
#define LED_MAX_B           150u
#define LED_MAX_W           100u

#define RGBW_TPS 30
static inline void rgbw_hold_cycle(void) {
    vTaskDelay(pdMS_TO_TICKS(1000u / RGBW_TPS));
}

/* raw 0..255 duty -> SCCP compare counts */
static inline u16 rgbw_counts(u16 duty) {
    return (u16)(((u32)duty * LED_PWM_PERIOD_COUNTS) / 255u);
}

static RGBW_STATE rgbw_current;
static RGBW_TRANSITION rgbw_transition = { { 0u, 0u, 0u, 0u }, 0u, 0u };

static void rgbw_start_transition(RGBW_STATE s, u32 d_ms) {
    taskENTER_CRITICAL();
    rgbw_transition.target = s;
    rgbw_transition.duration = d_ms;
    rgbw_transition.left = d_ms;
    taskEXIT_CRITICAL();
}

static u32 rgbw_transition_left(void) {
    u32 left;

    taskENTER_CRITICAL();
    left = rgbw_transition.left;
    taskEXIT_CRITICAL();

    return left;
}

static void rgbw_log_u8(u8 value) {
    char digits[4];
    u8 count = 0u;

    if (value >= 100u) {
        digits[count++] = (char)('0' + (value / 100u));
        value = (u8)(value % 100u);
        digits[count++] = (char)('0' + (value / 10u));
        value = (u8)(value % 10u);
    } else if (value >= 10u) {
        digits[count++] = (char)('0' + (value / 10u));
        value = (u8)(value % 10u);
    }

    digits[count++] = (char)('0' + value);
    digits[count] = '\0';
    dlog_write(digits);
}

static void rgbw_log_transition(const char *prefix, RGBW_STATE s) {
    dlog_write(prefix);
    dlog_write("r=");
    rgbw_log_u8(s.r);
    dlog_write(" g=");
    rgbw_log_u8(s.g);
    dlog_write(" b=");
    rgbw_log_u8(s.b);
    dlog_write(" w=");
    rgbw_log_u8(s.w);
    dlog("");
}

void rgbw_new_transition(RGBW_STATE s, u32 d_ms) {
    rgbw_start_transition(s, d_ms);
    rgbw_log_transition("RGBW new -> ", s);
}

void rgbw_hold_transition(RGBW_STATE s, u32 d_ms) {
    rgbw_start_transition(s, d_ms);
    rgbw_log_transition("RGBW hold -> ", s);

    while (rgbw_transition_left() != 0u) {
        rgbw_hold_cycle();
    }
}

/* map a raw 0..255 channel value onto that channel's own duty ceiling.
 *
 * two things happen here:
 *  - per-channel scale (not a shared max + clamp) keeps the whole 0..255
 *    range live, so a fade uses every step instead of saturating early.
 *  - gamma ~2.0 (square law) pre-distorts the output. LED luminance is ~linear
 *    in duty but the eye is ~cube-root of luminance, so a LINEAR duty ramp
 *    looks like it slams bright in the first ~10% then crawls -- reads as a
 *    hard cut. Squaring makes luminance track f^2, so perceived brightness is
 *    ~linear in time and the fade actually looks like a fade. One multiply,
 *    no lookup table. */
static u16 rgbw_scale(u8 raw, u16 max) {
    u32 g = ((u32)raw * raw) / 255u;   /* gamma 2.0: 0..255 -> 0..255 */
    return (u16)((g * max) / 255u);
}

/* push the current raw state out to the four PWM channels */
static void rgbw_apply(void) {
    rgbw_set_r(rgbw_scale(rgbw_current.r, LED_MAX_R));
    rgbw_set_g(rgbw_scale(rgbw_current.g, LED_MAX_G));
    rgbw_set_b(rgbw_scale(rgbw_current.b, LED_MAX_B));
    rgbw_set_w(rgbw_scale(rgbw_current.w, LED_MAX_W));
}

/* move one channel toward target by 1/steps of the remaining delta.
 * recomputed every tick, so the path is linear and converges exactly.
 * a non-zero delta always moves at least 1 so small fades don't stall. */
static u8 rgbw_step_channel(u8 cur, u8 tgt, u32 steps) {
    int delta = (int)tgt - (int)cur;
    int step;

    if (delta == 0) return cur;

    step = delta / (int)steps;
    if (step == 0) step = (delta > 0) ? 1 : -1;

    return (u8)((int)cur + step);
}

void rgbw_interpolate(void) {
    u32 dt = 1000u / RGBW_TPS;   /* ms advanced per tick */
    u32 steps;

    taskENTER_CRITICAL();

    if (rgbw_transition.left == 0u) {
        taskEXIT_CRITICAL();
        return;
    }

    if (rgbw_transition.left <= dt) {
        /* final tick: snap exactly onto target */
        rgbw_current = rgbw_transition.target;
        rgbw_transition.left = 0u;
    } else {
        steps = rgbw_transition.left / dt;   /* >= 1 here */
        rgbw_current.r = rgbw_step_channel(rgbw_current.r, rgbw_transition.target.r, steps);
        rgbw_current.g = rgbw_step_channel(rgbw_current.g, rgbw_transition.target.g, steps);
        rgbw_current.b = rgbw_step_channel(rgbw_current.b, rgbw_transition.target.b, steps);
        rgbw_current.w = rgbw_step_channel(rgbw_current.w, rgbw_transition.target.w, steps);
        rgbw_transition.left -= dt;
    }

    taskEXIT_CRITICAL();

    rgbw_apply();
}

void rgbw_tick(void) {
    if (rgbw_transition_left() != 0u) {
        rgbw_interpolate();
    }
    rgbw_hold_cycle();   /* always pace; never busy-spin when idle */
}


/* SYSTEM_Initialize() has already run the SCCP init; enforce the period
 * the duty math assumes, zero the outputs and turn the blocks on */
void rgbw_init(void) {
    PWM_LEDR_PeriodSet(LED_PWM_PERIOD_COUNTS);
    PWM_LEDG_PeriodSet(LED_PWM_PERIOD_COUNTS);
    PWM_LEDB_PeriodSet(LED_PWM_PERIOD_COUNTS);
    PWM_LEDW_PeriodSet(LED_PWM_PERIOD_COUNTS);

    rgbw_set_r(0u);
    rgbw_set_g(0u);
    rgbw_set_b(0u);
    rgbw_set_w(0u);

    PWM_LEDR_Enable();
    PWM_LEDG_Enable();
    PWM_LEDB_Enable();
    PWM_LEDW_Enable();
}

void rgbw_set_r(u16 duty) {
    duty = (duty <= LED_MAX_R) ? duty : LED_MAX_R;
    PWM_LEDR_DutyCycleSet(rgbw_counts(duty));
}

void rgbw_set_g(u16 duty) {
    duty = (duty <= LED_MAX_G) ? duty : LED_MAX_G;
    PWM_LEDG_DutyCycleSet(rgbw_counts(duty));
}

void rgbw_set_b(u16 duty) {
    duty = (duty <= LED_MAX_B) ? duty : LED_MAX_B;
    PWM_LEDB_DutyCycleSet(rgbw_counts(duty));
}

void rgbw_set_w(u16 duty) {
    duty = (duty <= LED_MAX_W) ? duty : LED_MAX_W;
    PWM_LEDW_DutyCycleSet(rgbw_counts(duty));
}


void rgbw_hold_r(u16 duty, u32 delay_ms) {
    rgbw_set_r(duty);
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}

void rgbw_hold_g(u16 duty, u32 delay_ms) {
    rgbw_set_g(duty);
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}

void rgbw_hold_b(u16 duty, u32 delay_ms) {
    rgbw_set_b(duty);
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}

void rgbw_hold_w(u16 duty, u32 delay_ms) {
    rgbw_set_w(duty);
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}
