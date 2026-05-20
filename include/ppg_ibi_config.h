#ifndef PPG_IBI_CONFIG_H
#define PPG_IBI_CONFIG_H

#include <stdint.h>

#define PPG_IBI_CHANNEL_COUNT (4u)
#define PPG_IBI_SAMPLE_RATE_HZ (50u)
#define PPG_IBI_EXPECTED_INTERVAL_MS (20u)
#define PPG_IBI_MIN_IBI_MS (300u)
#define PPG_IBI_MAX_IBI_MS (2000u)

typedef struct {
    uint16_t sample_rate_hz;
    uint16_t expected_interval_ms;
    uint16_t min_ibi_ms;
    uint16_t max_ibi_ms;
    uint8_t allow_timestamp_strict_check;
} ppg_ibi_config_t;

#endif
