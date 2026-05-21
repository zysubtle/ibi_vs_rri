#include "ppg_ibi.h"

#include <assert.h>
#include "../src/ppg_ibi_internal.h"

static ppg_ibi_sample_t mk(uint32_t ts, int32_t raw, uint8_t allow) {
    ppg_ibi_sample_t s;
    s.timestamp_ms = ts;
    s.ppg[0] = raw;
    s.ppg[1] = raw;
    s.ppg[2] = raw;
    s.ppg[3] = raw;
    s.allow_measure = allow;
    return s;
}

int main(void) {
    ppg_ibi_context_t ctx;
    ppg_ibi_event_t e;
    ppg_ibi_sample_t s;

    assert(ppg_ibi_init(&ctx, NULL) == PPG_IBI_STATUS_OK);

    s = mk(1000u, 100, 1u);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.state == PPG_IBI_STATE_ACQUIRE);

    s = mk(1020u, 100, 0u);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.state == PPG_IBI_STATE_HOLD);
    assert(ctx.has_prev_sample == 0u);
    assert(ctx.has_last_pulse == 0u);

    s = mk(1040u, 100, 1u);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.state == PPG_IBI_STATE_REACQUIRE);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);

    s = mk(2000u, 100, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = mk(2020u, 300, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = mk(2040u, 100, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.has_last_pulse == 1u);
    assert(ctx.beat_count == 0u);

    for (uint32_t ts = 2060u; ts <= 2320u; ts += 20u) {
        s = mk(ts, 100, 1u);
        assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    }
    s = mk(2340u, 300, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = mk(2360u, 100, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_EVENT_READY);
    assert(e.state == PPG_IBI_STATE_TRACK);
    assert(ctx.state == PPG_IBI_STATE_TRACK);

    s = mk(2380u, PPG_IBI_PPG_MAX_24BIT, 1u);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.state == PPG_IBI_STATE_REACQUIRE);
    assert(e.reject_reason == PPG_IBI_REJECT_SATURATED);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);
    s = mk(3000u, 100, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = mk(3020u, 300, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = mk(3040u, 100, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = mk(3060u, 300, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = mk(3080u, 100, 1u); assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_IBI_OUT_OF_RANGE);
    assert(ctx.state == PPG_IBI_STATE_REACQUIRE);

    return 0;
}
