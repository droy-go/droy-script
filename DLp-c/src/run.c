/**
 * DLp - Run Module
 * Runs Droy programs
 */

#include "dlp.h"
#include <signal.h>

static pid_t child_pid = 0;

void signal_handler(int sig) {
    if (child_pid > 0) {
        kill(child_pid, sig);
    }
}

int dlp_run_file(const char *file, dlp_options_t *opts) {
    char *droy_bin = dlp_get_droy_bin();
    
    if (!dlp_file_exists(droy_bin)) {
        dlp_error("Droy not installed. Run: DLp install\n");
        return DLP_ERROR;
    }
    
    if (!dlp_file_exists(file)) {
        dlp_error("File not found: %s\n", file);
        return DLP_ERROR;
    }
    
    dlp_print(COLOR_BLUE, "\n▶️ Running: %s\n\n", file);
    
    char cmd[MAX_CMD];
    snprintf(cmd, MAX_CMD, "%s", droy_bin);
    
    /* Build args */
    char args[MAX_CMD] = {0};
    
    if (opts->verbose) {
        strcat(args, " -d");
    }
    if (opts->tokens) {
        strcat(args, " -t");
    }
    if (opts->ast) {
        strcat(args, " -a");
    }
    
    /* Run with exec */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    pid_t pid = fork();
    
    if (pid < 0) {
        dlp_error("Failed to fork process\n");
        return DLP_ERROR;
    } else if (pid == 0) {
        /* Child process */
        if (strlen(args) > 0) {
            execlp(droy_bin, "droy", args + 1, file, (char *)NULL);
        } else {
            execlp(droy_bin, "droy", file, (char *)NULL);
        }
        _exit(1);
    } else {
        /* Parent process */
        child_pid = pid;
        int status;
        waitpid(pid, &status, 0);
        child_pid = 0;
        
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return DLP_ERROR;
    }
}

int dlp_run_repl(void) {
    char *droy_bin = dlp_get_droy_bin();
    
    if (!dlp_file_exists(droy_bin)) {
        dlp_error("Droy not installed. Run: DLp install\n");
        return DLP_ERROR;
    }
    
    dlp_print(COLOR_BLUE, "\n🚀 Droy Language REPL\n");
    dlp_print(COLOR_GRAY, "Type 'exit' or press Ctrl+C to quit\n\n");
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    pid_t pid = fork();
    
    if (pid < 0) {
        dlp_error("Failed to fork process\n");
        return DLP_ERROR;
    } else if (pid == 0) {
        /* Child process */
        execlp(droy_bin, "droy", "-i", (char *)NULL);
        _exit(1);
    } else {
        /* Parent process */
        child_pid = pid;
        int status;
        waitpid(pid, &status, 0);
        child_pid = 0;
        
        printf("\n");
        
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return DLP_ERROR;
    }
}

int dlp_run(int argc, char *argv[], dlp_options_t *opts) {
    const char *file = NULL;
    
    if (argc > 0 && argv[0][0] != '-') {
        file = argv[0];
    }
    
    if (!file) {
        /* Try to find default entry point */
        if (dlp_file_exists("src/main.droy")) {
            file = "src/main.droy";
        } else if (dlp_file_exists("main.droy")) {
            file = "main.droy";
        } else {
            /* Start REPL */
            return dlp_run_repl();
        }
    }
    
    return dlp_run_file(file, opts);
}
