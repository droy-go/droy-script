/**
 * DLp - Packages Module
 * Manages Droy packages
 */

#include "dlp.h"

int dlp_add(int argc, char *argv[], dlp_options_t *opts) {
    (void)opts;
    
    if (argc == 0) {
        dlp_error("No packages specified\n");
        return DLP_ERROR;
    }
    
    dlp_print(COLOR_BLUE, "\n➕ Adding packages...\n\n");
    
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-') continue;
        
        dlp_info_msg("Adding %s...\n", argv[i]);
        
        /* Create package directory */
        char pkg_dir[MAX_PATH];
        snprintf(pkg_dir, MAX_PATH, "%s/%s/%s", 
                 dlp_get_dlp_dir(), PACKAGES_DIR, argv[i]);
        
        dlp_mkdir_p(pkg_dir);
        
        /* Clone package repository */
        char cmd[MAX_CMD];
        char repo_url[MAX_PATH];
        snprintf(repo_url, MAX_PATH, "https://github.com/droy-go/droy-%s.git", argv[i]);
        
        char git_dir[MAX_PATH];
        snprintf(git_dir, MAX_PATH, "%s/.git", pkg_dir);
        
        if (dlp_dir_exists(git_dir)) {
            snprintf(cmd, MAX_CMD, "cd %s && git pull > /dev/null 2>&1", pkg_dir);
        } else {
            snprintf(cmd, MAX_CMD, "rm -rf %s && git clone --depth 1 %s %s > /dev/null 2>&1",
                     pkg_dir, repo_url, pkg_dir);
        }
        
        if (dlp_exec(cmd, 0) != 0) {
            dlp_warning("Failed to add %s (may not exist)\n", argv[i]);
        } else {
            dlp_success("%s added\n", argv[i]);
        }
    }
    
    dlp_print(COLOR_GREEN, "\n✅ Done!\n\n");
    return DLP_OK;
}

int dlp_remove(int argc, char *argv[], dlp_options_t *opts) {
    (void)opts;
    
    if (argc == 0) {
        dlp_error("No packages specified\n");
        return DLP_ERROR;
    }
    
    dlp_print(COLOR_BLUE, "\n➖ Removing packages...\n\n");
    
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-') continue;
        
        char pkg_dir[MAX_PATH];
        snprintf(pkg_dir, MAX_PATH, "%s/%s/%s",
                 dlp_get_dlp_dir(), PACKAGES_DIR, argv[i]);
        
        if (dlp_dir_exists(pkg_dir)) {
            char cmd[MAX_CMD];
            snprintf(cmd, MAX_CMD, "rm -rf %s", pkg_dir);
            dlp_exec(cmd, 0);
            dlp_success("%s removed\n", argv[i]);
        } else {
            dlp_warning("%s is not installed\n", argv[i]);
        }
    }
    
    dlp_print(COLOR_GREEN, "\n✅ Done!\n\n");
    return DLP_OK;
}

int dlp_update(int argc, char *argv[], dlp_options_t *opts) {
    (void)opts;
    
    dlp_print(COLOR_BLUE, "\n🔄 Updating packages...\n\n");
    
    /* Update Droy first */
    dlp_spinner_start("Updating Droy");
    char *droy_dir = dlp_get_droy_dir();
    char git_dir[MAX_PATH];
    snprintf(git_dir, MAX_PATH, "%s/.git", droy_dir);
    
    if (dlp_dir_exists(git_dir)) {
        char cmd[MAX_CMD];
        snprintf(cmd, MAX_CMD, "cd %s && git pull > /dev/null 2>&1", droy_dir);
        if (dlp_exec(cmd, 0) == 0) {
            /* Rebuild */
            snprintf(cmd, MAX_CMD, "cd %s && make clean > /dev/null 2>&1 && make > /dev/null 2>&1", droy_dir);
            dlp_exec(cmd, 0);
            dlp_spinner_stop("ok", "Droy updated");
        } else {
            dlp_spinner_stop("error", "Droy update failed");
        }
    } else {
        dlp_spinner_stop("error", "Droy not installed");
    }
    
    /* Update packages */
    char pkg_dir[MAX_PATH];
    snprintf(pkg_dir, MAX_PATH, "%s/%s", dlp_get_dlp_dir(), PACKAGES_DIR);
    
    if (!dlp_dir_exists(pkg_dir)) {
        dlp_info_msg("No packages installed\n");
        return DLP_OK;
    }
    
    DIR *dir = opendir(pkg_dir);
    if (!dir) {
        dlp_error("Failed to open packages directory\n");
        return DLP_ERROR;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", pkg_dir, entry->d_name);
        
        char git_path[MAX_PATH];
        snprintf(git_path, MAX_PATH, "%s/.git", full_path);
        
        if (dlp_dir_exists(git_path)) {
            dlp_info_msg("Updating %s...\n", entry->d_name);
            char cmd[MAX_CMD];
            snprintf(cmd, MAX_CMD, "cd %s && git pull > /dev/null 2>&1", full_path);
            dlp_exec(cmd, 0);
        }
    }
    
    closedir(dir);
    
    dlp_print(COLOR_GREEN, "\n✅ Update complete!\n\n");
    return DLP_OK;
}

int dlp_list(int argc, char *argv[], dlp_options_t *opts) {
    (void)opts;
    (void)argc;
    (void)argv;
    
    dlp_print(COLOR_BLUE, "\n📋 Installed Packages\n\n");
    
    /* Show Droy version */
    char *droy_bin = dlp_get_droy_bin();
    if (dlp_file_exists(droy_bin)) {
        dlp_print(COLOR_CYAN, "Droy Language:\n");
        char cmd[MAX_CMD];
        snprintf(cmd, MAX_CMD, "%s -v 2>&1", droy_bin);
        char version[MAX_LINE] = {0};
        FILE *fp = popen(cmd, "r");
        if (fp && fgets(version, MAX_LINE, fp)) {
            size_t n = strlen(version);
            if (n > 0 && version[n-1] == '\n') version[n-1] = '\0';
            dlp_print(COLOR_WHITE, "  ✓ droy@%s\n", version);
        } else {
            dlp_print(COLOR_WHITE, "  ✓ droy\n");
        }
        if (fp) pclose(fp);
        printf("\n");
    }
    
    /* List packages */
    char pkg_dir[MAX_PATH];
    snprintf(pkg_dir, MAX_PATH, "%s/%s", dlp_get_dlp_dir(), PACKAGES_DIR);
    
    if (!dlp_dir_exists(pkg_dir)) {
        dlp_print(COLOR_GRAY, "No packages installed.\n\n");
        return DLP_OK;
    }
    
    DIR *dir = opendir(pkg_dir);
    if (!dir) {
        dlp_print(COLOR_GRAY, "No packages installed.\n\n");
        return DLP_OK;
    }
    
    dlp_print(COLOR_CYAN, "Packages:\n");
    
    int count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", pkg_dir, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            dlp_print(COLOR_WHITE, "  ✓ %s\n", entry->d_name);
            count++;
        }
    }
    
    closedir(dir);
    
    dlp_print(COLOR_GRAY, "\nTotal: %d packages\n\n", count);
    
    return DLP_OK;
}

int dlp_search(int argc, char *argv[], dlp_options_t *opts) {
    (void)opts;
    
    if (argc == 0) {
        dlp_error("No search query specified\n");
        return DLP_ERROR;
    }
    
    const char *query = argv[0];
    
    dlp_print(COLOR_BLUE, "\n🔍 Searching for: %s\n\n", query);
    
    /* Mock search results - in production this would query a registry */
    const char *packages[][2] = {
        {"core", "Droy core library"},
        {"math", "Math utilities"},
        {"io", "Input/Output operations"},
        {"http", "HTTP client"},
        {"json", "JSON parser"},
        {"crypto", "Cryptography utilities"},
        {"test", "Testing framework"},
        {"cli", "CLI utilities"},
        {"fs", "File system operations"},
        {"string", "String utilities"},
        {NULL, NULL}
    };
    
    int found = 0;
    for (int i = 0; packages[i][0]; i++) {
        if (strstr(packages[i][0], query) || strstr(packages[i][1], query)) {
            dlp_print(COLOR_WHITE, "%s\n", packages[i][0]);
            dlp_print(COLOR_GRAY, "  %s\n", packages[i][1]);
            dlp_print(COLOR_CYAN, "  DLp add %s\n\n", packages[i][0]);
            found++;
        }
    }
    
    if (found == 0) {
        dlp_print(COLOR_YELLOW, "No packages found.\n\n");
    } else {
        dlp_print(COLOR_GRAY, "Found %d packages\n\n", found);
    }
    
    return DLP_OK;
}
