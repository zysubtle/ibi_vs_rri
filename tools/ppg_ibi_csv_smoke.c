#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppg_ibi.h"

#define INPUT_PATH "tests/fixtures/sample_ppg_20000.csv"
#define OUTPUT_DIR "build/output"
#define OUTPUT_EVENTS "build/output/ibi_events.csv"
#define OUTPUT_SUMMARY "build/output/smoke_summary.txt"
#define LINE_BUF_SIZE 512

static int parse_line(const char *line, ppg_ibi_sample_t *sample) {
    unsigned long timestamp = 0UL;
    long p0 = 0L, p1 = 0L, p2 = 0L, p3 = 0L;
    unsigned long allow = 0UL;
    char extra = '\0';
    int scanned = sscanf(line, " %lu , %ld , %ld , %ld , %ld , %lu %c",
                         &timestamp, &p0, &p1, &p2, &p3, &allow, &extra);

    if (scanned != 6) {
        return -1;
    }

    sample->timestamp_ms = (uint32_t)timestamp;
    sample->ppg[0] = (int32_t)p0;
    sample->ppg[1] = (int32_t)p1;
    sample->ppg[2] = (int32_t)p2;
    sample->ppg[3] = (int32_t)p3;
    sample->allow_measure = (uint8_t)(allow != 0UL);
    return 0;
}

int main(void) {
    FILE *in = fopen(INPUT_PATH, "r");
    FILE *events = NULL;
    FILE *summary = NULL;
    ppg_ibi_context_t ctx;
    ppg_ibi_config_t config;
    char line[LINE_BUF_SIZE];
    uint32_t total_samples = 0U, parsed_samples = 0U, invalid_lines = 0U;
    uint32_t allow_false = 0U, event_ready_count = 0U;
    uint32_t reject_allow = 0U, reject_low = 0U, reject_sat = 0U;
    uint32_t reject_gap = 0U, reject_drop = 0U, reject_ibi = 0U;
    uint32_t ibi_min = 0U, ibi_max = 0U, ibi_oor = 0U;
    ppg_ibi_status_t status = PPG_IBI_STATUS_OK;

    if (in == NULL) {
        fprintf(stderr, "S1: missing %s\n", INPUT_PATH);
        return 1;
    }

    if (fgets(line, sizeof(line), in) == NULL) {
        fclose(in);
        fprintf(stderr, "empty input csv\n");
        return 1;
    }

    if (strcmp(line, "timestamp_ms,PPG_G1,PPG_G2,PPG_G3,PPG_G4,allow_measure\n") != 0 &&
        strcmp(line, "timestamp_ms,PPG_G1,PPG_G2,PPG_G3,PPG_G4,allow_measure\r\n") != 0) {
        fclose(in);
        fprintf(stderr, "invalid header, expect 6 columns with allow_measure\n");
        return 1;
    }

    (void)system("mkdir -p " OUTPUT_DIR);
    events = fopen(OUTPUT_EVENTS, "w");
    summary = fopen(OUTPUT_SUMMARY, "w");
    if (events == NULL || summary == NULL) {
        fclose(in);
        if (events != NULL) fclose(events);
        if (summary != NULL) fclose(summary);
        fprintf(stderr, "failed to open output files\n");
        return 1;
    }

    fprintf(events, "timestamp_ms,sample_index,ibi_ms,beat_count,confidence,signal_quality,selected_channel,state,reject_reason,debug_flags\n");

    ppg_ibi_config_default(&config);
    status = ppg_ibi_init(&ctx, &config);
    if (status != PPG_IBI_STATUS_OK) {
        fprintf(stderr, "init failed\n");
        fclose(in); fclose(events); fclose(summary);
        return 1;
    }

    while (fgets(line, sizeof(line), in) != NULL) {
        ppg_ibi_sample_t sample;
        ppg_ibi_event_t event;

        total_samples++;
        if (parse_line(line, &sample) != 0) {
            invalid_lines++;
            continue;
        }
        parsed_samples++;
        if (sample.allow_measure == 0U) {
            allow_false++;
        }

        status = ppg_ibi_process(&ctx, &sample, &event);
        if (event.reject_reason == PPG_IBI_REJECT_ALLOW_MEASURE_FALSE) reject_allow++;
        if (event.reject_reason == PPG_IBI_REJECT_LOW_SIGNAL_QUALITY) reject_low++;
        if (event.reject_reason == PPG_IBI_REJECT_SATURATED) reject_sat++;
        if (event.reject_reason == PPG_IBI_REJECT_TIMESTAMP_GAP) reject_gap++;
        if (event.reject_reason == PPG_IBI_REJECT_SAMPLE_DROP) reject_drop++;
        if (event.reject_reason == PPG_IBI_REJECT_IBI_OUT_OF_RANGE) reject_ibi++;

        if (status == PPG_IBI_STATUS_EVENT_READY) {
            uint32_t ibi = (uint32_t)event.ibi_ms;
            event_ready_count++;
            if (event_ready_count == 1U || ibi < ibi_min) ibi_min = ibi;
            if (event_ready_count == 1U || ibi > ibi_max) ibi_max = ibi;
            if (ibi < PPG_IBI_MIN_IBI_MS || ibi > PPG_IBI_MAX_IBI_MS) ibi_oor++;

            fprintf(events, "%u,%u,%u,%u,%u,%u,%u,%d,%d,%u\n",
                    event.timestamp_ms, event.sample_index, event.ibi_ms,
                    event.beat_count, event.confidence, event.signal_quality,
                    event.selected_channel, (int)event.state,
                    (int)event.reject_reason, event.debug_flags);
        }
    }

    fprintf(summary, "input_path=%s\n", INPUT_PATH);
    fprintf(summary, "total_samples=%u\n", total_samples);
    fprintf(summary, "parsed_samples=%u\n", parsed_samples);
    fprintf(summary, "invalid_lines=%u\n", invalid_lines);
    fprintf(summary, "allow_measure_false_samples=%u\n", allow_false);
    fprintf(summary, "event_ready_count=%u\n", event_ready_count);
    if (event_ready_count == 0U) {
        fprintf(summary, "ibi_min_ms=NA\nibi_max_ms=NA\n");
    } else {
        fprintf(summary, "ibi_min_ms=%u\nibi_max_ms=%u\n", ibi_min, ibi_max);
    }
    fprintf(summary, "ibi_out_of_range_events=%u\n", ibi_oor);
    fprintf(summary, "reject_allow_measure_false=%u\n", reject_allow);
    fprintf(summary, "reject_low_signal_quality=%u\n", reject_low);
    fprintf(summary, "reject_saturated=%u\n", reject_sat);
    fprintf(summary, "reject_timestamp_gap=%u\n", reject_gap);
    fprintf(summary, "reject_sample_drop=%u\n", reject_drop);
    fprintf(summary, "reject_ibi_out_of_range=%u\n", reject_ibi);
    fprintf(summary, "final_status=%d\n", (int)status);

    fclose(in);
    fclose(events);
    fclose(summary);
    return 0;
}
