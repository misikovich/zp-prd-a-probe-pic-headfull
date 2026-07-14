#include "emeter_service.h"

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "emeter.h"
#include "mcc_generated_files/system/pins.h"
#include "wproto.h"

#define EMETER_SERVICE_STACK 384u /* 16-bit words */
#define EMETER_SERVICE_PRIO  1u
#define EMETER_SAMPLE_MS     100u
#define EMETER_BATCH_MS      500u
#define EMETER_SCALAR_MS     1000u

typedef struct {
    EmeterSample latest;
    bool have_capture;
    u16 va[WP_BUFFER_LOG_SIZE];
    u16 vb[WP_BUFFER_LOG_SIZE];
    u16 vc[WP_BUFFER_LOG_SIZE];
    u16 ia[WP_BUFFER_LOG_SIZE];
    u16 ib[WP_BUFFER_LOG_SIZE];
    u16 imax[WP_BUFFER_LOG_SIZE];
    u16 batch_generation;
} EmeterPublished;

static EmeterPublished published;
static u16 staging_va[WP_BUFFER_LOG_SIZE];
static u16 staging_vb[WP_BUFFER_LOG_SIZE];
static u16 staging_vc[WP_BUFFER_LOG_SIZE];
static u16 staging_ia[WP_BUFFER_LOG_SIZE];
static u16 staging_ib[WP_BUFFER_LOG_SIZE];
static u16 staging_imax[WP_BUFFER_LOG_SIZE];
static u8 staging_count;
static u32 last_frame;
static bool have_frame;

static bool get_latest(EmeterSample *sample)
{
    bool available;

    taskENTER_CRITICAL();
    available = published.have_capture;
    if (available) {
        *sample = published.latest;
    }
    taskEXIT_CRITICAL();
    return available;
}

static u16 rms_u16(u32 raw)
{
    return (u16)((raw >> 8u) & 0xFFFFu);
}

static void append_chart_sample(const EmeterSample *sample)
{
    u32 imax;
    u8 i;

    i = staging_count;
    staging_va[i] = rms_u16(sample->va_rms);
    staging_vb[i] = rms_u16(sample->vb_rms);
    staging_vc[i] = rms_u16(sample->vc_rms);
    staging_ia[i] = rms_u16(sample->ia_rms);
    staging_ib[i] = rms_u16(sample->ib_rms);

    imax = sample->ia_rms;
    if (sample->ib_rms > imax) {
        imax = sample->ib_rms;
    }
    if (sample->ic_rms > imax) {
        imax = sample->ic_rms;
    }
    staging_imax[i] = rms_u16(imax);

    staging_count++;
    if (staging_count < WP_BUFFER_LOG_SIZE) {
        return;
    }

    taskENTER_CRITICAL();
    memcpy(published.va, staging_va, sizeof(published.va));
    memcpy(published.vb, staging_vb, sizeof(published.vb));
    memcpy(published.vc, staging_vc, sizeof(published.vc));
    memcpy(published.ia, staging_ia, sizeof(published.ia));
    memcpy(published.ib, staging_ib, sizeof(published.ib));
    memcpy(published.imax, staging_imax, sizeof(published.imax));
    published.batch_generation++;
    if (published.batch_generation == 0u) {
        published.batch_generation = 1u;
    }
    taskEXIT_CRITICAL();
    staging_count = 0u;
}

static void emeter_sampler(void *parameters)
{
    EmeterSample sample;
    TickType_t wake;
    u8 last_error = 0xFFu;
    char err_msg[] = "emeter: err=?";
    u8 ok;

    unused(parameters);
    wake = xTaskGetTickCount();
    dlog("Started Task: emeter");

    forever
    {
        ok = emeter_poll(&sample);
        if (sample.error != last_error) {
            last_error = sample.error;
            err_msg[sizeof(err_msg) - 2u] = (char)('0' + sample.error);
            dlog(err_msg);
        }
        if ((!ok) && (sample.error == EMETER_ERROR_BUS_BUSY)) {
            /* Expected while FPGA owns SPI: retain the last valid state. */
            vTaskDelayUntil(&wake, pdMS_TO_TICKS(EMETER_SAMPLE_MS));
            continue;
        }

        taskENTER_CRITICAL();
        published.latest = sample;
        published.have_capture = true;
        taskEXIT_CRITICAL();

        if (ok && ((!have_frame) || (sample.frame != last_frame))) {
            have_frame = true;
            last_frame = sample.frame;
            append_chart_sample(&sample);
        }

        vTaskDelayUntil(&wake, pdMS_TO_TICKS(EMETER_SAMPLE_MS));
    }
}

static u8 collect_batch(u8 *value, const u16 *samples, u16 *last_generation)
{
    u16 generation;
    u8 i;

    taskENTER_CRITICAL();
    generation = published.batch_generation;
    if ((generation == 0u) || (generation == *last_generation)) {
        taskEXIT_CRITICAL();
        return 0u;
    }
    for (i = 0u; i < WP_BUFFER_LOG_SIZE; i++) {
        value[(u8)(i * 2u)] = (u8)(samples[i] & 0xFFu);
        value[(u8)(i * 2u + 1u)] = (u8)(samples[i] >> 8u);
    }
    *last_generation = generation;
    taskEXIT_CRITICAL();
    return (u8)(WP_BUFFER_LOG_SIZE * 2u);
}

#define DEFINE_BATCH_COLLECTOR(name, member)                 \
    static u8 name(u8 *value)                                \
    {                                                        \
        static u16 last_generation;                          \
        return collect_batch(value, published.member,        \
                &last_generation);                           \
    }

DEFINE_BATCH_COLLECTOR(collect_em_va, va)
DEFINE_BATCH_COLLECTOR(collect_em_vb, vb)
DEFINE_BATCH_COLLECTOR(collect_em_vc, vc)
DEFINE_BATCH_COLLECTOR(collect_em_ia, ia)
DEFINE_BATCH_COLLECTOR(collect_em_ib, ib)
DEFINE_BATCH_COLLECTOR(collect_em_imax, imax)

static u8 collect_em_link(u8 *value)
{
    EmeterSample sample;

    if (!get_latest(&sample)) {
        return 0u;
    }
    value[0] = sample.link_ok ? 0x01u
             : (sample.error == EMETER_ERROR_ALL_ZERO) ? 0x00u : 0xFFu;
    return 1u;
}

static u8 collect_em_fwver(u8 *value)
{
    EmeterSample sample;

    if (!get_latest(&sample)) {
        return 0u;
    }
    value[0] = (u8)((sample.fw_version >> 16u) & 0xFFu);
    value[1] = (u8)((sample.fw_version >> 8u) & 0xFFu);
    value[2] = (u8)(sample.fw_version & 0xFFu);
    return 3u;
}

static u8 collect_em_drdy(u8 *value)
{
    EmeterSample sample;

    if (!get_latest(&sample)) {
        return 0u;
    }
    value[0] = (sample.status & EMETER_STATUS_DRDY) ? 1u : 0u;
    return 1u;
}

static u8 collect_em_frequency(u8 *value)
{
    EmeterSample sample;
    s32 raw;
    u16 hundredths;

    if (!get_latest(&sample)) {
        return 0u;
    }
    raw = emeter_s24(sample.freq); /* S.16 Hz */
    hundredths = (raw > 0) ?
            (u16)(((u32)raw * 100UL + 32768UL) / 65536UL) : 0u;
    value[0] = (u8)(hundredths & 0xFFu);
    value[1] = (u8)(hundredths >> 8u);
    return 2u;
}

static u8 collect_em_warn(u8 *value)
{
    EmeterSample sample;

    if (!get_latest(&sample)) {
        return 0u;
    }
    value[0] = (sample.status & EMETER_STATUS_ALARMS) ? 1u : 0u;
    return 1u;
}

static u8 collect_em_error(u8 *value)
{
    EmeterSample sample;

    if (!get_latest(&sample)) {
        return 0u;
    }
    value[0] = sample.error;
    return 1u;
}

static u8 collect_em_temp(u8 *value)
{
    EmeterSample sample;
    u16 raw;

    if (!get_latest(&sample)) {
        return 0u;
    }
    raw = (u16)(sample.tempc & 0xFFFFu); /* existing unsigned raw S.10 */
    value[0] = (u8)(raw & 0xFFu);
    value[1] = (u8)(raw >> 8u);
    return 2u;
}

static u8 collect_em_interrupt(u8 *value)
{
    value[0] = EM_INT_GetValue() ? 0u : 1u; /* active low */
    return 1u;
}

static u8 collect_em_alarm(u8 *value)
{
    value[0] = EM_ALM_GetValue() ? 0u : 1u; /* active low */
    return 1u;
}

void emeter_service_init(void)
{
    BaseType_t created;

    memset(&published, 0, sizeof(published));
    staging_count = 0u;
    have_frame = false;
    emeter_init();

    configASSERT(wproto_add_reporter(WP_TYPE_EM_VA, EMETER_BATCH_MS,
            collect_em_va));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_VB, EMETER_BATCH_MS,
            collect_em_vb));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_VC, EMETER_BATCH_MS,
            collect_em_vc));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_IA, EMETER_BATCH_MS,
            collect_em_ia));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_IB, EMETER_BATCH_MS,
            collect_em_ib));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_IMAX, EMETER_BATCH_MS,
            collect_em_imax));

    configASSERT(wproto_add_reporter(WP_TYPE_EM_LINK, EMETER_SCALAR_MS,
            collect_em_link));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_FWVER, EMETER_SCALAR_MS,
            collect_em_fwver));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_DRDY, EMETER_SCALAR_MS,
            collect_em_drdy));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_FHZ, EMETER_SCALAR_MS,
            collect_em_frequency));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_WARN, EMETER_SCALAR_MS,
            collect_em_warn));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_ERR, EMETER_SCALAR_MS,
            collect_em_error));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_TEMP, EMETER_SCALAR_MS,
            collect_em_temp));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_INTERRUPT, EMETER_SCALAR_MS,
            collect_em_interrupt));
    configASSERT(wproto_add_reporter(WP_TYPE_EM_ALARM, EMETER_SCALAR_MS,
            collect_em_alarm));

    created = xTaskCreate(emeter_sampler, "emeter", EMETER_SERVICE_STACK,
            NULL, EMETER_SERVICE_PRIO, NULL);
    configASSERT(created == pdPASS);
}
