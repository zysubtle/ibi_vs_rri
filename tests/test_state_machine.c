#include "ppg_ibi.h"

#include <assert.h>

#include "../src/ppg_ibi_internal.h"

static ppg_ibi_sample_t make_sample(uint32_t ts, int32_t c0, int32_t c1, int32_t c2, int32_t c3, uint8_t allow) {
    ppg_ibi_sample_t s;
    s.timestamp_ms = ts;
    s.ppg[0] = c0;
    s.ppg[1] = c1;
    s.ppg[2] = c2;
    s.ppg[3] = c3;
    s.allow_measure = allow;
    return s;
}

int main(void) {
    ppg_ibi_context_t ctx;
    ppg_ibi_event_t e;
    ppg_ibi_sample_t s;
    assert(ppg_ibi_init(&ctx, NULL) == PPG_IBI_STATUS_OK);

    s = make_sample(1000, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(1020, 250, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(1040, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);

    s = make_sample(1060, 260, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(1080, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_IBI_OUT_OF_RANGE);
    assert(e.state == PPG_IBI_STATE_REACQUIRE);
    assert(ctx.state == PPG_IBI_STATE_REACQUIRE);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);
    s = make_sample(2000, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(2020, 250, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(2040, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    for (uint32_t ts = 2060; ts < 2340; ts += 20) {
        s = make_sample(ts, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    }
    s = make_sample(2340, 280, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(2360, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_EVENT_READY);
    s = make_sample(2380, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason != PPG_IBI_REJECT_IBI_OUT_OF_RANGE);
    for (uint32_t ts = 2400; ts < 2700; ts += 20) {
        s = make_sample(ts, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    }
    s = make_sample(2700, 300, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(2720, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_EVENT_READY);

    s = make_sample(2740, 100, 100, 100, 100, 0); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_ALLOW_MEASURE_FALSE);
    s = make_sample(2760, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(2770, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_TIMESTAMP_GAP);
    s = make_sample(2810, 100, 100, 100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_SAMPLE_DROP);
    s = make_sample(2830, PPG_IBI_PPG_MAX_24BIT - 4, PPG_IBI_PPG_MAX_24BIT - 4, PPG_IBI_PPG_MAX_24BIT - 4, PPG_IBI_PPG_MAX_24BIT - 4, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_LOW_SIGNAL_QUALITY);
    return 0;
}
