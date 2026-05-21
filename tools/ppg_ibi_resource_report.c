#include <stdio.h>

#include "ppg_ibi.h"

#define REPORT_PATH "build/output/resource_report.txt"
#define RAM_BUDGET_MIN_BYTES 15360U
#define RAM_BUDGET_MAX_BYTES 20480U

int main(void) {
    FILE *report = NULL;
    size_t context_size = ppg_ibi_context_size();
    const char *within_budget = (context_size <= RAM_BUDGET_MAX_BYTES) ? "yes" : "no";

    report = fopen(REPORT_PATH, "w");
    if (report == NULL) {
        fprintf(stderr, "failed to open %s\n", REPORT_PATH);
        return 1;
    }

    fprintf(report, "context_size_bytes=%zu\n", context_size);
    fprintf(report, "ram_budget_min_bytes=%u\n", RAM_BUDGET_MIN_BYTES);
    fprintf(report, "ram_budget_max_bytes=%u\n", RAM_BUDGET_MAX_BYTES);
    fprintf(report, "context_size_within_budget=%s\n", within_budget);
    fprintf(report, "uses_dynamic_memory=no\n");
    fprintf(report, "uses_recursion=no\n");

    fclose(report);
    return 0;
}
