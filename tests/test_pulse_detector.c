#include "ppg_ibi.h"

#include <assert.h>

static ppg_ibi_sample_t make_sample(uint32_t ts, int32_t c0, int32_t c1, int32_t c2, int32_t c3, uint8_t allow)
{
    ppg_ibi_sample_t s;
    s.timestamp_ms = ts;
    s.ppg[0] = c0;
    s.ppg[1] = c1;
    s.ppg[2] = c2;
    s.ppg[3] = c3;
    s.allow_measure = allow;
    return s;
}

int main(void)
{
    ppg_ibi_context_t ctx;
    ppg_ibi_event_t event;
    ppg_ibi_sample_t sample;

    assert(ppg_ibi_init(&ctx, NULL) == PPG_IBI_STATUS_OK);

    sample = make_sample(0u, 100, 90, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);

    sample = make_sample(20u, 200, 90, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);

    sample = make_sample(40u, 100, 90, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);

    sample = make_sample(360u, 120, 90, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);

    sample = make_sample(380u, 220, 90, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);

    sample = make_sample(400u, 130, 90, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_EVENT_READY);
    assert(event.timestamp_ms == 380u);
    assert(event.sample_index == 5u);
    assert(event.selected_channel == 0u);
    assert(event.signal_quality == 80u);
    assert(event.confidence == 80u);
    assert(event.ibi_ms == 360u);

    sample = make_sample(420u, 100, 300, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    sample = make_sample(440u, 100, 200, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    sample = make_sample(460u, 100, 250, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);

    sample = make_sample(480u, 120, 90, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    sample = make_sample(500u, 220, 90, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    sample = make_sample(520u, 130, 90, 80, 70, 1u);
    assert(ppg_ibi_process(&ctx, &sample, &event) == PPG_IBI_STATUS_NO_EVENT);
    assert(event.reject_reason == PPG_IBI_REJECT_IBI_OUT_OF_RANGE);

    return 0;
}
