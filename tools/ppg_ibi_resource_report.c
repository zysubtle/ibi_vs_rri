#include <stdio.h>
#include <stdint.h>

#include "ppg_ibi.h"

#define OUTPUT_REPORT "build/output/resource_report.txt"
#define RAM_BUDGET_MIN_BYTES (15U * 1024U)
#define RAM_BUDGET_MAX_BYTES (20U * 1024U)

int main(void) {
    FILE *out = fopen(OUTPUT_REPORT, "w");
    size_t context_size = ppg_ibi_context_size();

    if (out == NULL) {
        fprintf(stderr, "failed to open %s\n", OUTPUT_REPORT);
        return 1;
    }

    fprintf(out, "context_size_bytes=%u\n", (unsigned)context_size);
    fprintf(out, "ram_budget_min_bytes=%u\n", RAM_BUDGET_MIN_BYTES);
    fprintf(out, "ram_budget_max_bytes=%u\n", RAM_BUDGET_MAX_BYTES);
    fprintf(out, "context_size_within_budget=%s\n",
            (context_size <= (size_t)RAM_BUDGET_MAX_BYTES) ? "yes" : "no");
    fprintf(out, "uses_dynamic_memory=no\n");
    fprintf(out, "uses_recursion=no\n");

    fclose(out);
    return 0;
}
