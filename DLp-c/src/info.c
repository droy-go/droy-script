/**
 * DLp - Info Module
 * Shows project information
 */

#include "dlp.h"
#include <sys/utsname.h>

int dlp_info(int argc, char *argv[], dlp_options_t *opts) {
    (void)argc;
    (void)argv;
    (void)opts;
    
    dlp_print(COLOR_BLUE, "\n📋 DLp Information\n\n");
    
    /* DLp info */
    dlp_print(COLOR_CYAN, "DLp:\n");
    dlp_print(COLOR_WHITE, "  Home: %s\n", dlp_get_dlp_dir());
    dlp_print(COLOR_WHITE, "  Version: %s\n", DLP_VERSION);
    printf("\n");
    
    /* Droy info */
    dlp_print(COLOR_CYAN, "Droy Language:\n");
    char *droy_bin = dlp_get_droy_bin();
    if (dlp_file_exists(droy_bin)) {
        char cmd[MAX_CMD];
        snprintf(cmd, MAX_CMD, "%s -v 2>&1", droy_bin);
        char version[MAX_LINE] = {0};
        FILE *fp = popen(cmd, "r");
        if (fp && fgets(version, MAX_LINE, fp)) {
            size_t n = strlen(version);
            if (n > 0 && version[n-1] == '\n') version[n-1] = '\0';
            dlp_print(COLOR_WHITE, "  Version: %s\n", version);
        }
        if (fp) pclose(fp);
        dlp_print(COLOR_WHITE, "  Path: %s\n", dlp_get_droy_dir());
    } else {
        dlp_print(COLOR_YELLOW, "  Not installed\n");
    }
    printf("\n");
    
    /* Packages */
    dlp_print(COLOR_CYAN, "Packages:\n");
    char pkg_dir[MAX_PATH];
    snprintf(pkg_dir, MAX_PATH, "%s/%s", dlp_get_dlp_dir(), PACKAGES_DIR);
    
    if (dlp_dir_exists(pkg_dir)) {
        DIR *dir = opendir(pkg_dir);
        if (dir) {
            int count = 0;
            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_name[0] == '.') continue;
                char full_path[MAX_PATH];
                snprintf(full_path, MAX_PATH, "%s/%s", pkg_dir, entry->d_name);
                if (dlp_dir_exists(full_path)) {
                    dlp_print(COLOR_GRAY, "    - %s\n", entry->d_name);
                    count++;
                }
            }
            closedir(dir);
            dlp_print(COLOR_WHITE, "  Installed: %d\n", count);
        }
    } else {
        dlp_print(COLOR_GRAY, "  None\n");
    }
    printf("\n");
    
    /* System */
    dlp_print(COLOR_CYAN, "System:\n");
    struct utsname sys;
    if (uname(&sys) == 0) {
        dlp_print(COLOR_WHITE, "  Platform: %s\n", sys.sysname);
        dlp_print(COLOR_WHITE, "  Arch: %s\n", sys.machine);
    }
    printf("\n");
    
    return DLP_OK;
}
