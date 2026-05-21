#include "ppg_ibi.h"
#include <assert.h>
#include "../src/ppg_ibi_internal.h"

static ppg_ibi_sample_t make_sample(uint32_t ts, int32_t raw, uint8_t allow) {
    ppg_ibi_sample_t s;
    s.timestamp_ms = ts;
    s.ppg[0] = raw;
    s.ppg[1] = 100;
    s.ppg[2] = 100;
    s.ppg[3] = 100;
    s.allow_measure = allow;
    return s;
}

int main(void) {
    ppg_ibi_context_t ctx;
    ppg_ibi_event_t e;
    ppg_ibi_sample_t s;
    assert(ppg_ibi_init(&ctx, NULL) == PPG_IBI_STATUS_OK);

    s = make_sample(1000, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(1020, 100, 0); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.state == PPG_IBI_STATE_HOLD); assert(ctx.state == PPG_IBI_STATE_HOLD);
    assert(ctx.has_prev_sample == 0u); assert(ctx.has_prev2_sample == 0u); assert(ctx.has_last_pulse == 0u);

    s = make_sample(1040, PPG_IBI_PPG_MAX_24BIT, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_SATURATED);
    assert(e.state == PPG_IBI_STATE_REACQUIRE); assert(ctx.state == PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample == 0u); assert(ctx.has_prev2_sample == 0u); assert(ctx.has_last_pulse == 0u);

    s = make_sample(1059, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_TIMESTAMP_GAP);
    assert(e.state == PPG_IBI_STATE_REACQUIRE); assert(ctx.state == PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample == 0u); assert(ctx.has_prev2_sample == 0u); assert(ctx.has_last_pulse == 0u);

    s = make_sample(1100, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_SAMPLE_DROP);
    assert(e.state == PPG_IBI_STATE_REACQUIRE); assert(ctx.state == PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample == 0u); assert(ctx.has_prev2_sample == 0u); assert(ctx.has_last_pulse == 0u);

    s = make_sample(1120, PPG_IBI_PPG_MAX_24BIT - 1, 1);
    s.ppg[1] = PPG_IBI_PPG_MAX_24BIT - 1; s.ppg[2] = PPG_IBI_PPG_MAX_24BIT - 1; s.ppg[3] = PPG_IBI_PPG_MAX_24BIT - 1;
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_LOW_SIGNAL_QUALITY);
    assert(e.state == PPG_IBI_STATE_REACQUIRE); assert(ctx.state == PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample == 0u); assert(ctx.has_prev2_sample == 0u); assert(ctx.has_last_pulse == 0u);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);
    s = make_sample(2000, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(2020, 250, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(2040, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);

    for (uint32_t ts = 2060; ts <= 2320; ts += 20) {
        s = make_sample(ts, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    }

    s = make_sample(2340, 300, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(2360, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_EVENT_READY);

    s = make_sample(2380, 150, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason != PPG_IBI_REJECT_IBI_OUT_OF_RANGE);

    for (uint32_t ts = 2400; ts <= 2660; ts += 20) {
        s = make_sample(ts, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    }
    s = make_sample(2680, 320, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(2700, 100, 1); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_EVENT_READY);

    return 0;
}
