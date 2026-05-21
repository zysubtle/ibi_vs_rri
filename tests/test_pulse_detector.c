#include "ppg_ibi.h"

#include <assert.h>

#include "../src/ppg_ibi_internal.h"

static ppg_ibi_sample_t make_sample(uint32_t ts, int32_t ch0, uint8_t allow)
{
    ppg_ibi_sample_t s;
    s.timestamp_ms = ts;
    s.ppg[0] = ch0;
    s.ppg[1] = ch0;
    s.ppg[2] = ch0;
    s.ppg[3] = ch0;
    s.allow_measure = allow;
    return s;
}

int main(void)
{
    ppg_ibi_context_t ctx;
    ppg_ibi_event_t event;

    assert(ppg_ibi_init(&ctx, NULL) == PPG_IBI_STATUS_OK);

    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1000u,{10,10,10,10},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1020u,{50,50,50,50},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1040u,{20,20,20,20},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);

    for (uint32_t ts = 1060u; ts <= 1340u; ts += 20u) {
        ppg_ibi_sample_t s = make_sample(ts, 5, 1u);
        assert(ppg_ibi_process(&ctx, &s, &event) == PPG_IBI_STATUS_NO_EVENT);
    }
    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1360u,{60,60,60,60},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1380u,{30,30,30,30},1u}, &event) == PPG_IBI_STATUS_EVENT_READY);
    assert(event.timestamp_ms == 1360u);
    assert(event.ibi_ms == 340u);
    assert(event.beat_count == 1u);

    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1400u,{5,5,5,5},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1420u,{40,40,40,40},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1440u,{10,10,10,10},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.reject_reason == PPG_IBI_REJECT_IBI_OUT_OF_RANGE);

    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1460u,{1,1,1,1},0u}, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1480u,{5,5,5,5},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1500u,{35,35,35,35},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx, &(ppg_ibi_sample_t){1520u,{10,10,10,10},1u}, &event) == PPG_IBI_STATUS_NO_EVENT);

    { ppg_ibi_sample_t s = make_sample(1540u, PPG_IBI_PPG_MAX_24BIT, 1u);
      assert(ppg_ibi_process(&ctx, &s, &event) == PPG_IBI_STATUS_NO_EVENT); }
    assert(event.reject_reason == PPG_IBI_REJECT_SATURATED);

    { ppg_ibi_sample_t s = make_sample(1580u, 100, 1u);
      assert(ppg_ibi_process(&ctx, &s, &event) == PPG_IBI_STATUS_NO_EVENT); }
    assert((event.reject_reason == PPG_IBI_REJECT_SAMPLE_DROP) || (event.reject_reason == PPG_IBI_REJECT_TIMESTAMP_GAP));

    { ppg_ibi_sample_t s = make_sample(1600u, PPG_IBI_PPG_MAX_24BIT - 1, 1u);
      assert(ppg_ibi_process(&ctx, &s, &event) == PPG_IBI_STATUS_NO_EVENT); }
    assert(event.reject_reason == PPG_IBI_REJECT_LOW_SIGNAL_QUALITY);

    return 0;
}
