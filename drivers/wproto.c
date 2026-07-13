#include "wproto.h"

#include <stddef.h>

#include "semphr.h"

#include "esp_uart.h"

#define WPROTO_MAX_REPORTERS 24u
#define WPROTO_MAX_COMMAND_HANDLERS 8u
#define WPROTO_TASK_STACK 320u /* 16-bit words; collect callbacks run here */
#define WPROTO_TASK_PRIO 2u
#define WPROTO_RX_CHUNK 32u
#define WPROTO_IDLE_MS 10u     /* reporter scheduling granularity */
#define WPROTO_TX_WAIT_MS 50u

struct reporter {
    u8 type;
    TickType_t period;
    TickType_t last;
    wproto_collect_fn collect;
};

struct command_handler {
    u8 type;
    wproto_command_fn handle;
};

static struct reporter reporters[WPROTO_MAX_REPORTERS];
static u8 reporter_count;
static struct command_handler command_handlers[WPROTO_MAX_COMMAND_HANDLERS];
static u8 command_handler_count;

static SemaphoreHandle_t tx_mutex;
static u8 tx_frame[WP_MAX_FRAME_LEN];
static wp_rx_t rx;
static volatile bool client_connected;

static void send_frame(u8 src, u8 type, const u8 *value, u8 len)
{
    u16 frame_len;

    if (xSemaphoreTake(tx_mutex, pdMS_TO_TICKS(WPROTO_TX_WAIT_MS)) != pdTRUE) {
        return; /* fire-and-forget link: drop rather than stall the caller */
    }
    frame_len = wp_frame_build(tx_frame, src, type, value, len);
    esp_uart_send(tx_frame, frame_len);
    (void)xSemaphoreGive(tx_mutex);
}

void wproto_sound(u8 sound_type)
{
    send_frame(WP_SRC_PICESP, sound_type, NULL, 0u);
}

bool wproto_connected(void)
{
    return client_connected;
}

static void handle_connected(u8 value)
{
    const bool connected = (value != 0u);

    if (connected == client_connected) {
        return; /* duplicate report: idempotent */
    }
    client_connected = connected;
    wproto_sound(connected ? INT_ACT_SOUND_CONNECT : INT_ACT_SOUND_DISCONNECT);
    dlog(connected ? "wproto: client connected" : "wproto: client disconnected");
}

static void dispatch(const u8 *frame)
{
    const u8 hdr = frame[2];
    const u8 type = frame[3];
    const u8 len = frame[4];
    const u8 *value = &frame[5];
    u8 i;

    if ((hdr >> 4) != WP_VERSION || (hdr & 0x0Fu) != WP_SRC_CLIENT) {
        return;
    }

    if (type == WP_TYPE_CONNECTED) {
        if (len >= 1u) {
            handle_connected(value[0]);
        }
        return;
    }

    for (i = 0u; i < command_handler_count; i++) {
        if (command_handlers[i].type == type) {
            command_handlers[i].handle(type, value, len);
            return;
        }
    }
    /* unknown type with valid CRC: skip silently */
}

static void run_due_reporters(void)
{
    static u8 value[WPROTO_VALUE_MAX]; /* wproto task context only */
    const TickType_t now = xTaskGetTickCount();
    u8 len;
    u8 i;

    for (i = 0; i < reporter_count; i++) {
        struct reporter *r = &reporters[i];

        if ((TickType_t)(now - r->last) < r->period) {
            continue;
        }
        r->last = now;

        len = r->collect(value);
        if (len > 0u) {
            send_frame(WP_SRC_PROBE, r->type, value, len);
        }
    }
}

static void wproto_task(void *parameters)
{
    static u8 chunk[WPROTO_RX_CHUNK];
    TickType_t last_byte = 0;
    size_t n;
    size_t i;

    unused(parameters);
    task_hold(5u);
    dlog("Started Task: wproto");

    forever
    {
        n = esp_uart_receive(chunk, sizeof(chunk), pdMS_TO_TICKS(WPROTO_IDLE_MS));
        if (n > 0u) {
            last_byte = xTaskGetTickCount();
            for (i = 0; i < n; i++) {
                if (wp_rx_feed(&rx, chunk[i])) {
                    dispatch(rx.frame);
                }
            }
        } else if (rx.len > 0u &&
                   (TickType_t)(xTaskGetTickCount() - last_byte) >
                       pdMS_TO_TICKS(WP_INTERBYTE_TIMEOUT_MS)) {
            wp_rx_reset(&rx); /* stale partial frame */
        }

        run_due_reporters();
    }
}

static u8 collect_heartbeat(u8 *value)
{
    value[0] = WP_HEARTBEAT_VALUE;
    return 1u;
}

bool wproto_add_reporter(u8 type, u16 period_ms, wproto_collect_fn collect)
{
    struct reporter *r;

    if (reporter_count >= WPROTO_MAX_REPORTERS) {
        return false;
    }
    r = &reporters[reporter_count];
    r->type = type;
    r->period = pdMS_TO_TICKS(period_ms);
    r->last = xTaskGetTickCount();
    r->collect = collect;
    reporter_count++;
    return true;
}

bool wproto_add_command_handler(u8 type, wproto_command_fn handler)
{
    struct command_handler *h;

    if ((handler == NULL) ||
            (command_handler_count >= WPROTO_MAX_COMMAND_HANDLERS)) {
        return false;
    }
    h = &command_handlers[command_handler_count];
    h->type = type;
    h->handle = handler;
    command_handler_count++;
    return true;
}

void wproto_init(void)
{
    tx_mutex = xSemaphoreCreateMutex();
    wp_rx_reset(&rx);
    esp_uart_init();

    (void)wproto_add_reporter(WP_TYPE_HEARTBEAT, 1000u, collect_heartbeat);

    xTaskCreate(wproto_task, "wproto", WPROTO_TASK_STACK, NULL, WPROTO_TASK_PRIO, NULL);
}
