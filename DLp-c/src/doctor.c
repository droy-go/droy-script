/**
 * DLp - Doctor Module
 * Checks system health
 */

#include "dlp.h"

int dlp_check_git(void) {
    char output[MAX_LINE] = {0};
    if (dlp_exec_capture("git --version 2>&1", output, MAX_LINE) == 0) {
        dlp_print(COLOR_GREEN, "  ✓ %s\n", output);
        return DLP_OK;
    }
    dlp_print(COLOR_RED, "  ✗ Git not found\n");
    return DLP_ERROR;
}

int dlp_check_gcc(void) {
    char output[MAX_LINE] = {0};
    if (dlp_exec_capture("gcc --version 2>&1 | head -1", output, MAX_LINE) == 0) {
        dlp_print(COLOR_GREEN, "  ✓ %s\n", output);
        return DLP_OK;
    }
    dlp_print(COLOR_RED, "  ✗ GCC not found\n");
    return DLP_ERROR;
}

int dlp_check_make(void) {
    char output[MAX_LINE] = {0};
    if (dlp_exec_capture("make --version 2>&1 | head -1", output, MAX_LINE) == 0) {
        dlp_print(COLOR_GREEN, "  ✓ %s\n", output);
        return DLP_OK;
    }
    dlp_print(COLOR_RED, "  ✗ Make not found\n");
    return DLP_ERROR;
}

int dlp_check_llvm(void) {
    char output[MAX_LINE] = {0};
    if (dlp_exec_capture("llvm-config --version 2>&1", output, MAX_LINE) == 0) {
        dlp_print(COLOR_GREEN, "  ✓ LLVM %s\n", output);
        return DLP_OK;
    }
    dlp_print(COLOR_YELLOW, "  ⚠ LLVM not found (optional)\n");
    return DLP_OK; /* Optional */
}

int dlp_check_droy(void) {
    char *droy_bin = dlp_get_droy_bin();
    if (dlp_file_exists(droy_bin)) {
        char cmd[MAX_CMD];
        snprintf(cmd, MAX_CMD, "%s -v 2>&1", droy_bin);
        char version[MAX_LINE] = {0};
        if (dlp_exec_capture(cmd, version, MAX_LINE) == 0) {
            dlp_print(COLOR_GREEN, "  ✓ Droy %s\n", version);
            return DLP_OK;
        }
        dlp_print(COLOR_YELLOW, "  ⚠ Droy installed but not working\n");
        return DLP_ERROR;
    }
    dlp_print(COLOR_RED, "  ✗ Droy not installed\n");
    dlp_print(COLOR_GRAY, "    Run: DLp install\n");
    return DLP_ERROR;
}

int dlp_doctor(int argc, char *argv[], dlp_options_t *opts) {
    (void)argc;
    (void)argv;
    (void)opts;
    
    dlp_print(COLOR_BLUE, "\n🔍 DLp Doctor\n\n");
    
    int issues = 0;
    
    /* Node.js version (not applicable for C version) */
    dlp_print(COLOR_CYAN, "Node.js:\n");
    dlp_print(COLOR_WHITE, "  N/A (C version)\n\n");
    
    /* Check Git */
    dlp_print(COLOR_CYAN, "Git:\n");
    if (dlp_check_git() != DLP_OK) issues++;
    printf("\n");
    
    /* Check GCC */
    dlp_print(COLOR_CYAN, "GCC:\n");
    if (dlp_check_gcc() != DLP_OK) issues++;
    printf("\n");
    
    /* Check Make */
    dlp_print(COLOR_CYAN, "Make:\n");
    if (dlp_check_make() != DLP_OK) issues++;
    printf("\n");
    
    /* Check LLVM */
    dlp_print(COLOR_CYAN, "LLVM (optional):\n");
    dlp_check_llvm();
    printf("\n");
    
    /* Check Droy */
    dlp_print(COLOR_CYAN, "Droy:\n");
    if (dlp_check_droy() != DLP_OK) issues++;
    printf("\n");
    
    /* Summary */
    if (issues == 0) {
        dlp_print(COLOR_GREEN, "✅ All checks passed!\n\n");
    } else {
        dlp_print(COLOR_YELLOW, "⚠️  %d issue(s) found\n\n", issues);
    }
    
    return issues > 0 ? DLP_ERROR : DLP_OK;
}
