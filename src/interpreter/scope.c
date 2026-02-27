/**
 * Droy Scope - Variable Scoping
 * ==============================
 * Variable scope management
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../include/droy.h"

Scope* scope_create(Scope* parent, const char* name) {
    Scope* scope = (Scope*)droy_calloc(1, sizeof(Scope));
    if (!scope) return NULL;
    
    scope->variables = NULL;
    scope->parent = parent;
    scope->children = NULL;
    scope->child_count = 0;
    scope->child_capacity = 0;
    scope->name = droy_strdup(name ? name : "anonymous");
    scope->is_module = false;
    scope->is_function = false;
    
    return scope;
}

void scope_destroy(Scope* scope) {
    if (!scope) return;
    
    // Free variables
    Variable* current = scope->variables;
    while (current) {
        Variable* next = current->next;
        droy_free(current->name);
        value_free(current->value);
        droy_free(current->doc);
        droy_free(current);
        current = next;
    }
    
    // Free children
    for (int i = 0; i < scope->child_count; i++) {
        scope_destroy(scope->children[i]);
    }
    droy_free(scope->children);
    
    droy_free(scope->name);
    droy_free(scope);
}

Variable* scope_define(Scope* scope, const char* name, Value* value, bool is_const) {
    if (!scope || !name) return NULL;
    
    // Check if already defined in current scope
    Variable* existing = scope_get(scope, name);
    if (existing && existing->is_const) {
        fprintf(stderr, "Cannot reassign constant: %s\n", name);
        return NULL;
    }
    
    // Create new variable
    Variable* var = (Variable*)droy_malloc(sizeof(Variable));
    var->name = droy_strdup(name);
    var->value = value_copy(value);
    var->is_const = is_const;
    var->is_exported = false;
    var->doc = NULL;
    var->next = NULL;
    
    // Add to scope
    if (!scope->variables) {
        scope->variables = var;
    } else {
        // Check if variable already exists
        Variable* current = scope->variables;
        Variable* prev = NULL;
        while (current) {
            if (strcmp(current->name, name) == 0) {
                // Replace existing variable
                value_free(current->value);
                current->value = value_copy(value);
                current->is_const = is_const;
                droy_free(var->name);
                droy_free(var);
                return current;
            }
            prev = current;
            current = current->next;
        }
        // Add to end
        prev->next = var;
    }
    
    return var;
}

Variable* scope_get(Scope* scope, const char* name) {
    if (!scope || !name) return NULL;
    
    // Search in current scope
    Variable* current = scope->variables;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    // Search in parent scope
    if (scope->parent) {
        return scope_get(scope->parent, name);
    }
    
    return NULL;
}

Value* scope_get_value(Scope* scope, const char* name) {
    Variable* var = scope_get(scope, name);
    return var ? var->value : NULL;
}

bool scope_set(Scope* scope, const char* name, Value* value) {
    if (!scope || !name) return false;
    
    // Search in current scope
    Variable* current = scope->variables;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            if (current->is_const) {
                fprintf(stderr, "Cannot reassign constant: %s\n", name);
                return false;
            }
            value_free(current->value);
            current->value = value_copy(value);
            return true;
        }
        current = current->next;
    }
    
    // Search in parent scope
    if (scope->parent) {
        return scope_set(scope->parent, name, value);
    }
    
    return false;
}

bool scope_has(Scope* scope, const char* name) {
    return scope_get(scope, name) != NULL;
}

bool scope_has_local(Scope* scope, const char* name) {
    if (!scope || !name) return false;
    
    Variable* current = scope->variables;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return true;
        }
        current = current->next;
    }
    
    return false;
}

void scope_push(Interpreter* interp, Scope* scope) {
    if (!interp || !scope) return;
    
    // Add as child of current scope
    if (interp->current_scope) {
        if (interp->current_scope->child_count >= interp->current_scope->child_capacity) {
            interp->current_scope->child_capacity = interp->current_scope->child_capacity ? 
                interp->current_scope->child_capacity * 2 : 4;
            interp->current_scope->children = (Scope**)droy_realloc(
                interp->current_scope->children,
                interp->current_scope->child_capacity * sizeof(Scope*));
        }
        interp->current_scope->children[interp->current_scope->child_count++] = scope;
    }
    
    interp->current_scope = scope;
}

void scope_pop(Interpreter* interp) {
    if (!interp || !interp->current_scope) return;
    
    interp->current_scope = interp->current_scope->parent;
}

void scope_export(Scope* scope, const char* name) {
    Variable* var = scope_get(scope, name);
    if (var) {
        var->is_exported = true;
    }
}

Value** scope_get_exports(Scope* scope, int* count) {
    if (!scope || !count) return NULL;
    
    // Count exports
    *count = 0;
    Variable* current = scope->variables;
    while (current) {
        if (current->is_exported) {
            (*count)++;
        }
        current = current->next;
    }
    
    if (*count == 0) return NULL;
    
    // Collect exports
    Value** exports = (Value**)droy_malloc(*count * sizeof(Value*));
    int i = 0;
    current = scope->variables;
    while (current) {
        if (current->is_exported) {
            exports[i++] = value_copy(current->value);
        }
        current = current->next;
    }
    
    return exports;
}

void scope_print(Scope* scope, int indent) {
    if (!scope) return;
    
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("Scope: %s\n", scope->name);
    
    Variable* current = scope->variables;
    while (current) {
        for (int i = 0; i < indent + 1; i++) {
            printf("  ");
        }
        char* val_str = value_to_string(current->value);
        printf("  %s%s = %s\n", 
            current->is_const ? "const " : "",
            current->name, 
            val_str);
        droy_free(val_str);
        current = current->next;
    }
    
    for (int i = 0; i < scope->child_count; i++) {
        scope_print(scope->children[i], indent + 1);
    }
}
