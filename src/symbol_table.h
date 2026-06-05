#ifndef MICROJAVA_SYMBOL_TABLE_H
#define MICROJAVA_SYMBOL_TABLE_H

#define SYMBOL_NAME_MAX 128
#define SYMBOL_TYPE_MAX 32
#define SYMBOL_TABLE_BUCKETS 211

typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_CONSTANT,
    SYMBOL_FUNCTION,
    SYMBOL_ARRAY,
    SYMBOL_CLASS
} SymbolKind;

typedef struct Symbol {
    char name[SYMBOL_NAME_MAX];
    SymbolKind kind;
    char type[SYMBOL_TYPE_MAX];
    int scope_level;
    int line;
    struct Symbol *next;
} Symbol;

typedef struct Scope {
    Symbol *buckets[SYMBOL_TABLE_BUCKETS];
    int level;
    struct Scope *parent;
} Scope;

typedef struct {
    Scope *current;
} SymbolTable;

void symbol_table_init(SymbolTable *table);
void symbol_table_free(SymbolTable *table);
void symbol_table_enter_scope(SymbolTable *table);
void symbol_table_exit_scope(SymbolTable *table);
Symbol *symbol_table_insert(SymbolTable *table, const char *name, SymbolKind kind, const char *type, int line);
Symbol *symbol_table_lookup(SymbolTable *table, const char *name);
Symbol *symbol_table_lookup_current(SymbolTable *table, const char *name);
int symbol_table_delete(SymbolTable *table, const char *name);
void symbol_table_dump(const SymbolTable *table);
const char *symbol_kind_name(SymbolKind kind);

#endif
