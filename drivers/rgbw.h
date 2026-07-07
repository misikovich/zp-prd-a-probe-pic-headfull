#ifndef RGBW_H
#define RGBW_H

#include "utils.h"

typedef struct
{
    u8 r;
    u8 g;
    u8 b;
    u8 w;
} RGBW_STATE;

#define RGBW_STATE_INIT(r_, g_, b_, w_) ((RGBW_STATE){ (u8)(r_), (u8)(g_), (u8)(b_), (u8)(w_) })

#define RGBW_RED    RGBW_STATE_INIT(255u, 0u, 0u, 0u)
#define RGBW_REDF   RGBW_STATE_INIT(255u, 0u, 60u, 10u)
#define RGBW_BLUE   RGBW_STATE_INIT(0u, 0u, 255u, 0u)
#define RGBW_BLUEF  RGBW_STATE_INIT(60u, 0u, 255u, 10u)
#define RGBW_GREEN  RGBW_STATE_INIT(0u, 255u, 0u, 0u)
#define RGBW_GREENF RGBW_STATE_INIT(0u, 255u, 60u, 10u)
#define RGBW_WHITE  RGBW_STATE_INIT(0u, 0u, 0u, 255u)
#define RGBW_WHITEF  RGBW_STATE_INIT(180u, 200u, 255u, 255u)
#define RGBW_CLEAR  RGBW_STATE_INIT(0u, 0u, 0u, 0u)

void rgbw_init(void);

void rgbw_set_r(u16 duty);
void rgbw_set_g(u16 duty);
void rgbw_set_b(u16 duty);
void rgbw_set_w(u16 duty);

void rgbw_hold_r(u16 duty, u32 delay_ms);
void rgbw_hold_g(u16 duty, u32 delay_ms);
void rgbw_hold_b(u16 duty, u32 delay_ms);
void rgbw_hold_w(u16 duty, u32 delay_ms);

typedef struct
{
    RGBW_STATE target;
    u32 duration;
    u32 left;
} RGBW_TRANSITION;

void rgbw_new_transition(RGBW_STATE state, u32 d_ms);
void rgbw_hold_transition(RGBW_STATE state, u32 d_ms);
void rgbw_tick(void);

#endif /* RGBW_H */
