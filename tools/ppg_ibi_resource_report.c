#include <stdio.h>

#include "ppg_ibi.h"

#define OUTPUT_PATH "build/output/resource_report.txt"
#define RAM_BUDGET_MIN_BYTES 15360U
#define RAM_BUDGET_MAX_BYTES 20480U

int main(void) {
    FILE *out = fopen(OUTPUT_PATH, "w");
    size_t context_size = ppg_ibi_context_size();
    int within_budget = (context_size >= RAM_BUDGET_MIN_BYTES &&
                         context_size <= RAM_BUDGET_MAX_BYTES)
                            ? 1
                            : 0;

    if (out == NULL) {
        fprintf(stderr, "failed to open %s\n", OUTPUT_PATH);
        return 1;
    }

    fprintf(out, "context_size_bytes=%zu\n", context_size);
    fprintf(out, "ram_budget_min_bytes=%u\n", RAM_BUDGET_MIN_BYTES);
    fprintf(out, "ram_budget_max_bytes=%u\n", RAM_BUDGET_MAX_BYTES);
    fprintf(out, "context_size_within_budget=%s\n",
            within_budget ? "yes" : "no");
    fprintf(out, "uses_dynamic_memory=no\n");

    fclose(out);
    return 0;
}
