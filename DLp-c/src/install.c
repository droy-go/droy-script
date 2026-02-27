/**
 * DLp - Install Module
 * Handles Droy installation and package management
 */

#include "dlp.h"
#include <stdarg.h>

static char spinner_msg[MAX_LINE] = {0};

void dlp_print(const char *color, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("%s", color);
    vprintf(fmt, args);
    printf(COLOR_RESET);
    va_end(args);
}

void dlp_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s❌ ", COLOR_RED);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, COLOR_RESET);
    va_end(args);
}

void dlp_success(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("%s✅ ", COLOR_GREEN);
    vprintf(fmt, args);
    printf(COLOR_RESET);
    va_end(args);
}

void dlp_warning(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("%s⚠️  ", COLOR_YELLOW);
    vprintf(fmt, args);
    printf(COLOR_RESET);
    va_end(args);
}

void dlp_info_msg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("%sℹ️  ", COLOR_BLUE);
    vprintf(fmt, args);
    printf(COLOR_RESET);
    va_end(args);
}

void dlp_spinner_start(const char *msg) {
    strncpy(spinner_msg, msg, MAX_LINE - 1);
    printf(COLOR_CYAN "⏳ %s..." COLOR_RESET, msg);
    fflush(stdout);
}

void dlp_spinner_stop(const char *status, const char *msg) {
    printf("\r");
    if (strcmp(status, "ok") == 0) {
        dlp_success("%s\n", msg ? msg : spinner_msg);
    } else if (strcmp(status, "error") == 0) {
        dlp_error("%s\n", msg ? msg : spinner_msg);
    } else {
        dlp_warning("%s\n", msg ? msg : spinner_msg);
    }
}

int dlp_exec(const char *cmd, int verbose) {
    if (verbose) {
        dlp_info_msg("Running: %s\n", cmd);
    }
    int status = system(cmd);
    return WIFEXITED(status) ? WEXITSTATUS(status) : DLP_ERROR;
}

int dlp_exec_capture(const char *cmd, char *output, size_t len) {
    FILE *fp = popen(cmd, "r");
    if (!fp) return DLP_ERROR;
    
    if (fgets(output, len, fp) != NULL) {
        /* Remove trailing newline */
        size_t n = strlen(output);
        if (n > 0 && output[n-1] == '\n') output[n-1] = '\0';
    }
    
    int status = pclose(fp);
    return WIFEXITED(status) ? WEXITSTATUS(status) : DLP_ERROR;
}

int dlp_dir_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

int dlp_file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

int dlp_mkdir_p(const char *path) {
    char tmp[MAX_PATH];
    char *p = NULL;
    size_t len;
    
    strncpy(tmp, path, MAX_PATH - 1);
    tmp[MAX_PATH - 1] = '\0';
    len = strlen(tmp);
    
    if (tmp[len - 1] == '/') tmp[len - 1] = '\0';
    
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
    
    return dlp_dir_exists(path) ? DLP_OK : DLP_ERROR;
}

char* dlp_get_home(void) {
    static char home[MAX_PATH];
    const char *h = getenv("HOME");
    if (h) {
        strncpy(home, h, MAX_PATH - 1);
        home[MAX_PATH - 1] = '\0';
    } else {
        strncpy(home, ".", MAX_PATH - 1);
    }
    return home;
}

char* dlp_get_dlp_dir(void) {
    static char dir[MAX_PATH];
    snprintf(dir, MAX_PATH, "%s/%s", dlp_get_home(), DLP_DIR);
    return dir;
}

char* dlp_get_droy_dir(void) {
    static char dir[MAX_PATH];
    snprintf(dir, MAX_PATH, "%s/%s/%s", dlp_get_home(), DLP_DIR, DROY_DIR);
    return dir;
}

char* dlp_get_bin_dir(void) {
    static char dir[MAX_PATH];
    snprintf(dir, MAX_PATH, "%s/%s/%s/%s", dlp_get_home(), DLP_DIR, DROY_DIR, BIN_DIR);
    return dir;
}

char* dlp_get_droy_bin(void) {
    static char bin[MAX_PATH];
    snprintf(bin, MAX_PATH, "%s/droy", dlp_get_bin_dir());
    return bin;
}

int dlp_check_dependencies(void) {
    dlp_spinner_start("Checking dependencies");
    
    const char *deps[] = {"git", "make", "gcc", NULL};
    char cmd[MAX_CMD];
    
    for (int i = 0; deps[i]; i++) {
        snprintf(cmd, MAX_CMD, "which %s > /dev/null 2>&1", deps[i]);
        if (system(cmd) != 0) {
            dlp_spinner_stop("error", "Missing dependencies");
            dlp_error("Missing: %s\n", deps[i]);
            dlp_info_msg("Install: sudo apt-get install git build-essential\n");
            return DLP_ERROR;
        }
    }
    
    dlp_spinner_stop("ok", "Dependencies OK");
    return DLP_OK;
}

int dlp_clone_droy(const char *dest) {
    char cmd[MAX_CMD];
    
    /* Check if already exists */
    char git_dir[MAX_PATH];
    snprintf(git_dir, MAX_PATH, "%s/.git", dest);
    
    if (dlp_dir_exists(git_dir)) {
        dlp_spinner_start("Updating Droy source");
        snprintf(cmd, MAX_CMD, "cd %s && git pull --depth 1 > /dev/null 2>&1", dest);
    } else {
        dlp_spinner_start("Cloning Droy repository");
        snprintf(cmd, MAX_CMD, "rm -rf %s && git clone --depth 1 %s %s > /dev/null 2>&1", 
                 dest, DROY_REPO, dest);
    }
    
    if (dlp_exec(cmd, 0) != 0) {
        dlp_spinner_stop("error", "Failed to download source");
        return DLP_ERROR;
    }
    
    dlp_spinner_stop("ok", "Source downloaded");
    return DLP_OK;
}

int dlp_build_droy(const char *srcdir) {
    char cmd[MAX_CMD];
    
    dlp_spinner_start("Building Droy compiler");
    
    snprintf(cmd, MAX_CMD, "cd %s && make clean > /dev/null 2>&1 && make > /dev/null 2>&1", srcdir);
    
    if (dlp_exec(cmd, 0) != 0) {
        dlp_spinner_stop("error", "Build failed");
        return DLP_ERROR;
    }
    
    dlp_spinner_stop("ok", "Build complete");
    return DLP_OK;
}

int dlp_setup_environment(void) {
    dlp_spinner_start("Setting up environment");
    
    char rc_file[MAX_PATH];
    const char *shell = getenv("SHELL");
    
    if (shell && strstr(shell, "zsh")) {
        snprintf(rc_file, MAX_PATH, "%s/.zshrc", dlp_get_home());
    } else {
        snprintf(rc_file, MAX_PATH, "%s/.bashrc", dlp_get_home());
    }
    
    /* Check if already configured */
    FILE *fp = fopen(rc_file, "r");
    if (fp) {
        char line[MAX_LINE];
        while (fgets(line, MAX_LINE, fp)) {
            if (strstr(line, "DLp")) {
                fclose(fp);
                dlp_spinner_stop("ok", "Environment already configured");
                return DLP_OK;
            }
        }
        fclose(fp);
    }
    
    /* Append to rc file */
    fp = fopen(rc_file, "a");
    if (fp) {
        fprintf(fp, "\n# DLp - Droy Language Package Manager\n");
        fprintf(fp, "export DROY_HOME=\"%s\"\n", dlp_get_droy_dir());
        fprintf(fp, "export PATH=\"%s:$PATH\"\n", dlp_get_bin_dir());
        fprintf(fp, "export DLP_HOME=\"%s\"\n", dlp_get_dlp_dir());
        fclose(fp);
    }
    
    dlp_spinner_stop("ok", "Environment configured");
    return DLP_OK;
}

int dlp_install_droy(dlp_options_t *opts) {
    dlp_print(COLOR_BLUE, "\n📦 Installing Droy Language...\n\n");
    
    /* Check if already installed */
    char *droy_bin = dlp_get_droy_bin();
    if (dlp_file_exists(droy_bin) && !opts->force) {
        dlp_warning("Droy is already installed.\n");
        dlp_info_msg("Use --force to reinstall.\n");
        return DLP_OK;
    }
    
    /* Check dependencies */
    if (dlp_check_dependencies() != DLP_OK) {
        return DLP_ERROR;
    }
    
    /* Create directories */
    dlp_mkdir_p(dlp_get_dlp_dir());
    dlp_mkdir_p(dlp_get_droy_dir());
    
    /* Clone repository */
    if (dlp_clone_droy(dlp_get_droy_dir()) != DLP_OK) {
        return DLP_ERROR;
    }
    
    /* Build Droy */
    if (dlp_build_droy(dlp_get_droy_dir()) != DLP_OK) {
        return DLP_ERROR;
    }
    
    /* Setup environment */
    dlp_setup_environment();
    
    /* Verify installation */
    dlp_spinner_start("Verifying installation");
    
    char version[MAX_LINE] = {0};
    char cmd[MAX_CMD];
    snprintf(cmd, MAX_CMD, "%s -v 2>&1", droy_bin);
    
    if (dlp_file_exists(droy_bin)) {
        FILE *fp = popen(cmd, "r");
        if (fp && fgets(version, MAX_LINE, fp)) {
            /* Remove newline */
            size_t n = strlen(version);
            if (n > 0 && version[n-1] == '\n') version[n-1] = '\0';
            pclose(fp);
            dlp_spinner_stop("ok", "Installation verified");
            dlp_success("Droy %s installed successfully!\n", version);
        } else {
            dlp_spinner_stop("ok", "Installation complete");
        }
    } else {
        dlp_spinner_stop("error", "Verification failed");
        return DLP_ERROR;
    }
    
    dlp_print(COLOR_GREEN, "\n✅ Installation complete!\n\n");
    dlp_print(COLOR_WHITE, "Usage:\n");
    dlp_print(COLOR_CYAN, "  DLp run <file.droy>    Run a Droy program\n");
    dlp_print(COLOR_CYAN, "  DLp repl               Start REPL mode\n");
    dlp_print(COLOR_CYAN, "  DLp init               Create new project\n\n");
    dlp_print(COLOR_YELLOW, "Note: Restart your terminal or run:\n");
    dlp_print(COLOR_CYAN, "  source %s/.bashrc\n\n", dlp_get_home());
    
    return DLP_OK;
}

int dlp_install_package(const char *package, dlp_options_t *opts) {
    (void)opts;
    dlp_info_msg("Installing package: %s\n", package);
    
    char pkg_dir[MAX_PATH];
    snprintf(pkg_dir, MAX_PATH, "%s/%s/%s", dlp_get_dlp_dir(), PACKAGES_DIR, package);
    
    dlp_mkdir_p(pkg_dir);
    
    char repo_url[MAX_PATH];
    snprintf(repo_url, MAX_PATH, "https://github.com/droy-go/droy-%s.git", package);
    
    char cmd[MAX_CMD];
    char git_dir[MAX_PATH];
    snprintf(git_dir, MAX_PATH, "%s/.git", pkg_dir);
    
    if (dlp_dir_exists(git_dir)) {
        snprintf(cmd, MAX_CMD, "cd %s && git pull > /dev/null 2>&1", pkg_dir);
    } else {
        snprintf(cmd, MAX_CMD, "rm -rf %s && git clone --depth 1 %s %s > /dev/null 2>&1", 
                 pkg_dir, repo_url, pkg_dir);
    }
    
    if (dlp_exec(cmd, 0) != 0) {
        dlp_error("Failed to install package: %s\n", package);
        return DLP_ERROR;
    }
    
    dlp_success("Package %s installed\n", package);
    return DLP_OK;
}

int dlp_install(int argc, char *argv[], dlp_options_t *opts) {
    if (argc == 0 || (argc == 1 && argv[0][0] == '-')) {
        /* Install Droy itself */
        return dlp_install_droy(opts);
    }
    
    /* Install specific packages */
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] != '-') {
            if (dlp_install_package(argv[i], opts) != DLP_OK) {
                return DLP_ERROR;
            }
        }
    }
    
    return DLP_OK;
}
