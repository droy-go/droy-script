/**
 * DLp - Test Module
 * Runs Droy tests
 */

#include "dlp.h"

int dlp_test(int argc, char *argv[], dlp_options_t *opts) {
    (void)argc;
    (void)argv;
    
    char *droy_bin = dlp_get_droy_bin();
    
    if (!dlp_file_exists(droy_bin)) {
        dlp_error("Droy not installed. Run: DLp install\n");
        return DLP_ERROR;
    }
    
    dlp_print(COLOR_BLUE, "\n🧪 Running Tests\n\n");
    
    /* Find test directory */
    const char *test_dir = "tests";
    if (!dlp_dir_exists(test_dir)) {
        dlp_print(COLOR_YELLOW, "No tests directory found.\n\n");
        return DLP_OK;
    }
    
    /* Count test files */
    DIR *dir = opendir(test_dir);
    if (!dir) {
        dlp_error("Failed to open tests directory\n");
        return DLP_ERROR;
    }
    
    int total = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".droy")) {
            total++;
        }
    }
    rewinddir(dir);
    
    if (total == 0) {
        dlp_print(COLOR_YELLOW, "No test files found.\n\n");
        closedir(dir);
        return DLP_OK;
    }
    
    dlp_info_msg("Found %d test(s)\n\n", total);
    
    int passed = 0;
    int failed = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        if (!strstr(entry->d_name, ".droy")) continue;
        
        char test_file[MAX_PATH];
        snprintf(test_file, MAX_PATH, "%s/%s", test_dir, entry->d_name);
        
        dlp_spinner_start(entry->d_name);
        
        char cmd[MAX_CMD];
        snprintf(cmd, MAX_CMD, "%s \"%s\" > /dev/null 2>&1", droy_bin, test_file);
        
        if (dlp_exec(cmd, opts->verbose) == 0) {
            dlp_spinner_stop("ok", entry->d_name);
            passed++;
        } else {
            dlp_spinner_stop("error", entry->d_name);
            failed++;
        }
    }
    
    closedir(dir);
    
    dlp_print(COLOR_BLUE, "\nResults: %d passed, %d failed\n\n", passed, failed);
    
    return failed > 0 ? DLP_ERROR : DLP_OK;
}
