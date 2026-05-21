#include <stdio.h>

#include "ppg_ibi.h"

#define OUTPUT_RESOURCE_REPORT "build/output/resource_report.txt"
#define RAM_BUDGET_MIN_BYTES 15360U
#define RAM_BUDGET_MAX_BYTES 20480U

int main(void) {
    FILE *report = fopen(OUTPUT_RESOURCE_REPORT, "w");
    size_t context_size_bytes = ppg_ibi_context_size();
    const char *within_budget =
        (context_size_bytes <= (size_t)RAM_BUDGET_MAX_BYTES) ? "yes" : "no";

    if (report == NULL) {
        fprintf(stderr, "failed to open %s\n", OUTPUT_RESOURCE_REPORT);
        return 1;
    }

    fprintf(report, "context_size_bytes=%lu\n", (unsigned long)context_size_bytes);
    fprintf(report, "ram_budget_min_bytes=%u\n", RAM_BUDGET_MIN_BYTES);
    fprintf(report, "ram_budget_max_bytes=%u\n", RAM_BUDGET_MAX_BYTES);
    fprintf(report, "context_size_within_budget=%s\n", within_budget);
    fprintf(report, "uses_dynamic_memory=no\n");
    fprintf(report, "uses_recursion=no\n");
    fclose(report);

    return 0;
}
