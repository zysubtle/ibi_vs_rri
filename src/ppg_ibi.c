#include "ppg_ibi.h"

#include <string.h>

#include "ppg_ibi_internal.h"

static void ppg_ibi_fill_no_event(ppg_ibi_context_t *ctx,
                                  const ppg_ibi_sample_t *sample,
                                  ppg_ibi_event_t *event)
{
    event->timestamp_ms = sample->timestamp_ms;
    event->sample_index = ctx->sample_counter;
    event->ibi_ms = 0u;
    event->beat_count = ctx->beat_count;
    event->confidence = 0u;
    event->signal_quality = 0u;
    event->selected_channel = PPG_IBI_SELECTED_CHANNEL_INVALID;
    event->state = ctx->state;
    event->reject_reason = PPG_IBI_REJECT_NONE;
    event->debug_flags = 0u;
}

static uint8_t ppg_ibi_channel_quality(int32_t raw)
{
    if ((raw <= PPG_IBI_PPG_MIN_24BIT) || (raw >= PPG_IBI_PPG_MAX_24BIT)) {
        return PPG_IBI_CHANNEL_QUALITY_INVALID;
    }

    if ((raw <= (PPG_IBI_PPG_MIN_24BIT + PPG_IBI_PPG_NEAR_SATURATION_MARGIN)) ||
        (raw >= (PPG_IBI_PPG_MAX_24BIT - PPG_IBI_PPG_NEAR_SATURATION_MARGIN))) {
        return PPG_IBI_CHANNEL_QUALITY_LOW;
    }

    return PPG_IBI_CHANNEL_QUALITY_BASIC_VALID;
}

static uint8_t ppg_ibi_is_ppg_saturated(const ppg_ibi_sample_t *sample)
{
    size_t ch;

    for (ch = 0u; ch < (size_t)PPG_IBI_CHANNEL_COUNT; ++ch) {
        if (ppg_ibi_channel_quality(sample->ppg[ch]) == PPG_IBI_CHANNEL_QUALITY_INVALID) {
            return 1u;
        }
    }
    return 0u;
}

static void ppg_ibi_select_channel(const ppg_ibi_sample_t *sample, ppg_ibi_event_t *event)
{
    size_t ch;
    uint8_t best_quality = PPG_IBI_CHANNEL_QUALITY_INVALID;
    uint8_t best_channel = PPG_IBI_SELECTED_CHANNEL_INVALID;

    for (ch = 0u; ch < (size_t)PPG_IBI_CHANNEL_COUNT; ++ch) {
        uint8_t quality = ppg_ibi_channel_quality(sample->ppg[ch]);
        if (quality > best_quality) {
            best_quality = quality;
            best_channel = (uint8_t)ch;
        }
    }

    event->signal_quality = best_quality;
    event->selected_channel = best_channel;

    if (best_channel != PPG_IBI_SELECTED_CHANNEL_INVALID) {
        event->debug_flags |= PPG_IBI_DEBUG_FLAG_CHANNEL_SELECTED;
    }
}

void ppg_ibi_config_default(ppg_ibi_config_t *config)
{
    if (config == NULL) {
        return;
    }
    config->sample_rate_hz = PPG_IBI_SAMPLE_RATE_HZ;
    config->expected_interval_ms = PPG_IBI_EXPECTED_INTERVAL_MS;
    config->min_ibi_ms = PPG_IBI_MIN_IBI_MS;
    config->max_ibi_ms = PPG_IBI_MAX_IBI_MS;
    config->allow_timestamp_strict_check = 1u;
}

ppg_ibi_status_t ppg_ibi_init(ppg_ibi_context_t *ctx, const ppg_ibi_config_t *config)
{
    ppg_ibi_config_t local_config;
    if (ctx == NULL) {
        return PPG_IBI_STATUS_INVALID_ARGUMENT;
    }

    memset(ctx, 0, sizeof(*ctx));
    ppg_ibi_config_default(&local_config);
    if (config != NULL) {
        ctx->config = *config;
    } else {
        ctx->config = local_config;
    }
    ctx->state = PPG_IBI_STATE_INIT;
    ctx->is_initialized = 1u;
    return PPG_IBI_STATUS_OK;
}

ppg_ibi_status_t ppg_ibi_reset(ppg_ibi_context_t *ctx)
{
    if (ctx == NULL) {
        return PPG_IBI_STATUS_INVALID_ARGUMENT;
    }
    if (ctx->is_initialized == 0u) {
        return PPG_IBI_STATUS_NOT_INITIALIZED;
    }

    ctx->sample_counter = 0u;
    ctx->beat_count = 0u;
    ctx->last_timestamp_ms = 0u;
    ctx->has_last_timestamp = 0u;
    ctx->state = PPG_IBI_STATE_INIT;
    return PPG_IBI_STATUS_OK;
}

ppg_ibi_status_t ppg_ibi_process(ppg_ibi_context_t *ctx,
                                 const ppg_ibi_sample_t *sample,
                                 ppg_ibi_event_t *event)
{
    uint32_t interval_ms;

    if ((ctx == NULL) || (sample == NULL) || (event == NULL)) {
        return PPG_IBI_STATUS_INVALID_ARGUMENT;
    }
    if (ctx->is_initialized == 0u) {
        return PPG_IBI_STATUS_NOT_INITIALIZED;
    }

    ctx->sample_counter += 1u;
    ppg_ibi_fill_no_event(ctx, sample, event);

    if (sample->allow_measure == 0u) {
        ctx->state = PPG_IBI_STATE_HOLD;
        event->state = ctx->state;
        event->reject_reason = PPG_IBI_REJECT_ALLOW_MEASURE_FALSE;
        event->debug_flags |= PPG_IBI_DEBUG_FLAG_ALLOW_MEASURE_OFF;
    } else {
        if (ctx->state == PPG_IBI_STATE_HOLD) {
            ctx->state = PPG_IBI_STATE_REACQUIRE;
        } else if (ctx->state == PPG_IBI_STATE_INIT) {
            ctx->state = PPG_IBI_STATE_ACQUIRE;
        }

        event->state = ctx->state;
        ppg_ibi_select_channel(sample, event);

        if (ppg_ibi_is_ppg_saturated(sample) != 0u) {
            event->reject_reason = PPG_IBI_REJECT_SATURATED;
            event->debug_flags |= PPG_IBI_DEBUG_FLAG_PPG_SATURATED;
        }

        if ((ctx->has_last_timestamp != 0u) && (ctx->config.allow_timestamp_strict_check != 0u)) {
            interval_ms = sample->timestamp_ms - ctx->last_timestamp_ms;
            if (interval_ms > (uint32_t)ctx->config.expected_interval_ms) {
                if (event->reject_reason == PPG_IBI_REJECT_NONE) {
                    event->reject_reason = PPG_IBI_REJECT_SAMPLE_DROP;
                }
                event->debug_flags |= PPG_IBI_DEBUG_FLAG_SAMPLE_DROP;
            } else if (interval_ms != (uint32_t)ctx->config.expected_interval_ms) {
                if (event->reject_reason == PPG_IBI_REJECT_NONE) {
                    event->reject_reason = PPG_IBI_REJECT_TIMESTAMP_GAP;
                }
                event->debug_flags |= PPG_IBI_DEBUG_FLAG_TIMESTAMP_GAP;
            }
        }

        if ((event->reject_reason == PPG_IBI_REJECT_NONE) &&
            (event->signal_quality < PPG_IBI_SIGNAL_QUALITY_ACCEPT_THRESHOLD)) {
            event->reject_reason = PPG_IBI_REJECT_LOW_SIGNAL_QUALITY;
            event->debug_flags |= PPG_IBI_DEBUG_FLAG_LOW_SIGNAL_QUALITY;
        }
    }

    ctx->last_timestamp_ms = sample->timestamp_ms;
    ctx->has_last_timestamp = 1u;

    return PPG_IBI_STATUS_NO_EVENT;
}

const char *ppg_ibi_version(void)
{
    return "0.4.0-m4";
}

size_t ppg_ibi_context_size(void)
{
    return sizeof(ppg_ibi_context_t);
}
