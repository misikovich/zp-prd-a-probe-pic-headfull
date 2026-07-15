#include "sensor_service.h"

#include <stdbool.h>
#include <xc.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "utils.h"
#include "wproto.h"

#define SENSOR_REPORT_MS       1000u
#define ADC_READY_SPIN_LIMIT   100000UL
#define ADC_CONVERT_SPIN_LIMIT 10000UL
#define ADC_SHARED_SAMPLE_TAD  250u

/* Physical pin/channel mapping for dsPIC33CK256MP506 SG48 board wiring. */
#define ADC_CHANNEL_GRID_NGND 2u  /* V_NG, RB7/AN2 */
#define ADC_CHANNEL_GRID_L1L2 3u  /* V_L1L2, RA3/AN3 */
#define ADC_CHANNEL_PWR_HWID  4u  /* PWR_HW_ID, RA4/AN4 */
#define ADC_CHANNEL_TEMP_RCD  13u /* TEMP2, RC1/AN13 */
#define ADC_CHANNEL_SRV_POS   15u /* SRV_POS, RC3/AN15 */
#define ADC_CHANNEL_SRV_SENSE 17u /* SRV_SENSE, RC6/AN17 */
#define ADC_CHANNEL_TEMP_TYPE2 18u /* TEMP1, RD10/AN18 */

static bool adc_ready;
static SemaphoreHandle_t adc_mutex;

static bool adc_channel_ready(u8 channel)
{
    if (channel < 16u) {
        return (ADSTATL & ((u16)1u << channel)) != 0u;
    }
    return (ADSTATH & ((u16)1u << (channel - 16u))) != 0u;
}

static u16 adc_result_read(u8 channel)
{
    switch (channel) {
    case ADC_CHANNEL_GRID_NGND:
        return ADCBUF2;
    case ADC_CHANNEL_GRID_L1L2:
        return ADCBUF3;
    case ADC_CHANNEL_PWR_HWID:
        return ADCBUF4;
    case ADC_CHANNEL_TEMP_RCD:
        return ADCBUF13;
    case ADC_CHANNEL_SRV_POS:
        return ADCBUF15;
    case ADC_CHANNEL_SRV_SENSE:
        return ADCBUF17;
    case ADC_CHANNEL_TEMP_TYPE2:
        return ADCBUF18;
    default:
        return 0u;
    }
}

static bool adc_convert_raw_unlocked(u8 channel, u16 *result)
{
    u32 spins;

    if ((!adc_ready) || (result == NULL)) {
        return false;
    }

    /* Reading the channel buffer clears any old ANxRDY indication. */
    (void)adc_result_read(channel);
    ADCON3Lbits.CNVCHSEL = channel;
    ADCON3Lbits.CNVRTCH = 1u;

    spins = ADC_CONVERT_SPIN_LIMIT;
    while ((!adc_channel_ready(channel)) && (spins > 0u)) {
        spins--;
    }
    if (spins == 0u) {
        return false;
    }

    *result = adc_result_read(channel);
    return true;
}

static bool adc_read_raw_unlocked(u8 channel, u16 *result)
{
    u16 settling_sample;

    /* The shared ADC core is multiplexed between unrelated analog sources.
       Discard the first conversion after selecting a channel so residue from
       the previous source cannot become the reported absolute reading. */
    if (!adc_convert_raw_unlocked(channel, &settling_sample)) {
        return false;
    }
    return adc_convert_raw_unlocked(channel, result);
}

static u8 collect_adc(u8 *value, u8 channel)
{
    u16 raw;

    if ((adc_mutex == NULL) ||
            (xSemaphoreTake(adc_mutex, portMAX_DELAY) != pdTRUE)) {
        return 0u;
    }
    if (!adc_read_raw_unlocked(channel, &raw)) {
        (void)xSemaphoreGive(adc_mutex);
        return 0u;
    }
    (void)xSemaphoreGive(adc_mutex);
    value[0] = (u8)(raw & 0xFFu);
    value[1] = (u8)(raw >> 8u);
    return 2u;
}

#define DEFINE_ADC_COLLECTOR(name, channel)       \
    static u8 name(u8 *value)                     \
    {                                              \
        return collect_adc(value, (channel));      \
    }

DEFINE_ADC_COLLECTOR(collect_temp_type2, ADC_CHANNEL_TEMP_TYPE2)
DEFINE_ADC_COLLECTOR(collect_temp_rcd, ADC_CHANNEL_TEMP_RCD)
DEFINE_ADC_COLLECTOR(collect_pwr_hwid, ADC_CHANNEL_PWR_HWID)
DEFINE_ADC_COLLECTOR(collect_grid_ngnd, ADC_CHANNEL_GRID_NGND)
DEFINE_ADC_COLLECTOR(collect_grid_l1l2, ADC_CHANNEL_GRID_L1L2)

bool sensor_service_read_servo(u16 *position, u16 *sense)
{
    bool ok;

    if ((position == NULL) || (sense == NULL) || (adc_mutex == NULL) ||
            (xSemaphoreTake(adc_mutex, portMAX_DELAY) != pdTRUE)) {
        return false;
    }
    ok = adc_read_raw_unlocked(ADC_CHANNEL_SRV_POS, position) &&
            adc_read_raw_unlocked(ADC_CHANNEL_SRV_SENSE, sense);
    (void)xSemaphoreGive(adc_mutex);
    return ok;
}

static bool adc_init(void)
{
    u32 spins;

    /* Keep these inputs correct even if MCC pin generation changes. */
    _TRISB7 = 1u;
    _ANSELB7 = 1u;
    _TRISA3 = 1u;
    _ANSELA3 = 1u;
    _TRISA4 = 1u;
    _ANSELA4 = 1u;
    _TRISC1 = 1u;
    _ANSELC1 = 1u;
    _TRISC3 = 1u;
    _ANSELC3 = 1u;
    _TRISC6 = 1u;
    _ANSELC6 = 1u;
    _TRISD10 = 1u;
    _ANSELD10 = 1u;

    ADCON1L = 0u;
    ADCON1H = 0u;
    ADCON2L = 0u;
    ADCON2H = 0u;
    ADCON3L = 0u;
    ADCON3H = 0u;
    ADCON4L = 0u;
    ADCON4H = 0u;
    ADCON5L = 0u;
    ADCON5H = 0u;
    ADMOD0L = 0u;
    ADMOD0H = 0u;
    ADMOD1L = 0u;
    ADMOD1H = 0u;
    ADIEL = 0u;
    ADIEH = 0u;
    ADEIEL = 0u;
    ADEIEH = 0u;

    /* FP=100 MHz, shared core=50 MHz, 12-bit unsigned integer results.
       Give potentiometers and other non-buffered sources 5.04 us to charge
       the ADC sample capacitor: (SHRSAMC + 2) * 20 ns. */
    ADCON1Hbits.SHRRES = 3u;
    ADCON2Lbits.SHRADCS = 1u;
    ADCON2Hbits.SHRSAMC = ADC_SHARED_SAMPLE_TAD;
    ADCON5Hbits.WARMTIME = 15u;

    ADCON1Lbits.ADON = 1u;
    ADCON5Lbits.SHRPWR = 1u;

    spins = ADC_READY_SPIN_LIMIT;
    while (((ADCON2Hbits.REFRDY == 0u) ||
            (ADCON5Lbits.SHRRDY == 0u)) && (spins > 0u)) {
        spins--;
    }
    if (spins == 0u) {
        ADCON5Lbits.SHRPWR = 0u;
        ADCON1Lbits.ADON = 0u;
        return false;
    }

    ADCON3Hbits.SHREN = 1u;
    return true;
}

void sensor_service_init(void)
{
    adc_mutex = xSemaphoreCreateMutex();
    configASSERT(adc_mutex != NULL);
    adc_ready = adc_init();
    if (!adc_ready) {
        dlog("sensors: ADC init failed");
        return;
    }

    /* Ambient temperature is intentionally omitted until its source is
       selected; TEMP1 is the type-2/socket sensor and TEMP2 is RCD. */
    configASSERT(wproto_add_reporter(WP_TYPE_TEMP_SOCKET_ADC,
            SENSOR_REPORT_MS, collect_temp_type2));
    configASSERT(wproto_add_reporter(WP_TYPE_TEMP_RCD_ADC,
            SENSOR_REPORT_MS, collect_temp_rcd));
    configASSERT(wproto_add_reporter(WP_TYPE_PWR_HWID_ADC,
            SENSOR_REPORT_MS, collect_pwr_hwid));
    configASSERT(wproto_add_reporter(WP_TYPE_GRID_NGND_ADC,
            SENSOR_REPORT_MS, collect_grid_ngnd));
    configASSERT(wproto_add_reporter(WP_TYPE_GRID_L1L2_ADC,
            SENSOR_REPORT_MS, collect_grid_l1l2));
}
