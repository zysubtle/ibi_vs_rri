#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppg_ibi.h"

#define INPUT_CSV_PATH "tests/fixtures/sample_ppg_20000.csv"
#define OUTPUT_EVENTS_PATH "build/output/ibi_events.csv"
#define OUTPUT_SUMMARY_PATH "build/output/smoke_summary.txt"
#define LINE_BUFFER_SIZE 512
#define EXPECTED_COLUMNS 6

static const char *status_to_string(ppg_ibi_status_t status) { switch (status) { case PPG_IBI_STATUS_OK: return "OK"; case PPG_IBI_STATUS_NO_EVENT: return "NO_EVENT"; case PPG_IBI_STATUS_EVENT_READY: return "EVENT_READY"; case PPG_IBI_STATUS_INVALID_ARGUMENT: return "INVALID_ARGUMENT"; case PPG_IBI_STATUS_NOT_INITIALIZED: return "NOT_INITIALIZED"; default: return "UNKNOWN_STATUS"; }}
static int parse_u32(const char *text, uint32_t *out_value) { unsigned long value; char *endptr = NULL; errno = 0; value = strtoul(text, &endptr, 10); if ((errno != 0) || (endptr == text) || (*endptr != '\0') || (value > UINT32_MAX)) { return -1; } *out_value = (uint32_t)value; return 0; }
static int parse_i32(const char *text, int32_t *out_value) { long value; char *endptr = NULL; errno = 0; value = strtol(text, &endptr, 10); if ((errno != 0) || (endptr == text) || (*endptr != '\0') || (value < INT32_MIN) || (value > INT32_MAX)) { return -1; } *out_value = (int32_t)value; return 0; }
static int parse_u8(const char *text, uint8_t *out_value) { unsigned long value; char *endptr = NULL; errno = 0; value = strtoul(text, &endptr, 10); if ((errno != 0) || (endptr == text) || (*endptr != '\0') || (value > UCHAR_MAX)) { return -1; } *out_value = (uint8_t)value; return 0; }
static int validate_header(char *line, int *has_allow_measure_column) {
    line[strcspn(line, "\r\n")] = '\0';
    if (strcmp(line, "timestamp_ms,PPG_G1,PPG_G2,PPG_G3,PPG_G4,allow_measure") == 0) { *has_allow_measure_column = 1; return 0; }
    if (strcmp(line, "timestamp_ms,PPG_G1,PPG_G2,PPG_G3,PPG_G4") == 0) { *has_allow_measure_column = 0; return 0; }
    return -1;
}

int main(void) {
    FILE *input = fopen(INPUT_CSV_PATH, "r"); FILE *events = NULL; FILE *summary = NULL; char line[LINE_BUFFER_SIZE];
    uint32_t total_samples = 0, parsed_samples = 0, invalid_lines = 0, allow_false_samples = 0, event_ready_count = 0, ibi_min_ms = 0, ibi_max_ms = 0, ibi_out_of_range_events = 0;
    uint32_t reject_allow_measure_false = 0, reject_low_signal_quality = 0, reject_saturated = 0, reject_timestamp_gap = 0, reject_sample_drop = 0, reject_ibi_out_of_range = 0;
    ppg_ibi_context_t ctx; ppg_ibi_config_t cfg; ppg_ibi_event_t event; ppg_ibi_status_t status = PPG_IBI_STATUS_OK;
    if (input == NULL) { fprintf(stderr, "S1: missing %s\n", INPUT_CSV_PATH); return 2; }
    if (fgets(line, sizeof(line), input) == NULL) { fprintf(stderr, "ERROR: empty CSV file: %s\n", INPUT_CSV_PATH); fclose(input); return 3; }
    int has_allow_measure_column = 0;
    if (validate_header(line, &has_allow_measure_column) != 0) { fprintf(stderr, "ERROR: unexpected CSV header in %s\n", INPUT_CSV_PATH); fclose(input); return 4; }
    ppg_ibi_config_default(&cfg); if (ppg_ibi_init(&ctx, &cfg) != PPG_IBI_STATUS_OK) { fprintf(stderr, "ERROR: ppg_ibi_init failed\n"); fclose(input); return 5; }
    events = fopen(OUTPUT_EVENTS_PATH, "w"); if (events == NULL) { fprintf(stderr, "ERROR: cannot open output file: %s\n", OUTPUT_EVENTS_PATH); fclose(input); return 6; }
    fprintf(events, "timestamp_ms,sample_index,ibi_ms,beat_count,confidence,signal_quality,selected_channel,state,reject_reason,debug_flags\n");
    while (fgets(line, sizeof(line), input) != NULL) {
        char *tokens[EXPECTED_COLUMNS]; char *cursor = line; char *next = NULL; int i = 0; ppg_ibi_sample_t sample;
        if (strchr(line, '\n') == NULL) { invalid_lines++; { int ch; while ((ch = fgetc(input)) != '\n' && ch != EOF) {} } continue; }
        line[strcspn(line, "\r\n")] = '\0'; total_samples++;
        for (i = 0; i < EXPECTED_COLUMNS; i++) { tokens[i] = cursor; next = strchr(cursor, ','); if ((next == NULL) && (i < EXPECTED_COLUMNS - 1)) { break; } if (next != NULL) { *next = '\0'; cursor = next + 1; } }
        if ((has_allow_measure_column && (i != EXPECTED_COLUMNS - 1 || strchr(cursor, ',') != NULL)) ||
            (!has_allow_measure_column && (i != EXPECTED_COLUMNS - 2 || strchr(cursor, ',') != NULL))) { invalid_lines++; continue; }
        if (parse_u32(tokens[0], &sample.timestamp_ms) != 0 || parse_i32(tokens[1], &sample.ppg[0]) != 0 || parse_i32(tokens[2], &sample.ppg[1]) != 0 || parse_i32(tokens[3], &sample.ppg[2]) != 0 || parse_i32(tokens[4], &sample.ppg[3]) != 0 || (has_allow_measure_column && parse_u8(tokens[5], &sample.allow_measure) != 0)) { invalid_lines++; continue; }
        if (!has_allow_measure_column) { sample.allow_measure = 1U; }
        parsed_samples++; if (sample.allow_measure == 0U) { allow_false_samples++; }
        status = ppg_ibi_process(&ctx, &sample, &event);
        switch (event.reject_reason) { case PPG_IBI_REJECT_ALLOW_MEASURE_FALSE: reject_allow_measure_false++; break; case PPG_IBI_REJECT_LOW_SIGNAL_QUALITY: reject_low_signal_quality++; break; case PPG_IBI_REJECT_SATURATED: reject_saturated++; break; case PPG_IBI_REJECT_TIMESTAMP_GAP: reject_timestamp_gap++; break; case PPG_IBI_REJECT_SAMPLE_DROP: reject_sample_drop++; break; case PPG_IBI_REJECT_IBI_OUT_OF_RANGE: reject_ibi_out_of_range++; break; default: break; }
        if (status == PPG_IBI_STATUS_EVENT_READY) { event_ready_count++; fprintf(events, "%" PRIu32 ",%" PRIu32 ",%u,%" PRIu32 ",%u,%u,%u,%d,%d,%" PRIu32 "\n", event.timestamp_ms, event.sample_index, (unsigned int)event.ibi_ms, event.beat_count, (unsigned int)event.confidence, (unsigned int)event.signal_quality, (unsigned int)event.selected_channel, (int)event.state, (int)event.reject_reason, event.debug_flags); if (event_ready_count == 1U) { ibi_min_ms = event.ibi_ms; ibi_max_ms = event.ibi_ms; } else { if (event.ibi_ms < ibi_min_ms) { ibi_min_ms = event.ibi_ms; } if (event.ibi_ms > ibi_max_ms) { ibi_max_ms = event.ibi_ms; } } if ((event.ibi_ms < PPG_IBI_MIN_IBI_MS) || (event.ibi_ms > PPG_IBI_MAX_IBI_MS)) { ibi_out_of_range_events++; } }
    }
    summary = fopen(OUTPUT_SUMMARY_PATH, "w"); if (summary == NULL) { fprintf(stderr, "ERROR: cannot open output file: %s\n", OUTPUT_SUMMARY_PATH); fclose(events); fclose(input); return 7; }
    fprintf(summary, "input_path=%s\n", INPUT_CSV_PATH); fprintf(summary, "total_samples=%" PRIu32 "\n", total_samples); fprintf(summary, "parsed_samples=%" PRIu32 "\n", parsed_samples); fprintf(summary, "invalid_lines=%" PRIu32 "\n", invalid_lines); fprintf(summary, "allow_measure_false_samples=%" PRIu32 "\n", allow_false_samples); fprintf(summary, "event_ready_count=%" PRIu32 "\n", event_ready_count);
    if (event_ready_count == 0U) { fprintf(summary, "ibi_min_ms=NA\n"); fprintf(summary, "ibi_max_ms=NA\n"); } else { fprintf(summary, "ibi_min_ms=%" PRIu32 "\n", ibi_min_ms); fprintf(summary, "ibi_max_ms=%" PRIu32 "\n", ibi_max_ms); }
    fprintf(summary, "ibi_out_of_range_events=%" PRIu32 "\n", ibi_out_of_range_events); fprintf(summary, "reject_allow_measure_false=%" PRIu32 "\n", reject_allow_measure_false); fprintf(summary, "reject_low_signal_quality=%" PRIu32 "\n", reject_low_signal_quality); fprintf(summary, "reject_saturated=%" PRIu32 "\n", reject_saturated); fprintf(summary, "reject_timestamp_gap=%" PRIu32 "\n", reject_timestamp_gap); fprintf(summary, "reject_sample_drop=%" PRIu32 "\n", reject_sample_drop); fprintf(summary, "reject_ibi_out_of_range=%" PRIu32 "\n", reject_ibi_out_of_range); fprintf(summary, "final_status=%s\n", status_to_string(status));
    fclose(summary); fclose(events); fclose(input); return 0;
}
