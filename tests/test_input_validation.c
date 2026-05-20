#include "ppg_ibi.h"

#include <assert.h>

#include "../src/ppg_ibi_internal.h"

static ppg_ibi_sample_t make_sample(uint32_t ts, int32_t v, uint8_t allow)
{
    ppg_ibi_sample_t s;
    s.timestamp_ms = ts;
    s.ppg[0] = v;
    s.ppg[1] = v;
    s.ppg[2] = v;
    s.ppg[3] = v;
    s.allow_measure = allow;
    return s;
}

int main(void)
{
    ppg_ibi_context_t ctx;
    ppg_ibi_event_t event;
    ppg_ibi_sample_t sample;

    assert(ppg_ibi_init(&ctx, NULL) == PPG_IBI_STATUS_OK);

    sample = make_sample(1000u, 100, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.sample_index == 1u);
    assert(event.ibi_ms == 0u);
    assert(event.state == PPG_IBI_STATE_ACQUIRE);
    assert(event.reject_reason == PPG_IBI_REJECT_NONE);

    sample.timestamp_ms = 1020u;
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.sample_index == 2u);
    assert(event.reject_reason == PPG_IBI_REJECT_NONE);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);
    sample = make_sample(2000u, 100, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.sample_index == 1u);

    sample.allow_measure = 0u;
    sample.timestamp_ms = 2020u;
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.ibi_ms == 0u);
    assert(event.reject_reason == PPG_IBI_REJECT_ALLOW_MEASURE_FALSE);
    assert(event.state == PPG_IBI_STATE_HOLD);
    assert((event.debug_flags & PPG_IBI_DEBUG_FLAG_ALLOW_MEASURE_OFF) != 0u);

    sample.allow_measure = 1u;
    sample.timestamp_ms = 2040u;
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.state == PPG_IBI_STATE_REACQUIRE);

    sample.timestamp_ms = 2030u;
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert((event.reject_reason == PPG_IBI_REJECT_TIMESTAMP_GAP) ||
           (event.reject_reason == PPG_IBI_REJECT_SAMPLE_DROP));
    assert(((event.debug_flags & PPG_IBI_DEBUG_FLAG_TIMESTAMP_GAP) != 0u) ||
           ((event.debug_flags & PPG_IBI_DEBUG_FLAG_SAMPLE_DROP) != 0u));

    sample.timestamp_ms = 2100u;
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert((event.reject_reason == PPG_IBI_REJECT_SAMPLE_DROP) ||
           ((event.debug_flags & PPG_IBI_DEBUG_FLAG_SAMPLE_DROP) != 0u));

    sample = make_sample(2120u, PPG_IBI_PPG_MAX_24BIT, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.reject_reason == PPG_IBI_REJECT_SATURATED);
    assert((event.debug_flags & PPG_IBI_DEBUG_FLAG_PPG_SATURATED) != 0u);

    sample = make_sample(2140u, PPG_IBI_PPG_MIN_24BIT, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.reject_reason == PPG_IBI_REJECT_SATURATED);

    sample = make_sample(2160u, PPG_IBI_PPG_MAX_24BIT + 1, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.reject_reason == PPG_IBI_REJECT_SATURATED);

    assert(event.ibi_ms == 0u);

    return 0;
}
