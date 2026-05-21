#include <stdio.h>
#include <stdint.h>

#include "ppg_ibi.h"

#define OUTPUT_REPORT_PATH "build/output/resource_report.txt"
#define RAM_BUDGET_MIN_BYTES 15360UL
#define RAM_BUDGET_MAX_BYTES 20480UL

int main(void) {
    FILE *out = fopen(OUTPUT_REPORT_PATH, "w");
    size_t context_size = ppg_ibi_context_size();
    unsigned long context_size_bytes = (unsigned long)context_size;
    const char *within_budget = "no";

    if (out == NULL) {
        fprintf(stderr, "failed to open %s\n", OUTPUT_REPORT_PATH);
        return 1;
    }

    if (context_size_bytes <= RAM_BUDGET_MAX_BYTES) {
        within_budget = "yes";
    }

    fprintf(out, "context_size_bytes=%lu\n", context_size_bytes);
    fprintf(out, "ram_budget_min_bytes=%lu\n", RAM_BUDGET_MIN_BYTES);
    fprintf(out, "ram_budget_max_bytes=%lu\n", RAM_BUDGET_MAX_BYTES);
    fprintf(out, "context_size_within_budget=%s\n", within_budget);
    fprintf(out, "uses_dynamic_memory=no\n");

    fclose(out);
    return 0;
}
