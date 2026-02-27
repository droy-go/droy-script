/**
 * Droy Built-in Functions
 * =======================
 * Native functions available in Droy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <curl/curl.h>
#include "../../include/droy.h"

/* I/O Functions */

Value* builtin_print(int argc, Value** args) {
    for (int i = 0; i < argc; i++) {
        if (i > 0) printf(" ");
        char* str = value_to_string(args[i]);
        printf("%s", str);
        droy_free(str);
    }
    return value_create_null();
}

Value* builtin_println(int argc, Value** args) {
    builtin_print(argc, args);
    printf("\n");
    return value_create_null();
}

Value* builtin_input(int argc, Value** args) {
    if (argc > 0) {
        char* prompt = value_to_string(args[0]);
        printf("%s", prompt);
        droy_free(prompt);
    }
    
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        // Remove newline
        buffer[strcspn(buffer, "\n")] = '\0';
        return value_create_string(buffer);
    }
    
    return value_create_string("");
}

/* Type Functions */

Value* builtin_type(int argc, Value** args) {
    if (argc < 1) return value_create_string("undefined");
    
    switch (args[0]->type) {
        case VAL_NUMBER: return value_create_string("number");
        case VAL_STRING: return value_create_string("string");
        case VAL_BOOLEAN: return value_create_string("boolean");
        case VAL_NULL: return value_create_string("null");
        case VAL_ARRAY: return value_create_string("array");
        case VAL_OBJECT: return value_create_string("object");
        case VAL_FUNCTION: return value_create_string("function");
        case VAL_NATIVE: return value_create_string("function");
        default: return value_create_string("unknown");
    }
}

Value* builtin_len(int argc, Value** args) {
    if (argc < 1) return value_create_number(0);
    
    switch (args[0]->type) {
        case VAL_STRING:
            return value_create_number(strlen(args[0]->as.string));
        case VAL_ARRAY:
            return value_create_number(args[0]->as.array.count);
        case VAL_OBJECT:
            return value_create_number(args[0]->as.object.count);
        default:
            return value_create_number(0);
    }
}

/* Array Functions */

Value* builtin_push(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value* array = args[0];
    
    // Ensure capacity
    if (array->as.array.count >= array->as.array.capacity) {
        array->as.array.capacity *= 2;
        array->as.array.items = (Value**)droy_realloc(array->as.array.items,
            array->as.array.capacity * sizeof(Value*));
    }
    
    // Add elements
    for (int i = 1; i < argc; i++) {
        array->as.array.items[array->as.array.count++] = value_copy(args[i]);
    }
    
    return value_create_number(array->as.array.count);
}

Value* builtin_pop(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value* array = args[0];
    
    if (array->as.array.count == 0) {
        return value_create_null();
    }
    
    Value* item = array->as.array.items[--array->as.array.count];
    return item;
}

Value* builtin_shift(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value* array = args[0];
    
    if (array->as.array.count == 0) {
        return value_create_null();
    }
    
    Value* item = array->as.array.items[0];
    
    // Shift elements
    for (int i = 0; i < array->as.array.count - 1; i++) {
        array->as.array.items[i] = array->as.array.items[i + 1];
    }
    array->as.array.count--;
    
    return item;
}

Value* builtin_unshift(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value* array = args[0];
    
    // Ensure capacity
    int new_count = array->as.array.count + argc - 1;
    if (new_count >= array->as.array.capacity) {
        while (array->as.array.capacity < new_count) {
            array->as.array.capacity *= 2;
        }
        array->as.array.items = (Value**)droy_realloc(array->as.array.items,
            array->as.array.capacity * sizeof(Value*));
    }
    
    // Shift existing elements
    for (int i = array->as.array.count - 1; i >= 0; i--) {
        array->as.array.items[i + argc - 1] = array->as.array.items[i];
    }
    
    // Add new elements
    for (int i = 1; i < argc; i++) {
        array->as.array.items[i - 1] = value_copy(args[i]);
    }
    
    array->as.array.count = new_count;
    
    return value_create_number(array->as.array.count);
}

Value* builtin_slice(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value* array = args[0];
    int start = (argc > 1 && args[1]->type == VAL_NUMBER) ? (int)args[1]->as.number : 0;
    int end = (argc > 2 && args[2]->type == VAL_NUMBER) ? (int)args[2]->as.number : array->as.array.count;
    
    // Handle negative indices
    if (start < 0) start = array->as.array.count + start;
    if (end < 0) end = array->as.array.count + end;
    
    // Clamp
    if (start < 0) start = 0;
    if (end > array->as.array.count) end = array->as.array.count;
    if (start > end) start = end;
    
    // Create new array
    Value* result = value_create_array(end - start);
    for (int i = start; i < end; i++) {
        result->as.array.items[result->as.array.count++] = value_copy(array->as.array.items[i]);
    }
    
    return result;
}

/* String Functions */

Value* builtin_split(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_STRING || args[1]->type != VAL_STRING) {
        return value_create_array(0);
    }
    
    char* str = droy_strdup(args[0]->as.string);
    const char* delim = args[1]->as.string;
    
    Value* result = value_create_array(8);
    
    char* token = strtok(str, delim);
    while (token) {
        if (result->as.array.count >= result->as.array.capacity) {
            result->as.array.capacity *= 2;
            result->as.array.items = (Value**)droy_realloc(result->as.array.items,
                result->as.array.capacity * sizeof(Value*));
        }
        result->as.array.items[result->as.array.count++] = value_create_string(token);
        token = strtok(NULL, delim);
    }
    
    droy_free(str);
    return result;
}

Value* builtin_join(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_ARRAY || args[1]->type != VAL_STRING) {
        return value_create_string("");
    }
    
    Value* array = args[0];
    const char* delim = args[1]->as.string;
    
    // Calculate total length
    size_t total_len = 0;
    for (int i = 0; i < array->as.array.count; i++) {
        char* str = value_to_string(array->as.array.items[i]);
        total_len += strlen(str);
        droy_free(str);
        if (i < array->as.array.count - 1) {
            total_len += strlen(delim);
        }
    }
    
    char* result = (char*)droy_malloc(total_len + 1);
    result[0] = '\0';
    
    for (int i = 0; i < array->as.array.count; i++) {
        char* str = value_to_string(array->as.array.items[i]);
        strcat(result, str);
        droy_free(str);
        if (i < array->as.array.count - 1) {
            strcat(result, delim);
        }
    }
    
    Value* val = value_create_string(result);
    droy_free(result);
    return val;
}

Value* builtin_replace(int argc, Value** args) {
    if (argc < 3 || args[0]->type != VAL_STRING || 
        args[1]->type != VAL_STRING || args[2]->type != VAL_STRING) {
        return value_create_string("");
    }
    
    char* result = replace_string(args[0]->as.string, args[1]->as.string, args[2]->as.string);
    Value* val = value_create_string(result);
    droy_free(result);
    return val;
}

Value* builtin_contains(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_STRING || args[1]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(strstr(args[0]->as.string, args[1]->as.string) != NULL);
}

Value* builtin_index_of(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_STRING || args[1]->type != VAL_STRING) {
        return value_create_number(-1);
    }
    
    const char* found = strstr(args[0]->as.string, args[1]->as.string);
    if (found) {
        return value_create_number(found - args[0]->as.string);
    }
    return value_create_number(-1);
}

/* Conversion Functions */

Value* builtin_to_string(int argc, Value** args) {
    if (argc < 1) return value_create_string("");
    
    char* str = value_to_string(args[0]);
    Value* result = value_create_string(str);
    droy_free(str);
    return result;
}

Value* builtin_to_number(int argc, Value** args) {
    if (argc < 1) return value_create_number(0);
    
    if (args[0]->type == VAL_NUMBER) {
        return value_copy(args[0]);
    }
    
    if (args[0]->type == VAL_STRING) {
        return value_create_number(atof(args[0]->as.string));
    }
    
    return value_create_number(0);
}

Value* builtin_parse_json(int argc, Value** args) {
    // TODO: Implement JSON parsing
    return value_create_null();
}

Value* builtin_stringify_json(int argc, Value** args) {
    if (argc < 1) return value_create_string("null");
    
    char* str = value_to_string(args[0]);
    Value* result = value_create_string(str);
    droy_free(str);
    return result;
}

/* System Functions */

Value* builtin_exit(int argc, Value** args) {
    int code = (argc > 0 && args[0]->type == VAL_NUMBER) ? (int)args[0]->as.number : 0;
    exit(code);
    return value_create_null();
}

Value* builtin_sleep(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_NUMBER) {
        return value_create_null();
    }
    
    usleep((useconds_t)(args[0]->as.number * 1000000));
    return value_create_null();
}

Value* builtin_time(int argc, Value** args) {
    return value_create_number((double)time(NULL));
}

/* Math Functions */

Value* builtin_random(int argc, Value** args) {
    return value_create_number((double)rand() / RAND_MAX);
}

Value* builtin_floor(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_NUMBER) {
        return value_create_number(0);
    }
    return value_create_number(floor(args[0]->as.number));
}

Value* builtin_ceil(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_NUMBER) {
        return value_create_number(0);
    }
    return value_create_number(ceil(args[0]->as.number));
}

Value* builtin_round(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_NUMBER) {
        return value_create_number(0);
    }
    return value_create_number(round(args[0]->as.number));
}

Value* builtin_abs(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_NUMBER) {
        return value_create_number(0);
    }
    return value_create_number(fabs(args[0]->as.number));
}

Value* builtin_sqrt(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_NUMBER) {
        return value_create_number(0);
    }
    return value_create_number(sqrt(args[0]->as.number));
}

Value* builtin_pow(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_NUMBER || args[1]->type != VAL_NUMBER) {
        return value_create_number(0);
    }
    return value_create_number(pow(args[0]->as.number, args[1]->as.number));
}

Value* builtin_min(int argc, Value** args) {
    if (argc < 1) return value_create_number(0);
    
    double min = args[0]->type == VAL_NUMBER ? args[0]->as.number : 0;
    for (int i = 1; i < argc; i++) {
        if (args[i]->type == VAL_NUMBER && args[i]->as.number < min) {
            min = args[i]->as.number;
        }
    }
    return value_create_number(min);
}

Value* builtin_max(int argc, Value** args) {
    if (argc < 1) return value_create_number(0);
    
    double max = args[0]->type == VAL_NUMBER ? args[0]->as.number : 0;
    for (int i = 1; i < argc; i++) {
        if (args[i]->type == VAL_NUMBER && args[i]->as.number > max) {
            max = args[i]->as.number;
        }
    }
    return value_create_number(max);
}

Value* builtin_range(int argc, Value** args) {
    int start = 0;
    int end = 0;
    int step = 1;
    
    if (argc == 1) {
        end = (int)args[0]->as.number;
    } else if (argc >= 2) {
        start = (int)args[0]->as.number;
        end = (int)args[1]->as.number;
        if (argc >= 3) {
            step = (int)args[2]->as.number;
        }
    }
    
    if (step == 0) step = 1;
    
    int count = (end - start) / step;
    if (count < 0) count = 0;
    
    Value* result = value_create_array(count);
    for (int i = 0; i < count; i++) {
        result->as.array.items[i] = value_create_number(start + i * step);
        result->as.array.count++;
    }
    
    return result;
}

/* Functional Functions */

Value* builtin_map(int argc, Value** args) {
    // TODO: Implement map
    return value_create_null();
}

Value* builtin_filter(int argc, Value** args) {
    // TODO: Implement filter
    return value_create_null();
}

Value* builtin_reduce(int argc, Value** args) {
    // TODO: Implement reduce
    return value_create_null();
}

Value* builtin_foreach(int argc, Value** args) {
    // TODO: Implement foreach
    return value_create_null();
}

Value* builtin_sort(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_ARRAY) {
        return value_create_null();
    }
    
    // Simple bubble sort
    Value* array = args[0];
    int n = array->as.array.count;
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (value_compare(array->as.array.items[j], array->as.array.items[j + 1]) > 0) {
                Value* temp = array->as.array.items[j];
                array->as.array.items[j] = array->as.array.items[j + 1];
                array->as.array.items[j + 1] = temp;
            }
        }
    }
    
    return value_copy(array);
}

Value* builtin_reverse(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value* array = args[0];
    int n = array->as.array.count;
    
    for (int i = 0; i < n / 2; i++) {
        Value* temp = array->as.array.items[i];
        array->as.array.items[i] = array->as.array.items[n - 1 - i];
        array->as.array.items[n - 1 - i] = temp;
    }
    
    return value_copy(array);
}

/* Object Functions */

Value* builtin_keys(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_OBJECT) {
        return value_create_array(0);
    }
    
    Value* obj = args[0];
    Value* result = value_create_array(obj->as.object.count);
    
    for (int i = 0; i < obj->as.object.count; i++) {
        result->as.array.items[i] = value_create_string(obj->as.object.keys[i]);
        result->as.array.count++;
    }
    
    return result;
}

Value* builtin_values(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_OBJECT) {
        return value_create_array(0);
    }
    
    Value* obj = args[0];
    Value* result = value_create_array(obj->as.object.count);
    
    for (int i = 0; i < obj->as.object.count; i++) {
        result->as.array.items[i] = value_copy(obj->as.object.values[i]);
        result->as.array.count++;
    }
    
    return result;
}

Value* builtin_entries(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_OBJECT) {
        return value_create_array(0);
    }
    
    Value* obj = args[0];
    Value* result = value_create_array(obj->as.object.count);
    
    for (int i = 0; i < obj->as.object.count; i++) {
        Value* entry = value_create_array(2);
        entry->as.array.items[0] = value_create_string(obj->as.object.keys[i]);
        entry->as.array.items[1] = value_copy(obj->as.object.values[i]);
        entry->as.array.count = 2;
        
        result->as.array.items[i] = entry;
        result->as.array.count++;
    }
    
    return result;
}

Value* builtin_has_key(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_OBJECT || args[1]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    Value* obj = args[0];
    const char* key = args[1]->as.string;
    
    for (int i = 0; i < obj->as.object.count; i++) {
        if (strcmp(obj->as.object.keys[i], key) == 0) {
            return value_create_boolean(true);
        }
    }
    
    return value_create_boolean(false);
}

/* File Functions */

Value* builtin_read_file(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_null();
    }
    
    char* content = read_file(args[0]->as.string);
    if (content) {
        Value* result = value_create_string(content);
        droy_free(content);
        return result;
    }
    
    return value_create_null();
}

Value* builtin_write_file(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_STRING || args[1]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    bool success = write_file(args[0]->as.string, args[1]->as.string);
    return value_create_boolean(success);
}

Value* builtin_append_file(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_STRING || args[1]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    FILE* file = fopen(args[0]->as.string, "a");
    if (!file) {
        return value_create_boolean(false);
    }
    
    fprintf(file, "%s", args[1]->as.string);
    fclose(file);
    
    return value_create_boolean(true);
}

Value* builtin_delete_file(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(remove(args[0]->as.string) == 0);
}

Value* builtin_exists(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(file_exists(args[0]->as.string));
}

Value* builtin_is_file(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(file_exists(args[0]->as.string));
}

Value* builtin_is_dir(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(dir_exists(args[0]->as.string));
}

Value* builtin_mkdir(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(create_dir(args[0]->as.string));
}

Value* builtin_rmdir(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(rmdir(args[0]->as.string) == 0);
}

Value* builtin_list_dir(int argc, Value** args) {
    const char* path = (argc > 0 && args[0]->type == VAL_STRING) ? args[0]->as.string : ".";
    
    DIR* dir = opendir(path);
    if (!dir) {
        return value_create_array(0);
    }
    
    Value* result = value_create_array(8);
    struct dirent* entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        if (result->as.array.count >= result->as.array.capacity) {
            result->as.array.capacity *= 2;
            result->as.array.items = (Value**)droy_realloc(result->as.array.items,
                result->as.array.capacity * sizeof(Value*));
        }
        
        result->as.array.items[result->as.array.count++] = value_create_string(entry->d_name);
    }
    
    closedir(dir);
    return result;
}

Value* builtin_chdir(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(set_cwd(args[0]->as.string));
}

Value* builtin_getcwd(int argc, Value** args) {
    char* cwd = get_cwd();
    Value* result = value_create_string(cwd);
    droy_free(cwd);
    return result;
}

Value* builtin_getenv(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_null();
    }
    
    const char* value = getenv(args[0]->as.string);
    if (value) {
        return value_create_string(value);
    }
    
    return value_create_null();
}

Value* builtin_setenv(int argc, Value** args) {
    if (argc < 2 || args[0]->type != VAL_STRING || args[1]->type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(setenv(args[0]->as.string, args[1]->as.string, 1) == 0);
}

Value* builtin_exec(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_number(-1);
    }
    
    int status = system(args[0]->as.string);
    return value_create_number(WEXITSTATUS(status));
}

/* Network Functions */

typedef struct {
    char* data;
    size_t size;
} CurlResponse;

static size_t curl_write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    CurlResponse* response = (CurlResponse*)userp;
    
    char* ptr = realloc(response->data, response->size + total_size + 1);
    if (!ptr) return 0;
    
    response->data = ptr;
    memcpy(&(response->data[response->size]), contents, total_size);
    response->size += total_size;
    response->data[response->size] = '\0';
    
    return total_size;
}

Value* builtin_fetch(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_null();
    }
    
    const char* url = args[0]->as.string;
    const char* method = (argc > 1 && args[1]->type == VAL_STRING) ? args[1]->as.string : "GET";
    const char* body = (argc > 2 && args[2]->type == VAL_STRING) ? args[2]->as.string : NULL;
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        return value_create_null();
    }
    
    CurlResponse response = {0};
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    if (strcmp(method, "POST") == 0) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (body) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
        }
    }
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        free(response.data);
        return value_create_null();
    }
    
    Value* result = value_create_string(response.data);
    free(response.data);
    
    return result;
}

/* Encoding Functions */

Value* builtin_encode_url(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_string("");
    }
    
    // Simple URL encoding
    const char* str = args[0]->as.string;
    size_t len = strlen(str);
    char* result = (char*)droy_malloc(len * 3 + 1);
    char* p = result;
    
    for (size_t i = 0; i < len; i++) {
        if (isalnum((unsigned char)str[i]) || str[i] == '-' || str[i] == '_' || 
            str[i] == '.' || str[i] == '~') {
            *p++ = str[i];
        } else {
            sprintf(p, "%%%02X", (unsigned char)str[i]);
            p += 3;
        }
    }
    *p = '\0';
    
    Value* val = value_create_string(result);
    droy_free(result);
    return val;
}

Value* builtin_decode_url(int argc, Value** args) {
    if (argc < 1 || args[0]->type != VAL_STRING) {
        return value_create_string("");
    }
    
    const char* str = args[0]->as.string;
    size_t len = strlen(str);
    char* result = (char*)droy_malloc(len + 1);
    char* p = result;
    
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '%' && i + 2 < len) {
            int val;
            sscanf(str + i + 1, "%2x", &val);
            *p++ = (char)val;
            i += 2;
        } else if (str[i] == '+') {
            *p++ = ' ';
        } else {
            *p++ = str[i];
        }
    }
    *p = '\0';
    
    Value* val = value_create_string(result);
    droy_free(result);
    return val;
}

Value* builtin_encode_base64(int argc, Value** args) {
    // TODO: Implement base64 encoding
    return value_create_string("");
}

Value* builtin_decode_base64(int argc, Value** args) {
    // TODO: Implement base64 decoding
    return value_create_string("");
}

/* Hash Functions */

Value* builtin_hash_md5(int argc, Value** args) {
    // TODO: Implement MD5 hashing
    return value_create_string("");
}

Value* builtin_hash_sha1(int argc, Value** args) {
    // TODO: Implement SHA1 hashing
    return value_create_string("");
}

Value* builtin_hash_sha256(int argc, Value** args) {
    // TODO: Implement SHA256 hashing
    return value_create_string("");
}

/* UUID Function */

Value* builtin_uuid(int argc, Value** args) {
    // Simple UUID v4 generation
    char uuid[37];
    const char* hex = "0123456789abcdef";
    
    for (int i = 0; i < 36; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            uuid[i] = '-';
        } else if (i == 14) {
            uuid[i] = '4';
        } else if (i == 19) {
            uuid[i] = hex[(rand() % 4) + 8];
        } else {
            uuid[i] = hex[rand() % 16];
        }
    }
    uuid[36] = '\0';
    
    return value_create_string(uuid);
}
