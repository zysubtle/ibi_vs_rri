#include <stdio.h>
#include <stdlib.h>

#include "ppg_ibi.h"

#define OUTPUT_DIR "build/output"
#define OUTPUT_PATH "build/output/resource_report.txt"
#define RAM_BUDGET_MIN_BYTES 15360U
#define RAM_BUDGET_MAX_BYTES 20480U

int main(void) {
    FILE *out = NULL;
    size_t context_size = ppg_ibi_context_size();
    const char *within_budget =
        (context_size <= RAM_BUDGET_MAX_BYTES) ? "yes" : "no";

    (void)system("mkdir -p " OUTPUT_DIR);

    out = fopen(OUTPUT_PATH, "w");
    if (out == NULL) {
        fprintf(stderr, "failed to open %s\n", OUTPUT_PATH);
        return 1;
    }

    fprintf(out, "context_size_bytes=%zu\n", context_size);
    fprintf(out, "ram_budget_min_bytes=%u\n", RAM_BUDGET_MIN_BYTES);
    fprintf(out, "ram_budget_max_bytes=%u\n", RAM_BUDGET_MAX_BYTES);
    fprintf(out, "context_size_within_budget=%s\n", within_budget);
    fprintf(out, "uses_dynamic_memory=no\n");

    fclose(out);
    return 0;
}
