/**
 * DLp - Init Module
 * Creates new Droy projects
 */

#include "dlp.h"
#include <time.h>

int dlp_create_dirs(const char *project_dir) {
    char path[MAX_PATH];
    const char *dirs[] = {"src", "tests", "examples", "docs", NULL};
    
    for (int i = 0; dirs[i]; i++) {
        snprintf(path, MAX_PATH, "%s/%s", project_dir, dirs[i]);
        if (dlp_mkdir_p(path) != DLP_OK) {
            dlp_error("Failed to create directory: %s\n", path);
            return DLP_ERROR;
        }
    }
    
    return DLP_OK;
}

int dlp_write_toml(const char *project_dir, const char *name) {
    char path[MAX_PATH];
    snprintf(path, MAX_PATH, "%s/droy.toml", project_dir);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        dlp_error("Failed to create droy.toml\n");
        return DLP_ERROR;
    }
    
    fprintf(fp, "[project]\n");
    fprintf(fp, "name = \"%s\"\n", name);
    fprintf(fp, "version = \"1.0.0\"\n");
    fprintf(fp, "description = \"A Droy Language project\"\n");
    fprintf(fp, "author = \"Your Name <email@example.com>\"\n");
    fprintf(fp, "license = \"MIT\"\n\n");
    fprintf(fp, "[build]\n");
    fprintf(fp, "output = \"dist\"\n");
    fprintf(fp, "target = \"llvm\"\n");
    fprintf(fp, "optimize = true\n\n");
    fprintf(fp, "[scripts]\n");
    fprintf(fp, "start = \"DLp run src/main.droy\"\n");
    fprintf(fp, "build = \"DLp build\"\n");
    fprintf(fp, "test = \"DLp test\"\n\n");
    fprintf(fp, "[dependencies]\n\n");
    fprintf(fp, "[dev-dependencies]\n");
    
    fclose(fp);
    return DLP_OK;
}

int dlp_write_readme(const char *project_dir, const char *name) {
    char path[MAX_PATH];
    snprintf(path, MAX_PATH, "%s/README.md", project_dir);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        dlp_error("Failed to create README.md\n");
        return DLP_ERROR;
    }
    
    fprintf(fp, "# %s\n\n", name);
    fprintf(fp, "A Droy Language project created with DLp.\n\n");
    fprintf(fp, "## Getting Started\n\n");
    fprintf(fp, "```bash\n");
    fprintf(fp, "# Run the project\n");
    fprintf(fp, "DLp run\n\n");
    fprintf(fp, "# Build the project\n");
    fprintf(fp, "DLp build\n\n");
    fprintf(fp, "# Run tests\n");
    fprintf(fp, "DLp test\n");
    fprintf(fp, "```\n\n");
    fprintf(fp, "## Project Structure\n\n");
    fprintf(fp, "- `src/` - Source code\n");
    fprintf(fp, "- `tests/` - Test files\n");
    fprintf(fp, "- `examples/` - Example programs\n");
    fprintf(fp, "- `docs/` - Documentation\n\n");
    fprintf(fp, "## Learn More\n\n");
    fprintf(fp, "- [Droy Language](https://github.com/droy-go/droy-lang)\n");
    fprintf(fp, "- [DLp Documentation](https://github.com/droy-go/DLp)\n");
    
    fclose(fp);
    return DLP_OK;
}

int dlp_write_gitignore(const char *project_dir) {
    char path[MAX_PATH];
    snprintf(path, MAX_PATH, "%s/.gitignore", project_dir);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        dlp_error("Failed to create .gitignore\n");
        return DLP_ERROR;
    }
    
    fprintf(fp, "# DLp - Droy Language Package Manager\n");
    fprintf(fp, "dist/\n");
    fprintf(fp, "build/\n");
    fprintf(fp, "*.o\n");
    fprintf(fp, "*.ll\n");
    fprintf(fp, "*.s\n");
    fprintf(fp, "*.exe\n");
    fprintf(fp, "output*\n");
    fprintf(fp, ".dlp-cache/\n");
    fprintf(fp, ".cache/\n");
    
    fclose(fp);
    return DLP_OK;
}

int dlp_write_main_droy(const char *project_dir, const char *name) {
    char path[MAX_PATH];
    snprintf(path, MAX_PATH, "%s/src/main.droy", project_dir);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        dlp_error("Failed to create main.droy\n");
        return DLP_ERROR;
    }
    
    fprintf(fp, "// %s - Main Entry Point\n", name);
    fprintf(fp, "// Created with DLp - Droy Language Package Manager\n\n");
    fprintf(fp, "// Project info\n");
    fprintf(fp, "set projectName = \"%s\"\n", name);
    fprintf(fp, "set version = \"1.0.0\"\n\n");
    fprintf(fp, "// Welcome message\n");
    fprintf(fp, "text \"Welcome to \" + projectName + \"!\"\n");
    fprintf(fp, "text \"Version: \" + version\n\n");
    fprintf(fp, "// System variables\n");
    fprintf(fp, "~s @si = 100\n");
    fprintf(fp, "~s @ui = 200\n\n");
    fprintf(fp, "em \"Values: \" + @si + \", \" + @ui\n\n");
    fprintf(fp, "// Math operations\n");
    fprintf(fp, "set sum = @si + @ui\n");
    fprintf(fp, "set diff = @ui - @si\n\n");
    fprintf(fp, "em \"Sum: \" + sum\n");
    fprintf(fp, "em \"Difference: \" + diff\n\n");
    fprintf(fp, "text \"Program completed successfully!\"\n");
    
    fclose(fp);
    return DLP_OK;
}

int dlp_write_example_droy(const char *project_dir, const char *name) {
    char path[MAX_PATH];
    snprintf(path, MAX_PATH, "%s/examples/hello.droy", project_dir);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return DLP_ERROR;
    }
    
    fprintf(fp, "// Hello from %s!\n\n", name);
    fprintf(fp, "text \"Hello, World!\"\n");
    fprintf(fp, "text \"Welcome to %s!\"\n\n", name);
    fprintf(fp, "set message = \"Droy is awesome!\"\n");
    fprintf(fp, "em message\n");
    
    fclose(fp);
    return DLP_OK;
}

int dlp_create_project(const char *name, const char *template) {
    (void)template; /* Unused for now */
    
    char project_dir[MAX_PATH];
    
    /* Use absolute path */
    if (name[0] == '/') {
        strncpy(project_dir, name, MAX_PATH - 1);
    } else {
        char cwd[MAX_PATH];
        if (getcwd(cwd, MAX_PATH) == NULL) {
            dlp_error("Failed to get current directory\n");
            return DLP_ERROR;
        }
        snprintf(project_dir, MAX_PATH, "%s/%s", cwd, name);
    }
    
    /* Check if directory exists */
    if (dlp_dir_exists(project_dir)) {
        dlp_error("Directory already exists: %s\n", project_dir);
        return DLP_ERROR;
    }
    
    dlp_print(COLOR_CYAN, "\nCreating project: %s\n\n", name);
    
    /* Create directories */
    dlp_spinner_start("Creating directories");
    if (dlp_create_dirs(project_dir) != DLP_OK) {
        return DLP_ERROR;
    }
    dlp_spinner_stop("ok", "Directories created");
    
    /* Write files */
    dlp_spinner_start("Writing configuration");
    if (dlp_write_toml(project_dir, name) != DLP_OK) {
        return DLP_ERROR;
    }
    dlp_spinner_stop("ok", "Configuration written");
    
    dlp_spinner_start("Creating README");
    dlp_write_readme(project_dir, name);
    dlp_spinner_stop("ok", "README created");
    
    dlp_spinner_start("Creating .gitignore");
    dlp_write_gitignore(project_dir);
    dlp_spinner_stop("ok", ".gitignore created");
    
    dlp_spinner_start("Creating main.droy");
    dlp_write_main_droy(project_dir, name);
    dlp_spinner_stop("ok", "main.droy created");
    
    dlp_spinner_start("Creating example");
    dlp_write_example_droy(project_dir, name);
    dlp_spinner_stop("ok", "Example created");
    
    dlp_print(COLOR_GREEN, "\n✅ Project '%s' created successfully!\n\n", name);
    dlp_print(COLOR_WHITE, "Next steps:\n");
    dlp_print(COLOR_CYAN, "  cd %s\n", name);
    dlp_print(COLOR_CYAN, "  DLp run              # Run the project\n");
    dlp_print(COLOR_CYAN, "  DLp build            # Build the project\n\n");
    
    return DLP_OK;
}

int dlp_init(int argc, char *argv[], dlp_options_t *opts) {
    const char *name = opts->name;
    
    if (!name && argc > 0 && argv[0][0] != '-') {
        name = argv[0];
    }
    
    if (!name) {
        /* Interactive mode - ask for name */
        dlp_print(COLOR_WHITE, "\n🆕 Create New Project\n\n");
        dlp_print(COLOR_WHITE, "Enter project name: ");
        
        static char input[MAX_NAME];
        if (fgets(input, MAX_NAME, stdin) == NULL) {
            dlp_error("Failed to read input\n");
            return DLP_ERROR;
        }
        
        /* Remove newline */
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
        }
        
        /* Trim whitespace */
        char *start = input;
        while (isspace(*start)) start++;
        char *end = input + strlen(input) - 1;
        while (end > start && isspace(*end)) *end-- = '\0';
        
        if (strlen(start) == 0) {
            dlp_error("Project name cannot be empty\n");
            return DLP_ERROR;
        }
        
        name = start;
    }
    
    return dlp_create_project(name, opts->template);
}
