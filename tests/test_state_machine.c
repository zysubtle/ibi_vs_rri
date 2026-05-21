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

static void seed_track_state(ppg_ibi_context_t *ctx, ppg_ibi_event_t *e, uint32_t ts) {
    ppg_ibi_sample_t s;
    s = make_sample(ts, 100, 100, 100, 100, 1);
    assert(ppg_ibi_process(ctx, &s, e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(ts + 20, 250, 100, 100, 100, 1);
    assert(ppg_ibi_process(ctx, &s, e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(ts + 40, 100, 100, 100, 100, 1);
    assert(ppg_ibi_process(ctx, &s, e) == PPG_IBI_STATUS_NO_EVENT);
    assert(ctx->has_last_pulse == 1u);
}

static void assert_detector_reset(const ppg_ibi_context_t *ctx) {
    assert(ctx->has_prev_sample == 0u);
    assert(ctx->has_prev2_sample == 0u);
    assert(ctx->has_last_pulse == 0u);
}

int main(void) {
    ppg_ibi_context_t ctx;
    ppg_ibi_event_t e;
    ppg_ibi_sample_t s;
    uint32_t ts;

    assert(ppg_ibi_init(&ctx, NULL) == PPG_IBI_STATUS_OK);

    seed_track_state(&ctx, &e, 1000);
    ctx.state = PPG_IBI_STATE_TRACK;

    s = make_sample(1060, 100, 100, 100, 100, 0);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_ALLOW_MEASURE_FALSE);
    assert(e.state == PPG_IBI_STATE_HOLD);
    assert(ctx.state == PPG_IBI_STATE_HOLD);
    assert_detector_reset(&ctx);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);
    seed_track_state(&ctx, &e, 2000);
    ctx.state = PPG_IBI_STATE_TRACK;

    s = make_sample(2060, PPG_IBI_PPG_MAX_24BIT, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_SATURATED);
    assert(e.state == PPG_IBI_STATE_REACQUIRE);
    assert(ctx.state == PPG_IBI_STATE_REACQUIRE);
    assert_detector_reset(&ctx);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);
    seed_track_state(&ctx, &e, 3000);
    ctx.state = PPG_IBI_STATE_TRACK;
    s = make_sample(3059, 100, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_TIMESTAMP_GAP);
    assert(e.state == PPG_IBI_STATE_REACQUIRE);
    assert(ctx.state == PPG_IBI_STATE_REACQUIRE);
    assert_detector_reset(&ctx);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);
    seed_track_state(&ctx, &e, 4000);
    ctx.state = PPG_IBI_STATE_TRACK;
    s = make_sample(4080, 100, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_SAMPLE_DROP);
    assert(e.state == PPG_IBI_STATE_REACQUIRE);
    assert(ctx.state == PPG_IBI_STATE_REACQUIRE);
    assert_detector_reset(&ctx);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);
    seed_track_state(&ctx, &e, 5000);
    ctx.state = PPG_IBI_STATE_TRACK;
    s = make_sample(5060, PPG_IBI_PPG_MAX_24BIT - 5, PPG_IBI_PPG_MAX_24BIT - 5, PPG_IBI_PPG_MAX_24BIT - 5, PPG_IBI_PPG_MAX_24BIT - 5, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason == PPG_IBI_REJECT_LOW_SIGNAL_QUALITY);
    assert(e.state == PPG_IBI_STATE_REACQUIRE);
    assert(ctx.state == PPG_IBI_STATE_REACQUIRE);
    assert_detector_reset(&ctx);

    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);
    s = make_sample(6000, 100, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(6020, 250, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(6040, 100, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);

    ts = 6060;
    for (int i = 0; i < 14; ++i) {
        s = make_sample(ts, 100, 100, 100, 100, 1);
        assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
        ts += 20;
    }
    s = make_sample(6340, 280, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(6360, 100, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_EVENT_READY);

    s = make_sample(6280, 100, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason != PPG_IBI_REJECT_IBI_OUT_OF_RANGE);

    ts = 6380;
    for (int i = 0; i < 27; ++i) {
        s = make_sample(ts, 100, 100, 100, 100, 1);
        assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
        ts += 20;
    }
    s = make_sample(6940, 260, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_NO_EVENT);
    s = make_sample(6960, 100, 100, 100, 100, 1);
    assert(ppg_ibi_process(&ctx, &s, &e) == PPG_IBI_STATUS_EVENT_READY);

    return 0;
}
