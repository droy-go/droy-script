/**
 * DLp - Clean Module
 * Cleans build artifacts
 */

#include "dlp.h"
#include <fnmatch.h>

int dlp_clean(int argc, char *argv[], dlp_options_t *opts) {
    (void)argc;
    (void)argv;
    
    dlp_print(COLOR_BLUE, "\n🧹 Cleaning...\n\n");
    
    int cleaned = 0;
    
    /* Clean patterns */
    const char *patterns[] = {"*.o", "*.ll", "*.s", "*.exe", "output*", NULL};
    
    DIR *dir = opendir(".");
    if (!dir) {
        dlp_error("Failed to open current directory\n");
        return DLP_ERROR;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        for (int i = 0; patterns[i]; i++) {
            if (fnmatch(patterns[i], entry->d_name, 0) == 0) {
                remove(entry->d_name);
                cleaned++;
                break;
            }
        }
    }
    
    closedir(dir);
    
    /* Clean dist directory */
    if (opts->all || opts->dist) {
        dlp_spinner_start("Cleaning dist");
        if (dlp_dir_exists("dist")) {
            dlp_exec("rm -rf dist", 0);
            cleaned++;
        }
        dlp_spinner_stop("ok", "Dist cleaned");
    }
    
    /* Clean build directory */
    if (opts->all) {
        dlp_spinner_start("Cleaning build");
        if (dlp_dir_exists("build")) {
            dlp_exec("rm -rf build", 0);
            cleaned++;
        }
        dlp_spinner_stop("ok", "Build cleaned");
        
        dlp_spinner_start("Cleaning cache");
        if (dlp_dir_exists(".dlp-cache")) {
            dlp_exec("rm -rf .dlp-cache", 0);
            cleaned++;
        }
        dlp_spinner_stop("ok", "Cache cleaned");
    }
    
    dlp_print(COLOR_GREEN, "\n✅ Cleaned %d items\n\n", cleaned);
    
    return DLP_OK;
}
