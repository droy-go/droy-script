/**
 * Droy Utilities - Helper Functions
 * ==================================
 * Common utility functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "../../include/droy.h"

/* Memory management */

void* droy_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr && size > 0) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return ptr;
}

void* droy_realloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        fprintf(stderr, "Memory reallocation failed\n");
        exit(1);
    }
    return new_ptr;
}

void droy_free(void* ptr) {
    free(ptr);
}

void* droy_calloc(size_t num, size_t size) {
    void* ptr = calloc(num, size);
    if (!ptr && num > 0 && size > 0) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return ptr;
}

char* droy_strdup(const char* str) {
    if (!str) return NULL;
    char* copy = (char*)droy_malloc(strlen(str) + 1);
    strcpy(copy, str);
    return copy;
}

/* File operations */

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read file
    char* content = (char*)droy_malloc(size + 1);
    size_t read = fread(content, 1, size, file);
    content[read] = '\0';
    
    fclose(file);
    return content;
}

bool write_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return false;
    }
    
    size_t written = fwrite(content, 1, strlen(content), file);
    fclose(file);
    
    return written == strlen(content);
}

bool file_exists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

bool dir_exists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

bool create_dir(const char* path) {
    #ifdef _WIN32
        return _mkdir(path) == 0 || errno == EEXIST;
    #else
        return mkdir(path, 0755) == 0 || errno == EEXIST;
    #endif
}

bool create_dir_recursive(const char* path) {
    char* copy = droy_strdup(path);
    char* p = copy;
    
    // Skip leading slash
    if (*p == '/') p++;
    
    while ((p = strchr(p, '/')) != NULL) {
        *p = '\0';
        if (!create_dir(copy)) {
            droy_free(copy);
            return false;
        }
        *p = '/';
        p++;
    }
    
    bool result = create_dir(copy);
    droy_free(copy);
    return result;
}

/* Path operations */

char* get_dirname(const char* path) {
    if (!path) return droy_strdup(".");
    
    char* copy = droy_strdup(path);
    char* last_slash = strrchr(copy, '/');
    
    if (last_slash) {
        *last_slash = '\0';
        return copy;
    }
    
    droy_free(copy);
    return droy_strdup(".");
}

char* get_basename(const char* path) {
    if (!path) return droy_strdup("");
    
    const char* last_slash = strrchr(path, '/');
    if (last_slash) {
        return droy_strdup(last_slash + 1);
    }
    
    return droy_strdup(path);
}

char* get_extension(const char* path) {
    if (!path) return droy_strdup("");
    
    const char* last_dot = strrchr(path, '.');
    if (last_dot) {
        return droy_strdup(last_dot + 1);
    }
    
    return droy_strdup("");
}

char* join_path(const char* a, const char* b) {
    if (!a) return droy_strdup(b ? b : "");
    if (!b) return droy_strdup(a);
    
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    
    bool need_sep = (len_a > 0 && a[len_a - 1] != '/') &&
                    (len_b > 0 && b[0] != '/');
    
    char* result = (char*)droy_malloc(len_a + len_b + (need_sep ? 2 : 1));
    
    strcpy(result, a);
    if (need_sep) {
        strcat(result, "/");
    }
    strcat(result, b);
    
    return result;
}

char* get_cwd(void) {
    char buffer[4096];
    if (getcwd(buffer, sizeof(buffer))) {
        return droy_strdup(buffer);
    }
    return droy_strdup(".");
}

bool set_cwd(const char* path) {
    return chdir(path) == 0;
}

/* String operations */

char** split_string(const char* str, const char* delim, int* count) {
    if (!str || !delim || !count) return NULL;
    
    *count = 0;
    char* copy = droy_strdup(str);
    char* token = strtok(copy, delim);
    
    // Count tokens
    while (token) {
        (*count)++;
        token = strtok(NULL, delim);
    }
    
    if (*count == 0) {
        droy_free(copy);
        return NULL;
    }
    
    // Allocate result
    char** result = (char**)droy_malloc(*count * sizeof(char*));
    
    // Copy string again for second pass
    strcpy(copy, str);
    token = strtok(copy, delim);
    
    int i = 0;
    while (token) {
        result[i++] = droy_strdup(token);
        token = strtok(NULL, delim);
    }
    
    droy_free(copy);
    return result;
}

char* trim_string(char* str) {
    if (!str) return NULL;
    
    // Trim leading whitespace
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == '\0') return str;
    
    // Trim trailing whitespace
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return str;
}

char* trim_string_copy(const char* str) {
    if (!str) return NULL;
    
    const char* start = str;
    while (isspace((unsigned char)*start)) start++;
    
    if (*start == '\0') return droy_strdup("");
    
    const char* end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    size_t len = end - start + 1;
    char* result = (char*)droy_malloc(len + 1);
    strncpy(result, start, len);
    result[len] = '\0';
    
    return result;
}

bool starts_with(const char* str, const char* prefix) {
    if (!str || !prefix) return false;
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool ends_with(const char* str, const char* suffix) {
    if (!str || !suffix) return false;
    
    size_t len_str = strlen(str);
    size_t len_suffix = strlen(suffix);
    
    if (len_suffix > len_str) return false;
    
    return strcmp(str + len_str - len_suffix, suffix) == 0;
}

char* replace_string(const char* str, const char* old, const char* new) {
    if (!str || !old || !new) return NULL;
    
    size_t len_old = strlen(old);
    size_t len_new = strlen(new);
    
    // Count occurrences
    int count = 0;
    const char* tmp = str;
    while ((tmp = strstr(tmp, old)) != NULL) {
        count++;
        tmp += len_old;
    }
    
    if (count == 0) return droy_strdup(str);
    
    // Allocate result
    size_t result_len = strlen(str) + count * (len_new - len_old) + 1;
    char* result = (char*)droy_malloc(result_len);
    
    // Replace
    char* dst = result;
    const char* src = str;
    while ((tmp = strstr(src, old)) != NULL) {
        size_t prefix_len = tmp - src;
        memcpy(dst, src, prefix_len);
        dst += prefix_len;
        memcpy(dst, new, len_new);
        dst += len_new;
        src = tmp + len_old;
    }
    strcpy(dst, src);
    
    return result;
}

char* to_lower(const char* str) {
    if (!str) return NULL;
    
    char* result = droy_strdup(str);
    for (char* p = result; *p; p++) {
        *p = tolower((unsigned char)*p);
    }
    return result;
}

char* to_upper(const char* str) {
    if (!str) return NULL;
    
    char* result = droy_strdup(str);
    for (char* p = result; *p; p++) {
        *p = toupper((unsigned char)*p);
    }
    return result;
}

unsigned long hash_string(const char* str) {
    if (!str) return 0;
    
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/* Error handling */

static void (*error_handler)(const char* file, int line, int column, const char* message) = NULL;

void set_error_handler(void (*handler)(const char* file, int line, int column, const char* message)) {
    error_handler = handler;
}

void droy_error(const char* file, int line, int column, const char* message) {
    if (error_handler) {
        error_handler(file, line, column, message);
    } else {
        fprintf(stderr, "[%s:%d:%d] Error: %s\n", file, line, column, message);
    }
}

void droy_warning(const char* file, int line, int column, const char* message) {
    fprintf(stderr, "[%s:%d:%d] Warning: %s\n", file, line, column, message);
}

void droy_info(const char* file, int line, int column, const char* message) {
    fprintf(stdout, "[%s:%d:%d] Info: %s\n", file, line, column, message);
}

/* Module functions */

Module* module_create(const char* name, const char* path) {
    Module* mod = (Module*)droy_calloc(1, sizeof(Module));
    mod->name = droy_strdup(name);
    mod->path = droy_strdup(path);
    mod->source = NULL;
    mod->tokens = NULL;
    mod->ast = NULL;
    mod->scope = NULL;
    mod->exports = NULL;
    mod->export_count = 0;
    mod->is_loaded = false;
    mod->is_main = false;
    mod->imports = NULL;
    mod->import_count = 0;
    mod->package = NULL;
    return mod;
}

void module_destroy(Module* mod) {
    if (!mod) return;
    
    droy_free(mod->name);
    droy_free(mod->path);
    droy_free(mod->source);
    
    // Tokens are freed by lexer
    // AST is freed separately
    
    if (mod->scope) {
        scope_destroy(mod->scope);
    }
    
    for (int i = 0; i < mod->export_count; i++) {
        value_free(mod->exports[i]);
    }
    droy_free(mod->exports);
    
    droy_free(mod->imports);
    droy_free(mod);
}

bool module_load_source(Module* mod, const char* source) {
    if (!mod || !source) return false;
    mod->source = droy_strdup(source);
    return true;
}

bool module_tokenize(Module* mod) {
    if (!mod || !mod->source) return false;
    
    Lexer* lexer = lexer_create(mod->source, mod->path);
    mod->tokens = lexer_tokenize(lexer);
    
    // Keep lexer alive for token access
    // lexer_destroy(lexer);
    
    return mod->tokens != NULL;
}

bool module_parse(Module* mod) {
    if (!mod || !mod->tokens) return false;
    
    Parser* parser = parser_create(mod->tokens);
    mod->ast = parser_parse(parser);
    parser_destroy(parser);
    
    return mod->ast != NULL;
}

bool module_execute(Module* mod, Interpreter* interp) {
    if (!mod || !mod->ast || !interp) return false;
    
    mod->scope = scope_create(interp->global_scope, mod->name);
    scope_push(interp, mod->scope);
    
    int result = interpreter_run(interp, mod->ast);
    
    scope_pop(interp);
    
    mod->is_loaded = true;
    
    return result == 0;
}

Value* module_get_export(Module* mod, const char* name) {
    if (!mod || !name) return NULL;
    
    return scope_get_value(mod->scope, name);
}

bool module_add_export(Module* mod, const char* name, Value* value) {
    if (!mod || !name || !value) return false;
    
    Variable* var = scope_get(mod->scope, name);
    if (var) {
        var->is_exported = true;
        return true;
    }
    
    return false;
}

/* Package functions */

Package* package_create(const char* name, const char* version) {
    Package* pkg = (Package*)droy_calloc(1, sizeof(Package));
    pkg->name = droy_strdup(name);
    pkg->version = droy_strdup(version);
    pkg->description = NULL;
    pkg->author = NULL;
    pkg->license = NULL;
    pkg->repository = NULL;
    pkg->homepage = NULL;
    pkg->keywords = NULL;
    pkg->keyword_count = 0;
    pkg->main = NULL;
    pkg->dependencies = NULL;
    pkg->dependency_count = 0;
    pkg->dev_dependencies = NULL;
    pkg->dev_dependency_count = 0;
    pkg->module = NULL;
    pkg->is_installed = false;
    pkg->is_loaded = false;
    pkg->install_path = NULL;
    pkg->next = NULL;
    return pkg;
}

void package_destroy(Package* pkg) {
    if (!pkg) return;
    
    droy_free(pkg->name);
    droy_free(pkg->version);
    droy_free(pkg->description);
    droy_free(pkg->author);
    droy_free(pkg->license);
    droy_free(pkg->repository);
    droy_free(pkg->homepage);
    
    for (int i = 0; i < pkg->keyword_count; i++) {
        droy_free(pkg->keywords[i]);
    }
    droy_free(pkg->keywords);
    
    droy_free(pkg->main);
    
    for (int i = 0; i < pkg->dependency_count; i++) {
        droy_free(pkg->dependencies[i]);
    }
    droy_free(pkg->dependencies);
    
    for (int i = 0; i < pkg->dev_dependency_count; i++) {
        droy_free(pkg->dev_dependencies[i]);
    }
    droy_free(pkg->dev_dependencies);
    
    if (pkg->module) {
        module_destroy(pkg->module);
    }
    
    droy_free(pkg->install_path);
    droy_free(pkg);
}

bool package_add_dependency(Package* pkg, const char* name, const char* version) {
    if (!pkg || !name) return false;
    
    // Check if already exists
    for (int i = 0; i < pkg->dependency_count; i++) {
        if (strcmp(pkg->dependencies[i], name) == 0) {
            return true;
        }
    }
    
    // Add dependency
    pkg->dependencies = (char**)droy_realloc(pkg->dependencies,
        (pkg->dependency_count + 1) * sizeof(char*));
    pkg->dependencies[pkg->dependency_count++] = droy_strdup(name);
    
    return true;
}

bool package_add_keyword(Package* pkg, const char* keyword) {
    if (!pkg || !keyword) return false;
    
    pkg->keywords = (char**)droy_realloc(pkg->keywords,
        (pkg->keyword_count + 1) * sizeof(char*));
    pkg->keywords[pkg->keyword_count++] = droy_strdup(keyword);
    
    return true;
}

/* Link functions */

Link* link_create(const char* id, const char* url) {
    Link* link = (Link*)droy_malloc(sizeof(Link));
    link->id = droy_strdup(id);
    link->url = droy_strdup(url);
    link->api = NULL;
    link->method = droy_strdup("GET");
    link->headers = NULL;
    link->header_count = 0;
    link->is_open = false;
    link->is_persistent = false;
    link->handle = NULL;
    link->next = NULL;
    return link;
}

void link_destroy(Link* link) {
    if (!link) return;
    
    droy_free(link->id);
    droy_free(link->url);
    droy_free(link->api);
    droy_free(link->method);
    
    for (int i = 0; i < link->header_count; i++) {
        droy_free(link->headers[i]);
    }
    droy_free(link->headers);
    
    droy_free(link);
}

bool link_set_header(Link* link, const char* key, const char* value) {
    if (!link || !key || !value) return false;
    
    // Format: "key: value"
    size_t len = strlen(key) + strlen(value) + 3;
    char* header = (char*)droy_malloc(len);
    sprintf(header, "%s: %s", key, value);
    
    link->headers = (char**)droy_realloc(link->headers,
        (link->header_count + 1) * sizeof(char*));
    link->headers[link->header_count++] = header;
    
    return true;
}
