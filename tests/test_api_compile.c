#include "ppg_ibi.h"

#include <assert.h>
#include <stddef.h>

int main(void)
{
    ppg_ibi_context_t ctx;
    ppg_ibi_sample_t sample;
    ppg_ibi_event_t event;
    ppg_ibi_status_t status;

    ppg_ibi_config_t cfg;
    ppg_ibi_config_default(&cfg);
    assert(cfg.sample_rate_hz == PPG_IBI_SAMPLE_RATE_HZ);
    assert(cfg.expected_interval_ms == PPG_IBI_EXPECTED_INTERVAL_MS);
    assert(cfg.min_ibi_ms == PPG_IBI_MIN_IBI_MS);
    assert(cfg.max_ibi_ms == PPG_IBI_MAX_IBI_MS);
    assert(cfg.allow_timestamp_strict_check == 1u);

    assert(ppg_ibi_context_size() > 0u);
    assert(ppg_ibi_init(NULL, &cfg) == PPG_IBI_STATUS_INVALID_ARGUMENT);
    assert(ppg_ibi_reset(NULL) == PPG_IBI_STATUS_INVALID_ARGUMENT);
    assert(ppg_ibi_process(NULL, &sample, &event) == PPG_IBI_STATUS_INVALID_ARGUMENT);

    assert(ppg_ibi_init(&ctx, NULL) == PPG_IBI_STATUS_OK);
    assert(ppg_ibi_reset(&ctx) == PPG_IBI_STATUS_OK);

    sample.timestamp_ms = 1000u;
    sample.ppg[0] = 0;
    sample.ppg[1] = 0;
    sample.ppg[2] = 0;
    sample.ppg[3] = 0;
    sample.allow_measure = 0u;

    status = ppg_ibi_process(&ctx, &sample, &event);
    assert(status == PPG_IBI_STATUS_NO_EVENT);
    assert(event.ibi_ms == 0u);
    assert(event.reject_reason == PPG_IBI_REJECT_ALLOW_MEASURE_FALSE);
    assert(event.state == PPG_IBI_STATE_HOLD);

    sample.allow_measure = 1u;
    sample.timestamp_ms += PPG_IBI_EXPECTED_INTERVAL_MS;
    status = ppg_ibi_process(&ctx, &sample, &event);
    assert(status == PPG_IBI_STATUS_NO_EVENT);
    assert(event.ibi_ms == 0u);
    assert(event.state == PPG_IBI_STATE_REACQUIRE);

    assert(ppg_ibi_version() != NULL);
    return 0;
}
