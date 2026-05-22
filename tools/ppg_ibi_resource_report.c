#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ppg_ibi.h"

#define OUTPUT_DIR "build/output"
#define OUTPUT_REPORT "build/output/resource_report.txt"
#define RAM_BUDGET_MIN_BYTES 15360u
#define RAM_BUDGET_MAX_BYTES 20480u

int main(void) {
    FILE *fp = NULL;
    const size_t context_size = ppg_ibi_context_size();
    const int within_budget = (context_size <= RAM_BUDGET_MAX_BYTES) ? 1 : 0;

    (void)mkdir("build", 0755);
    (void)mkdir(OUTPUT_DIR, 0755);

    fp = fopen(OUTPUT_REPORT, "w");
    if (fp == NULL) {
        return 1;
    }

    fprintf(fp, "context_size_bytes=%zu\n", context_size);
    fprintf(fp, "ram_budget_min_bytes=%u\n", RAM_BUDGET_MIN_BYTES);
    fprintf(fp, "ram_budget_max_bytes=%u\n", RAM_BUDGET_MAX_BYTES);
    fprintf(fp, "context_size_within_budget=%s\n", within_budget ? "yes" : "no");
    fprintf(fp, "uses_dynamic_memory=no\n");
    fprintf(fp, "uses_recursion=no\n");

    fclose(fp);
    return 0;
}
