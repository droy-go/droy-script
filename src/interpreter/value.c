/**
 * Droy Value - Value System
 * =========================
 * Dynamic value types and operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "../../include/droy.h"

/* Value creation */

Value* value_create_number(double num) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_NUMBER;
    value->as.number = num;
    value->is_const = false;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

Value* value_create_string(const char* str) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_STRING;
    value->as.string = droy_strdup(str);
    value->is_const = false;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

Value* value_create_boolean(bool b) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_BOOLEAN;
    value->as.boolean = b;
    value->is_const = false;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

Value* value_create_null(void) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_NULL;
    value->is_const = false;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

Value* value_create_array(int capacity) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_ARRAY;
    value->as.array.capacity = capacity > 0 ? capacity : 8;
    value->as.array.count = 0;
    value->as.array.items = (Value**)droy_malloc(value->as.array.capacity * sizeof(Value*));
    value->is_const = false;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

Value* value_create_object(int capacity) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_OBJECT;
    value->as.object.capacity = capacity > 0 ? capacity : 8;
    value->as.object.count = 0;
    value->as.object.keys = (char**)droy_malloc(value->as.object.capacity * sizeof(char*));
    value->as.object.values = (Value**)droy_malloc(value->as.object.capacity * sizeof(Value*));
    value->is_const = false;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

Value* value_create_function(Function* fn) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_FUNCTION;
    value->as.function = fn;
    value->is_const = true;
    value->is_exported = false;
    value->doc = fn->doc;
    return value;
}

Value* value_create_native(const char* name, Value* (*fn)(int argc, Value** args)) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_NATIVE;
    value->as.native.name = droy_strdup(name);
    value->as.native.fn = fn;
    value->is_const = true;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

Value* value_create_link(Link* link) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_LINK;
    value->as.link = link;
    value->is_const = false;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

Value* value_create_package(Package* pkg) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_PACKAGE;
    value->as.package = pkg;
    value->is_const = false;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

Value* value_create_module(Module* mod) {
    Value* value = (Value*)droy_malloc(sizeof(Value));
    value->type = VAL_MODULE;
    value->as.module = mod;
    value->is_const = false;
    value->is_exported = false;
    value->doc = NULL;
    return value;
}

/* Value destruction */

void value_free(Value* value) {
    if (!value) return;
    
    switch (value->type) {
        case VAL_STRING:
            droy_free(value->as.string);
            break;
        case VAL_ARRAY:
            for (int i = 0; i < value->as.array.count; i++) {
                value_free(value->as.array.items[i]);
            }
            droy_free(value->as.array.items);
            break;
        case VAL_OBJECT:
            for (int i = 0; i < value->as.object.count; i++) {
                droy_free(value->as.object.keys[i]);
                value_free(value->as.object.values[i]);
            }
            droy_free(value->as.object.keys);
            droy_free(value->as.object.values);
            break;
        case VAL_FUNCTION:
            // Function is managed separately
            break;
        case VAL_NATIVE:
            droy_free(value->as.native.name);
            break;
        case VAL_LINK:
            // Link is managed separately
            break;
        case VAL_PACKAGE:
            // Package is managed separately
            break;
        case VAL_MODULE:
            // Module is managed separately
            break;
        default:
            break;
    }
    
    droy_free(value->doc);
    droy_free(value);
}

/* Value copying */

Value* value_copy(Value* value) {
    if (!value) return value_create_null();
    
    switch (value->type) {
        case VAL_NUMBER:
            return value_create_number(value->as.number);
        case VAL_STRING:
            return value_create_string(value->as.string);
        case VAL_BOOLEAN:
            return value_create_boolean(value->as.boolean);
        case VAL_NULL:
            return value_create_null();
        case VAL_ARRAY: {
            Value* copy = value_create_array(value->as.array.capacity);
            for (int i = 0; i < value->as.array.count; i++) {
                copy->as.array.items[copy->as.array.count++] = value_copy(value->as.array.items[i]);
            }
            return copy;
        }
        case VAL_OBJECT: {
            Value* copy = value_create_object(value->as.object.capacity);
            for (int i = 0; i < value->as.object.count; i++) {
                copy->as.object.keys[copy->as.object.count] = droy_strdup(value->as.object.keys[i]);
                copy->as.object.values[copy->as.object.count] = value_copy(value->as.object.values[i]);
                copy->as.object.count++;
            }
            return copy;
        }
        case VAL_FUNCTION:
        case VAL_NATIVE:
        case VAL_LINK:
        case VAL_PACKAGE:
        case VAL_MODULE:
            // Reference types - return same pointer
            return value;
        default:
            return value_create_null();
    }
}

/* Value to string */

char* value_to_string(Value* value) {
    if (!value) return droy_strdup("null");
    
    char buffer[1024];
    
    switch (value->type) {
        case VAL_NUMBER:
            if (value->as.number == (int)value->as.number) {
                snprintf(buffer, sizeof(buffer), "%.0f", value->as.number);
            } else {
                snprintf(buffer, sizeof(buffer), "%g", value->as.number);
            }
            return droy_strdup(buffer);
        case VAL_STRING:
            return droy_strdup(value->as.string);
        case VAL_BOOLEAN:
            return droy_strdup(value->as.boolean ? "true" : "false");
        case VAL_NULL:
            return droy_strdup("null");
        case VAL_ARRAY: {
            char* result = droy_strdup("[");
            for (int i = 0; i < value->as.array.count; i++) {
                char* item = value_to_string(value->as.array.items[i]);
                if (i > 0) {
                    char* temp = result;
                    result = (char*)droy_malloc(strlen(temp) + strlen(item) + 3);
                    sprintf(result, "%s, %s", temp, item);
                    droy_free(temp);
                } else {
                    droy_free(result);
                    result = droy_strdup("[");
                    result = (char*)droy_realloc(result, strlen(result) + strlen(item) + 1);
                    strcat(result, item);
                }
                droy_free(item);
            }
            char* temp = result;
            result = (char*)droy_malloc(strlen(temp) + 2);
            sprintf(result, "%s]", temp);
            droy_free(temp);
            return result;
        }
        case VAL_OBJECT: {
            char* result = droy_strdup("{");
            for (int i = 0; i < value->as.object.count; i++) {
                char* val = value_to_string(value->as.object.values[i]);
                int len = strlen(result) + strlen(value->as.object.keys[i]) + strlen(val) + 5;
                char* temp = result;
                result = (char*)droy_malloc(len);
                if (i > 0) {
                    sprintf(result, "%s, \"%s\": %s", temp, value->as.object.keys[i], val);
                } else {
                    sprintf(result, "{\"%s\": %s", value->as.object.keys[i], val);
                }
                droy_free(temp);
                droy_free(val);
            }
            char* temp = result;
            result = (char*)droy_malloc(strlen(temp) + 2);
            sprintf(result, "%s}", temp);
            droy_free(temp);
            return result;
        }
        case VAL_FUNCTION:
            return droy_strdup("[Function]");
        case VAL_NATIVE:
            return droy_strdup("[Native Function]");
        case VAL_LINK:
            return droy_strdup("[Link]");
        case VAL_PACKAGE:
            return droy_strdup("[Package]");
        case VAL_MODULE:
            return droy_strdup("[Module]");
        default:
            return droy_strdup("[Unknown]");
    }
}

/* Value predicates */

bool value_is_truthy(Value* value) {
    if (!value) return false;
    
    switch (value->type) {
        case VAL_NULL:
            return false;
        case VAL_BOOLEAN:
            return value->as.boolean;
        case VAL_NUMBER:
            return value->as.number != 0;
        case VAL_STRING:
            return strlen(value->as.string) > 0;
        case VAL_ARRAY:
            return value->as.array.count > 0;
        case VAL_OBJECT:
            return value->as.object.count > 0;
        default:
            return true;
    }
}

bool value_equals(Value* a, Value* b) {
    if (!a || !b) return a == b;
    if (a->type != b->type) return false;
    
    switch (a->type) {
        case VAL_NULL:
            return true;
        case VAL_NUMBER:
            return a->as.number == b->as.number;
        case VAL_STRING:
            return strcmp(a->as.string, b->as.string) == 0;
        case VAL_BOOLEAN:
            return a->as.boolean == b->as.boolean;
        default:
            return a == b;
    }
}

int value_compare(Value* a, Value* b) {
    if (!a || !b) return 0;
    if (a->type != b->type) return 0;
    
    switch (a->type) {
        case VAL_NUMBER:
            if (a->as.number < b->as.number) return -1;
            if (a->as.number > b->as.number) return 1;
            return 0;
        case VAL_STRING:
            return strcmp(a->as.string, b->as.string);
        default:
            return 0;
    }
}

/* Arithmetic operations */

Value* value_add(Value* a, Value* b) {
    if (!a || !b) return value_create_null();
    
    // String concatenation
    if (a->type == VAL_STRING || b->type == VAL_STRING) {
        char* str_a = value_to_string(a);
        char* str_b = value_to_string(b);
        int len = strlen(str_a) + strlen(str_b) + 1;
        char* result = (char*)droy_malloc(len);
        strcpy(result, str_a);
        strcat(result, str_b);
        droy_free(str_a);
        droy_free(str_b);
        Value* val = value_create_string(result);
        droy_free(result);
        return val;
    }
    
    // Number addition
    if (a->type == VAL_NUMBER && b->type == VAL_NUMBER) {
        return value_create_number(a->as.number + b->as.number);
    }
    
    // Array concatenation
    if (a->type == VAL_ARRAY && b->type == VAL_ARRAY) {
        Value* result = value_create_array(a->as.array.count + b->as.array.count);
        for (int i = 0; i < a->as.array.count; i++) {
            result->as.array.items[result->as.array.count++] = value_copy(a->as.array.items[i]);
        }
        for (int i = 0; i < b->as.array.count; i++) {
            result->as.array.items[result->as.array.count++] = value_copy(b->as.array.items[i]);
        }
        return result;
    }
    
    return value_create_null();
}

Value* value_subtract(Value* a, Value* b) {
    if (a->type == VAL_NUMBER && b->type == VAL_NUMBER) {
        return value_create_number(a->as.number - b->as.number);
    }
    return value_create_null();
}

Value* value_multiply(Value* a, Value* b) {
    if (a->type == VAL_NUMBER && b->type == VAL_NUMBER) {
        return value_create_number(a->as.number * b->as.number);
    }
    
    // String repetition
    if (a->type == VAL_STRING && b->type == VAL_NUMBER) {
        int count = (int)b->as.number;
        int len = strlen(a->as.string);
        char* result = (char*)droy_malloc(len * count + 1);
        result[0] = '\0';
        for (int i = 0; i < count; i++) {
            strcat(result, a->as.string);
        }
        Value* val = value_create_string(result);
        droy_free(result);
        return val;
    }
    
    return value_create_null();
}

Value* value_divide(Value* a, Value* b) {
    if (a->type == VAL_NUMBER && b->type == VAL_NUMBER) {
        if (b->as.number == 0) {
            fprintf(stderr, "Division by zero\n");
            return value_create_null();
        }
        return value_create_number(a->as.number / b->as.number);
    }
    return value_create_null();
}

Value* value_modulo(Value* a, Value* b) {
    if (a->type == VAL_NUMBER && b->type == VAL_NUMBER) {
        if (b->as.number == 0) {
            fprintf(stderr, "Modulo by zero\n");
            return value_create_null();
        }
        return value_create_number(fmod(a->as.number, b->as.number));
    }
    return value_create_null();
}

Value* value_power(Value* a, Value* b) {
    if (a->type == VAL_NUMBER && b->type == VAL_NUMBER) {
        return value_create_number(pow(a->as.number, b->as.number));
    }
    return value_create_null();
}
