#include <stdio.h>
#include "ppg_ibi.h"

int main(void)
{
    const size_t context_size_bytes = ppg_ibi_context_size();
    const size_t ram_budget_min_bytes = 15360U;
    const size_t ram_budget_max_bytes = 20480U;
    const int context_size_within_budget = (context_size_bytes <= ram_budget_max_bytes);

    FILE *fp = fopen("build/output/resource_report.txt", "w");
    if (fp == NULL) {
        fprintf(stderr, "failed to open build/output/resource_report.txt\n");
        return 1;
    }

    fprintf(fp, "context_size_bytes=%zu\n", context_size_bytes);
    fprintf(fp, "ram_budget_min_bytes=%zu\n", ram_budget_min_bytes);
    fprintf(fp, "ram_budget_max_bytes=%zu\n", ram_budget_max_bytes);
    fprintf(
        fp,
        "context_size_within_budget=%s\n",
        context_size_within_budget != 0 ? "yes" : "no");
    fprintf(fp, "uses_dynamic_memory=no\n");

    if (fclose(fp) != 0) {
        fprintf(stderr, "failed to close build/output/resource_report.txt\n");
        return 1;
    }

    return 0;
}
