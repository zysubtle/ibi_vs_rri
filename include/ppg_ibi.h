#ifndef PPG_IBI_H
#define PPG_IBI_H

#include <stddef.h>
#include <stdint.h>

#include "ppg_ibi_config.h"

typedef enum {
    PPG_IBI_STATE_INIT = 0,
    PPG_IBI_STATE_ACQUIRE,
    PPG_IBI_STATE_TRACK,
    PPG_IBI_STATE_HOLD,
    PPG_IBI_STATE_REACQUIRE,
    PPG_IBI_STATE_INVALID
} ppg_ibi_state_t;

typedef enum {
    PPG_IBI_REJECT_NONE = 0,
    PPG_IBI_REJECT_ALLOW_MEASURE_FALSE,
    PPG_IBI_REJECT_LOW_SIGNAL_QUALITY,
    PPG_IBI_REJECT_SATURATED,
    PPG_IBI_REJECT_TIMESTAMP_GAP,
    PPG_IBI_REJECT_SAMPLE_DROP,
    PPG_IBI_REJECT_CHANNEL_INVALID,
    PPG_IBI_REJECT_IBI_OUT_OF_RANGE,
    PPG_IBI_REJECT_NO_STABLE_PULSE,
    PPG_IBI_REJECT_INTERNAL_INVALID
} ppg_ibi_reject_reason_t;

typedef enum {
    PPG_IBI_STATUS_OK = 0,
    PPG_IBI_STATUS_NO_EVENT,
    PPG_IBI_STATUS_EVENT_READY,
    PPG_IBI_STATUS_INVALID_ARGUMENT,
    PPG_IBI_STATUS_NOT_INITIALIZED
} ppg_ibi_status_t;

typedef struct {
    uint32_t timestamp_ms;
    int32_t ppg[PPG_IBI_CHANNEL_COUNT];
    uint8_t allow_measure;
} ppg_ibi_sample_t;

typedef struct {
    uint32_t timestamp_ms;
    uint32_t sample_index;
    uint16_t ibi_ms;
    uint32_t beat_count;
    uint8_t confidence;
    uint8_t signal_quality;
    uint8_t selected_channel;
    ppg_ibi_state_t state;
    ppg_ibi_reject_reason_t reject_reason;
    uint32_t debug_flags;
} ppg_ibi_event_t;

typedef struct {
    ppg_ibi_config_t config;
    uint32_t sample_counter;
    uint32_t beat_count;
    uint32_t last_timestamp_ms;
    uint8_t has_last_timestamp;
    uint8_t is_initialized;
    ppg_ibi_state_t state;
} ppg_ibi_context_t;

void ppg_ibi_config_default(ppg_ibi_config_t *config);
ppg_ibi_status_t ppg_ibi_init(ppg_ibi_context_t *ctx, const ppg_ibi_config_t *config);
ppg_ibi_status_t ppg_ibi_reset(ppg_ibi_context_t *ctx);
ppg_ibi_status_t ppg_ibi_process(ppg_ibi_context_t *ctx, const ppg_ibi_sample_t *sample, ppg_ibi_event_t *event);
const char *ppg_ibi_version(void);
size_t ppg_ibi_context_size(void);

#endif
