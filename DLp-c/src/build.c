/**
 * DLp - Build Module
 * Builds Droy projects
 */

#include "dlp.h"

int dlp_build_llvm(const char *file, const char *output, dlp_options_t *opts) {
    char cmd[MAX_CMD];
    char *droy_bin = dlp_get_droy_bin();
    
    if (!dlp_file_exists(droy_bin)) {
        dlp_error("Droy not installed. Run: DLp install\n");
        return DLP_ERROR;
    }
    
    if (!dlp_file_exists(file)) {
        dlp_error("File not found: %s\n", file);
        return DLP_ERROR;
    }
    
    dlp_mkdir_p(output);
    
    char out_file[MAX_PATH];
    const char *basename = strrchr(file, '/');
    if (!basename) basename = file;
    else basename++;
    
    char name[MAX_NAME];
    strncpy(name, basename, MAX_NAME - 1);
    char *dot = strrchr(name, '.');
    if (dot) *dot = '\0';
    
    snprintf(out_file, MAX_PATH, "%s/%s.ll", output, name);
    
    dlp_spinner_start("Compiling to LLVM IR");
    snprintf(cmd, MAX_CMD, "%s -c \"%s\" -o \"%s\" %s 2>&1", 
             droy_bin, file, out_file, opts->verbose ? "" : "> /dev/null");
    
    if (dlp_exec(cmd, opts->verbose) != 0) {
        dlp_spinner_stop("error", "Compilation failed");
        return DLP_ERROR;
    }
    
    dlp_spinner_stop("ok", "LLVM IR created");
    dlp_success("Output: %s\n", out_file);
    
    return DLP_OK;
}

int dlp_build_binary(const char *file, const char *output, dlp_options_t *opts) {
    char cmd[MAX_CMD];
    char *droy_bin = dlp_get_droy_bin();
    
    if (!dlp_file_exists(droy_bin)) {
        dlp_error("Droy not installed. Run: DLp install\n");
        return DLP_ERROR;
    }
    
    if (!dlp_file_exists(file)) {
        dlp_error("File not found: %s\n", file);
        return DLP_ERROR;
    }
    
    dlp_mkdir_p(output);
    
    char name[MAX_NAME];
    const char *basename = strrchr(file, '/');
    if (!basename) basename = file;
    else basename++;
    
    strncpy(name, basename, MAX_NAME - 1);
    char *dot = strrchr(name, '.');
    if (dot) *dot = '\0';
    
    char ll_file[MAX_PATH];
    char obj_file[MAX_PATH];
    char bin_file[MAX_PATH];
    
    snprintf(ll_file, MAX_PATH, "%s/%s.ll", output, name);
    snprintf(obj_file, MAX_PATH, "%s/%s.o", output, name);
    snprintf(bin_file, MAX_PATH, "%s/%s", output, name);
    
    /* Step 1: Compile to LLVM IR */
    dlp_spinner_start("Compiling to LLVM IR");
    snprintf(cmd, MAX_CMD, "%s -c \"%s\" -o \"%s\" 2>&1", droy_bin, file, ll_file);
    
    if (dlp_exec(cmd, 0) != 0) {
        dlp_spinner_stop("error", "Compilation failed");
        return DLP_ERROR;
    }
    dlp_spinner_stop("ok", "LLVM IR generated");
    
    /* Step 2: Generate object file */
    dlp_spinner_start("Generating object file");
    snprintf(cmd, MAX_CMD, "llc -filetype=obj -O=%d \"%s\" -o \"%s\" 2>&1", 
             opts->optimize ? 2 : 0, ll_file, obj_file);
    
    if (dlp_exec(cmd, 0) != 0) {
        dlp_spinner_stop("error", "Object generation failed");
        remove(ll_file);
        return DLP_ERROR;
    }
    dlp_spinner_stop("ok", "Object file generated");
    
    /* Step 3: Link binary */
    dlp_spinner_start("Linking binary");
    snprintf(cmd, MAX_CMD, "gcc \"%s\" -o \"%s\" -lm 2>&1", obj_file, bin_file);
    
    if (dlp_exec(cmd, 0) != 0) {
        dlp_spinner_stop("error", "Linking failed");
        remove(ll_file);
        remove(obj_file);
        return DLP_ERROR;
    }
    
    chmod(bin_file, 0755);
    dlp_spinner_stop("ok", "Binary created");
    
    /* Cleanup */
    remove(ll_file);
    remove(obj_file);
    
    dlp_success("Binary: %s\n", bin_file);
    
    return DLP_OK;
}

int dlp_build_asm(const char *file, const char *output, dlp_options_t *opts) {
    char cmd[MAX_CMD];
    char *droy_bin = dlp_get_droy_bin();
    
    if (!dlp_file_exists(droy_bin)) {
        dlp_error("Droy not installed. Run: DLp install\n");
        return DLP_ERROR;
    }
    
    if (!dlp_file_exists(file)) {
        dlp_error("File not found: %s\n", file);
        return DLP_ERROR;
    }
    
    dlp_mkdir_p(output);
    
    char name[MAX_NAME];
    const char *basename = strrchr(file, '/');
    if (!basename) basename = file;
    else basename++;
    
    strncpy(name, basename, MAX_NAME - 1);
    char *dot = strrchr(name, '.');
    if (dot) *dot = '\0';
    
    char ll_file[MAX_PATH];
    char asm_file[MAX_PATH];
    
    snprintf(ll_file, MAX_PATH, "%s/%s.ll", output, name);
    snprintf(asm_file, MAX_PATH, "%s/%s.s", output, name);
    
    /* Step 1: Compile to LLVM IR */
    dlp_spinner_start("Compiling to LLVM IR");
    snprintf(cmd, MAX_CMD, "%s -c \"%s\" -o \"%s\" 2>&1", droy_bin, file, ll_file);
    
    if (dlp_exec(cmd, 0) != 0) {
        dlp_spinner_stop("error", "Compilation failed");
        return DLP_ERROR;
    }
    dlp_spinner_stop("ok", "LLVM IR generated");
    
    /* Step 2: Generate assembly */
    dlp_spinner_start("Generating assembly");
    snprintf(cmd, MAX_CMD, "llc \"%s\" -o \"%s\" 2>&1", ll_file, asm_file);
    
    if (dlp_exec(cmd, 0) != 0) {
        dlp_spinner_stop("error", "Assembly generation failed");
        remove(ll_file);
        return DLP_ERROR;
    }
    
    remove(ll_file);
    dlp_spinner_stop("ok", "Assembly created");
    dlp_success("Output: %s\n", asm_file);
    
    return DLP_OK;
}

int dlp_build(int argc, char *argv[], dlp_options_t *opts) {
    const char *file = "src/main.droy";
    
    if (argc > 0 && argv[0][0] != '-') {
        file = argv[0];
    }
    
    dlp_print(COLOR_BLUE, "\n🔨 Building...\n\n");
    dlp_info_msg("File: %s\n", file);
    dlp_info_msg("Target: %s\n", opts->target);
    dlp_info_msg("Output: %s\n\n", opts->output);
    
    if (strcmp(opts->target, "llvm") == 0) {
        return dlp_build_llvm(file, opts->output, opts);
    } else if (strcmp(opts->target, "bin") == 0 || strcmp(opts->target, "binary") == 0) {
        return dlp_build_binary(file, opts->output, opts);
    } else if (strcmp(opts->target, "asm") == 0 || strcmp(opts->target, "assembly") == 0) {
        return dlp_build_asm(file, opts->output, opts);
    } else {
        dlp_error("Unknown target: %s\n", opts->target);
        return DLP_ERROR;
    }
}
