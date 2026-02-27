/**
 * DLp - Droy Language Package Manager
 * Main Entry Point
 */

#include "dlp.h"
#include <stdarg.h>

static int verbose_mode = 0;
static int spinner_active = 0;

void print_banner(void) {
    printf(COLOR_BLUE "\n");
    printf("╔═══════════════════════════════════════════╗\n");
    printf("║                                           ║\n");
    printf("║   🚀 DLp - Droy Language Package Manager  ║\n");
    printf("║                                           ║\n");
    printf("╚═══════════════════════════════════════════╝\n");
    printf(COLOR_RESET "\n");
}

void print_version(void) {
    printf("DLp version %s\n", DLP_VERSION);
    printf("Droy Language Package Manager\n");
    printf("Copyright (c) 2026 Droy Team\n");
}

void print_help(void) {
    printf("Usage: DLp <command> [options] [args...]\n\n");
    printf("Commands:\n");
    printf("  install [packages...]  Install Droy or packages\n");
    printf("  init [name]            Create new project\n");
    printf("  build [file]           Build project\n");
    printf("  run [file]             Run Droy program\n");
    printf("  add <packages...>      Add packages\n");
    printf("  remove <packages...>   Remove packages\n");
    printf("  update [packages...]   Update packages\n");
    printf("  list                   List installed packages\n");
    printf("  search <query>         Search packages\n");
    printf("  test                   Run tests\n");
    printf("  clean                  Clean build artifacts\n");
    printf("  repl                   Start REPL mode\n");
    printf("  info                   Show project info\n");
    printf("  doctor                 Check system health\n");
    printf("  version                Show version\n");
    printf("  help                   Show this help\n\n");
    printf("Options:\n");
    printf("  -g, --global           Global installation\n");
    printf("  -f, --force            Force reinstall\n");
    printf("  -v, --verbose          Verbose output\n");
    printf("  -D, --dev              Dev dependency\n");
    printf("  -t, --target <target>  Build target (llvm, bin, asm)\n");
    printf("  -o, --output <dir>     Output directory\n");
    printf("  --template <tmpl>      Project template\n\n");
}

dlp_command_t parse_command(const char *cmd) {
    if (!cmd) return CMD_UNKNOWN;
    if (strcmp(cmd, "install") == 0 || strcmp(cmd, "i") == 0) return CMD_INSTALL;
    if (strcmp(cmd, "init") == 0 || strcmp(cmd, "create") == 0) return CMD_INIT;
    if (strcmp(cmd, "build") == 0 || strcmp(cmd, "b") == 0) return CMD_BUILD;
    if (strcmp(cmd, "run") == 0 || strcmp(cmd, "r") == 0) return CMD_RUN;
    if (strcmp(cmd, "add") == 0 || strcmp(cmd, "a") == 0) return CMD_ADD;
    if (strcmp(cmd, "remove") == 0 || strcmp(cmd, "rm") == 0) return CMD_REMOVE;
    if (strcmp(cmd, "update") == 0 || strcmp(cmd, "up") == 0) return CMD_UPDATE;
    if (strcmp(cmd, "list") == 0 || strcmp(cmd, "ls") == 0) return CMD_LIST;
    if (strcmp(cmd, "search") == 0 || strcmp(cmd, "s") == 0) return CMD_SEARCH;
    if (strcmp(cmd, "test") == 0 || strcmp(cmd, "t") == 0) return CMD_TEST;
    if (strcmp(cmd, "clean") == 0 || strcmp(cmd, "c") == 0) return CMD_CLEAN;
    if (strcmp(cmd, "repl") == 0) return CMD_REPL;
    if (strcmp(cmd, "info") == 0) return CMD_INFO;
    if (strcmp(cmd, "doctor") == 0) return CMD_DOCTOR;
    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0) return CMD_HELP;
    if (strcmp(cmd, "version") == 0 || strcmp(cmd, "v") == 0) return CMD_VERSION;
    return CMD_UNKNOWN;
}

int execute_command(dlp_command_t cmd, int argc, char *argv[]) {
    dlp_options_t opts = {0};
    opts.target = "llvm";
    opts.output = "dist";
    opts.template = "basic";
    
    /* Parse options */
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--global") == 0) {
            opts.global = 1;
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0) {
            opts.force = 1;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            opts.verbose = 1;
            verbose_mode = 1;
        } else if (strcmp(argv[i], "-D") == 0 || strcmp(argv[i], "--dev") == 0) {
            opts.dev = 1;
        } else if (strcmp(argv[i], "-S") == 0 || strcmp(argv[i], "--save") == 0) {
            opts.save = 1;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--target") == 0) {
            if (i + 1 < argc) opts.target = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) opts.output = argv[++i];
        } else if (strcmp(argv[i], "--template") == 0) {
            if (i + 1 < argc) opts.template = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0) {
            if (i + 1 < argc) opts.name = argv[++i];
        }
    }
    
    switch (cmd) {
        case CMD_INSTALL:
            return dlp_install(argc, argv, &opts);
        case CMD_INIT:
            return dlp_init(argc, argv, &opts);
        case CMD_BUILD:
            return dlp_build(argc, argv, &opts);
        case CMD_RUN:
            return dlp_run(argc, argv, &opts);
        case CMD_ADD:
            return dlp_add(argc, argv, &opts);
        case CMD_REMOVE:
            return dlp_remove(argc, argv, &opts);
        case CMD_UPDATE:
            return dlp_update(argc, argv, &opts);
        case CMD_LIST:
            return dlp_list(argc, argv, &opts);
        case CMD_SEARCH:
            return dlp_search(argc, argv, &opts);
        case CMD_TEST:
            return dlp_test(argc, argv, &opts);
        case CMD_CLEAN:
            return dlp_clean(argc, argv, &opts);
        case CMD_REPL:
            return dlp_run_repl();
        case CMD_INFO:
            return dlp_info(argc, argv, &opts);
        case CMD_DOCTOR:
            return dlp_doctor(argc, argv, &opts);
        case CMD_VERSION:
            print_version();
            return DLP_OK;
        case CMD_HELP:
            print_help();
            return DLP_OK;
        default:
            dlp_error("Unknown command. Use 'DLp help' for usage.\n");
            return DLP_ERROR;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_banner();
        print_help();
        return DLP_OK;
    }
    
    dlp_command_t cmd = parse_command(argv[1]);
    
    if (cmd == CMD_UNKNOWN && argv[1][0] != '-') {
        /* Try to run as file */
        if (dlp_file_exists(argv[1])) {
            dlp_options_t opts = {0};
            return dlp_run_file(argv[1], &opts);
        }
        dlp_error("Unknown command: %s\n", argv[1]);
        return DLP_ERROR;
    }
    
    return execute_command(cmd, argc - 2, argv + 2);
}
