#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"

static unsigned hash_name(const char *name) {
    unsigned hash = 5381;
    while (*name != '\0') {
        hash = ((hash << 5) + hash) + (unsigned char)*name;
        name++;
    }
    return hash % SYMBOL_TABLE_BUCKETS;
}

static Scope *scope_create(int level, Scope *parent) {
    Scope *scope = calloc(1, sizeof(Scope));
    if (scope == NULL) {
        fprintf(stderr, "Out of memory while creating scope\n");
        exit(1);
    }
    scope->level = level;
    scope->parent = parent;
    return scope;
}

static void scope_free(Scope *scope) {
    int i;
    if (scope == NULL) {
        return;
    }
    for (i = 0; i < SYMBOL_TABLE_BUCKETS; i++) {
        Symbol *symbol = scope->buckets[i];
        while (symbol != NULL) {
            Symbol *next = symbol->next;
            free(symbol);
            symbol = next;
        }
    }
    free(scope);
}

void symbol_table_init(SymbolTable *table) {
    table->current = NULL;
    symbol_table_enter_scope(table);
}

void symbol_table_free(SymbolTable *table) {
    while (table->current != NULL) {
        symbol_table_exit_scope(table);
    }
}

void symbol_table_enter_scope(SymbolTable *table) {
    int level = table->current == NULL ? 0 : table->current->level + 1;
    table->current = scope_create(level, table->current);
}

void symbol_table_exit_scope(SymbolTable *table) {
    Scope *old_scope;
    if (table->current == NULL) {
        return;
    }
    old_scope = table->current;
    table->current = old_scope->parent;
    scope_free(old_scope);
}

const char *symbol_kind_name(SymbolKind kind) {
    switch (kind) {
        case SYMBOL_VARIABLE: return "variable";
        case SYMBOL_CONSTANT: return "constant";
        case SYMBOL_FUNCTION: return "function";
        case SYMBOL_ARRAY: return "array";
        case SYMBOL_CLASS: return "class";
        default: return "unknown";
    }
}

Symbol *symbol_table_lookup_current(SymbolTable *table, const char *name) {
    unsigned bucket;
    Symbol *symbol;

    if (table->current == NULL) {
        return NULL;
    }

    bucket = hash_name(name);
    symbol = table->current->buckets[bucket];
    while (symbol != NULL) {
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }
    return NULL;
}

Symbol *symbol_table_lookup(SymbolTable *table, const char *name) {
    Scope *scope;
    for (scope = table->current; scope != NULL; scope = scope->parent) {
        unsigned bucket = hash_name(name);
        Symbol *symbol = scope->buckets[bucket];
        while (symbol != NULL) {
            if (strcmp(symbol->name, name) == 0) {
                return symbol;
            }
            symbol = symbol->next;
        }
    }
    return NULL;
}

Symbol *symbol_table_insert(SymbolTable *table, const char *name, SymbolKind kind, const char *type, int line) {
    unsigned bucket;
    Symbol *symbol;

    if (table->current == NULL || symbol_table_lookup_current(table, name) != NULL) {
        return NULL;
    }

    bucket = hash_name(name);
    symbol = calloc(1, sizeof(Symbol));
    if (symbol == NULL) {
        fprintf(stderr, "Out of memory while inserting symbol\n");
        exit(1);
    }

    strncpy(symbol->name, name, SYMBOL_NAME_MAX - 1);
    strncpy(symbol->type, type, SYMBOL_TYPE_MAX - 1);
    symbol->kind = kind;
    symbol->scope_level = table->current->level;
    symbol->line = line;
    symbol->next = table->current->buckets[bucket];
    table->current->buckets[bucket] = symbol;
    return symbol;
}

int symbol_table_delete(SymbolTable *table, const char *name) {
    unsigned bucket;
    Symbol *symbol;
    Symbol *previous = NULL;

    if (table->current == NULL) {
        return 0;
    }

    bucket = hash_name(name);
    symbol = table->current->buckets[bucket];
    while (symbol != NULL) {
        if (strcmp(symbol->name, name) == 0) {
            if (previous == NULL) {
                table->current->buckets[bucket] = symbol->next;
            } else {
                previous->next = symbol->next;
            }
            free(symbol);
            return 1;
        }
        previous = symbol;
        symbol = symbol->next;
    }
    return 0;
}

void symbol_table_dump(const SymbolTable *table) {
    const Scope *scope;
    int i;

    printf("Symbol table dump\n");
    printf("%-20s %-10s %-10s %-6s %-6s\n", "Name", "Kind", "Type", "Scope", "Line");
    for (scope = table->current; scope != NULL; scope = scope->parent) {
        for (i = 0; i < SYMBOL_TABLE_BUCKETS; i++) {
            const Symbol *symbol = scope->buckets[i];
            while (symbol != NULL) {
                printf("%-20s %-10s %-10s %-6d %-6d\n",
                       symbol->name, symbol_kind_name(symbol->kind), symbol->type,
                       symbol->scope_level, symbol->line);
                symbol = symbol->next;
            }
        }
    }
}
