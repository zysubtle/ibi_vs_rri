#include "ppg_ibi.h"

#include <string.h>

#include "ppg_ibi_internal.h"

static void ppg_ibi_fill_no_event(ppg_ibi_context_t *ctx, const ppg_ibi_sample_t *sample, ppg_ibi_event_t *event) {
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

static void ppg_ibi_reset_detector(ppg_ibi_context_t *ctx) {
    ctx->has_prev_sample = 0u; ctx->has_prev2_sample = 0u;
    ctx->prev_raw = 0; ctx->prev2_raw = 0;
    ctx->prev_selected_channel = PPG_IBI_SELECTED_CHANNEL_INVALID; ctx->prev2_selected_channel = PPG_IBI_SELECTED_CHANNEL_INVALID;
    ctx->prev_signal_quality = 0u; ctx->prev2_signal_quality = 0u;
    ctx->prev_timestamp_ms = 0u; ctx->prev2_timestamp_ms = 0u;
    ctx->prev_sample_index = 0u; ctx->prev2_sample_index = 0u;
    ctx->has_last_pulse = 0u; ctx->last_pulse_timestamp_ms = 0u; ctx->last_pulse_sample_index = 0u;
}

static uint8_t ppg_ibi_channel_quality(int32_t raw) {
    if ((raw <= PPG_IBI_PPG_MIN_24BIT) || (raw >= PPG_IBI_PPG_MAX_24BIT)) return PPG_IBI_CHANNEL_QUALITY_INVALID;
    if ((raw <= (PPG_IBI_PPG_MIN_24BIT + PPG_IBI_PPG_NEAR_SATURATION_MARGIN)) || (raw >= (PPG_IBI_PPG_MAX_24BIT - PPG_IBI_PPG_NEAR_SATURATION_MARGIN))) return PPG_IBI_CHANNEL_QUALITY_LOW;
    return PPG_IBI_CHANNEL_QUALITY_BASIC_VALID;
}
static uint8_t ppg_ibi_is_ppg_saturated(const ppg_ibi_sample_t *sample) {
    for (size_t ch=0; ch<(size_t)PPG_IBI_CHANNEL_COUNT; ++ch) if (ppg_ibi_channel_quality(sample->ppg[ch]) == PPG_IBI_CHANNEL_QUALITY_INVALID) return 1u;
    return 0u;
}
static void ppg_ibi_select_channel(const ppg_ibi_sample_t *sample, ppg_ibi_event_t *event) {
    uint8_t bq=PPG_IBI_CHANNEL_QUALITY_INVALID, bc=PPG_IBI_SELECTED_CHANNEL_INVALID;
    for (size_t ch=0; ch<(size_t)PPG_IBI_CHANNEL_COUNT; ++ch) { uint8_t q=ppg_ibi_channel_quality(sample->ppg[ch]); if (q>bq){bq=q; bc=(uint8_t)ch;} }
    event->signal_quality=bq; event->selected_channel=bc; if (bc!=PPG_IBI_SELECTED_CHANNEL_INVALID) event->debug_flags|=PPG_IBI_DEBUG_FLAG_CHANNEL_SELECTED;
}

void ppg_ibi_config_default(ppg_ibi_config_t *config){ if(!config) return; config->sample_rate_hz=PPG_IBI_SAMPLE_RATE_HZ; config->expected_interval_ms=PPG_IBI_EXPECTED_INTERVAL_MS; config->min_ibi_ms=PPG_IBI_MIN_IBI_MS; config->max_ibi_ms=PPG_IBI_MAX_IBI_MS; config->allow_timestamp_strict_check=1u; }
ppg_ibi_status_t ppg_ibi_init(ppg_ibi_context_t *ctx,const ppg_ibi_config_t *config){ ppg_ibi_config_t local; if(!ctx) return PPG_IBI_STATUS_INVALID_ARGUMENT; memset(ctx,0,sizeof(*ctx)); ppg_ibi_config_default(&local); ctx->config=config?*config:local; ctx->state=PPG_IBI_STATE_INIT; ctx->is_initialized=1u; ppg_ibi_reset_detector(ctx); return PPG_IBI_STATUS_OK; }
ppg_ibi_status_t ppg_ibi_reset(ppg_ibi_context_t *ctx){ if(!ctx) return PPG_IBI_STATUS_INVALID_ARGUMENT; if(!ctx->is_initialized) return PPG_IBI_STATUS_NOT_INITIALIZED; ctx->sample_counter=0u; ctx->beat_count=0u; ctx->last_timestamp_ms=0u; ctx->has_last_timestamp=0u; ctx->state=PPG_IBI_STATE_INIT; ppg_ibi_reset_detector(ctx); return PPG_IBI_STATUS_OK; }

ppg_ibi_status_t ppg_ibi_process(ppg_ibi_context_t *ctx,const ppg_ibi_sample_t *sample,ppg_ibi_event_t *event){
    if ((!ctx) || (!sample) || (!event)) {
        return PPG_IBI_STATUS_INVALID_ARGUMENT;
    }
    if (ctx->is_initialized == 0u) {
        return PPG_IBI_STATUS_NOT_INITIALIZED;
    }
    ctx->sample_counter += 1u; ppg_ibi_fill_no_event(ctx,sample,event);
    if(sample->allow_measure==0u){ ctx->state=PPG_IBI_STATE_HOLD; event->state=ctx->state; event->reject_reason=PPG_IBI_REJECT_ALLOW_MEASURE_FALSE; event->debug_flags|=PPG_IBI_DEBUG_FLAG_ALLOW_MEASURE_OFF; ppg_ibi_reset_detector(ctx); }
    else {
        uint8_t strict_reject=0u;
        if(ctx->state==PPG_IBI_STATE_HOLD) ctx->state=PPG_IBI_STATE_REACQUIRE; else if(ctx->state==PPG_IBI_STATE_INIT) ctx->state=PPG_IBI_STATE_ACQUIRE;
        event->state=ctx->state; ppg_ibi_select_channel(sample,event);
        if(ppg_ibi_is_ppg_saturated(sample)){ event->reject_reason=PPG_IBI_REJECT_SATURATED; event->debug_flags|=PPG_IBI_DEBUG_FLAG_PPG_SATURATED; strict_reject=1u; }
        if((ctx->has_last_timestamp!=0u)&&(ctx->config.allow_timestamp_strict_check!=0u)){
            uint32_t dt=sample->timestamp_ms-ctx->last_timestamp_ms;
            if(dt>(uint32_t)ctx->config.expected_interval_ms){ if(event->reject_reason==PPG_IBI_REJECT_NONE) event->reject_reason=PPG_IBI_REJECT_SAMPLE_DROP; event->debug_flags|=PPG_IBI_DEBUG_FLAG_SAMPLE_DROP; strict_reject=1u; }
            else if(dt!=(uint32_t)ctx->config.expected_interval_ms){ if(event->reject_reason==PPG_IBI_REJECT_NONE) event->reject_reason=PPG_IBI_REJECT_TIMESTAMP_GAP; event->debug_flags|=PPG_IBI_DEBUG_FLAG_TIMESTAMP_GAP; strict_reject=1u; }
        }
        if((event->reject_reason==PPG_IBI_REJECT_NONE)&&(event->signal_quality<PPG_IBI_SIGNAL_QUALITY_ACCEPT_THRESHOLD)){ event->reject_reason=PPG_IBI_REJECT_LOW_SIGNAL_QUALITY; event->debug_flags|=PPG_IBI_DEBUG_FLAG_LOW_SIGNAL_QUALITY; strict_reject=1u; }

        if(strict_reject!=0u){ ppg_ibi_reset_detector(ctx); }
        else {
            int32_t raw = sample->ppg[event->selected_channel];
            if ((ctx->has_prev_sample != 0u) &&
                (ctx->has_prev2_sample != 0u) &&
                (ctx->prev2_selected_channel == ctx->prev_selected_channel) &&
                (ctx->prev_selected_channel == event->selected_channel) &&
                (ctx->prev_raw > ctx->prev2_raw) &&
                (ctx->prev_raw > raw)) {
                if(ctx->has_last_pulse==0u){ ctx->has_last_pulse=1u; ctx->last_pulse_timestamp_ms=ctx->prev_timestamp_ms; ctx->last_pulse_sample_index=ctx->prev_sample_index; }
                else {
                    uint32_t ibi = ctx->prev_timestamp_ms - ctx->last_pulse_timestamp_ms;
                    if((ibi>=(uint32_t)ctx->config.min_ibi_ms)&&(ibi<=(uint32_t)ctx->config.max_ibi_ms)){
                        ctx->beat_count += 1u;
                        event->timestamp_ms = ctx->prev_timestamp_ms; event->sample_index=ctx->prev_sample_index;
                        event->selected_channel=ctx->prev_selected_channel; event->signal_quality=ctx->prev_signal_quality; event->confidence=ctx->prev_signal_quality;
                        event->ibi_ms = (uint16_t)ibi; event->beat_count=ctx->beat_count;
                        event->state=PPG_IBI_STATE_TRACK; ctx->state=PPG_IBI_STATE_TRACK; event->reject_reason=PPG_IBI_REJECT_NONE;
                        ctx->last_pulse_timestamp_ms=ctx->prev_timestamp_ms; ctx->last_pulse_sample_index=ctx->prev_sample_index;
                        ctx->last_timestamp_ms=sample->timestamp_ms; ctx->has_last_timestamp=1u;
                        return PPG_IBI_STATUS_EVENT_READY;
                    }
                    event->reject_reason=PPG_IBI_REJECT_IBI_OUT_OF_RANGE;
                    ctx->state=PPG_IBI_STATE_REACQUIRE;
                    event->state=PPG_IBI_STATE_REACQUIRE;
                    ppg_ibi_reset_detector(ctx);
                }
            }
            if((event->reject_reason==PPG_IBI_REJECT_NONE) && (ctx->has_prev_sample!=0u)){
                ctx->has_prev2_sample=1u; ctx->prev2_raw=ctx->prev_raw; ctx->prev2_selected_channel=ctx->prev_selected_channel; ctx->prev2_signal_quality=ctx->prev_signal_quality; ctx->prev2_timestamp_ms=ctx->prev_timestamp_ms; ctx->prev2_sample_index=ctx->prev_sample_index;
            }
            if(event->reject_reason==PPG_IBI_REJECT_NONE){
                ctx->has_prev_sample=1u; ctx->prev_raw=raw; ctx->prev_selected_channel=event->selected_channel; ctx->prev_signal_quality=event->signal_quality; ctx->prev_timestamp_ms=sample->timestamp_ms; ctx->prev_sample_index=ctx->sample_counter;
            }
        }
    }
    ctx->last_timestamp_ms=sample->timestamp_ms; ctx->has_last_timestamp=1u; return PPG_IBI_STATUS_NO_EVENT;
}
const char *ppg_ibi_version(void){ return "0.5.0-m5-fix2"; }
size_t ppg_ibi_context_size(void){ return sizeof(ppg_ibi_context_t); }
