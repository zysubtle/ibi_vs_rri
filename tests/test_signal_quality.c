#include "ppg_ibi.h"

#include <assert.h>

#include "../src/ppg_ibi_internal.h"

static ppg_ibi_sample_t make_sample(uint32_t ts,
                                    int32_t ch0,
                                    int32_t ch1,
                                    int32_t ch2,
                                    int32_t ch3,
                                    uint8_t allow)
{
    ppg_ibi_sample_t s;
    s.timestamp_ms = ts;
    s.ppg[0] = ch0;
    s.ppg[1] = ch1;
    s.ppg[2] = ch2;
    s.ppg[3] = ch3;
    s.allow_measure = allow;
    return s;
}

int main(void)
{
    ppg_ibi_context_t ctx;
    ppg_ibi_event_t event;
    ppg_ibi_sample_t sample;

    assert(ppg_ibi_init(&ctx, NULL) == PPG_IBI_STATUS_OK);

    sample = make_sample(1000u, 100, 110, 120, 130, 0u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.selected_channel == PPG_IBI_SELECTED_CHANNEL_INVALID);
    assert(event.signal_quality == 0u);
    assert(event.ibi_ms == 0u);

    sample = make_sample(1020u, 200, 210, 220, 230, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.selected_channel == 0u);
    assert(event.signal_quality == PPG_IBI_CHANNEL_QUALITY_BASIC_VALID);
    assert((event.debug_flags & PPG_IBI_DEBUG_FLAG_CHANNEL_SELECTED) != 0u);
    assert(event.ibi_ms == 0u);

    sample = make_sample(1040u,
                         PPG_IBI_PPG_MAX_24BIT - (PPG_IBI_PPG_NEAR_SATURATION_MARGIN / 2),
                         400,
                         PPG_IBI_PPG_MIN_24BIT + (PPG_IBI_PPG_NEAR_SATURATION_MARGIN / 2),
                         500,
                         1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.selected_channel == 1u);
    assert(event.signal_quality == PPG_IBI_CHANNEL_QUALITY_BASIC_VALID);

    sample = make_sample(1060u,
                         PPG_IBI_PPG_MAX_24BIT - 1,
                         PPG_IBI_PPG_MAX_24BIT - 2,
                         PPG_IBI_PPG_MIN_24BIT + 1,
                         PPG_IBI_PPG_MIN_24BIT + 2,
                         1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.signal_quality == PPG_IBI_CHANNEL_QUALITY_LOW);
    assert(event.reject_reason == PPG_IBI_REJECT_LOW_SIGNAL_QUALITY);
    assert((event.debug_flags & PPG_IBI_DEBUG_FLAG_LOW_SIGNAL_QUALITY) != 0u);

    sample = make_sample(1080u, PPG_IBI_PPG_MAX_24BIT, 100, 100, 100, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.reject_reason == PPG_IBI_REJECT_SATURATED);
    assert((event.debug_flags & PPG_IBI_DEBUG_FLAG_PPG_SATURATED) != 0u);

    assert(event.ibi_ms == 0u);
    assert(event.confidence == 0u);

    return 0;
}
