#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_handler.h"
#include "lexer.h"
#include "symbol_table.h"

typedef struct {
    DoubleBufferReader reader;
    Token current;
    int errors;
    int dump_symbols;
    ErrorList error_list;
    SymbolTable symbols;
} Parser;

static void advance(Parser *parser);
static void parse_program(Parser *parser);
static void parse_member_list(Parser *parser);
static void parse_member(Parser *parser);
static void parse_member_after(Parser *parser, Token name, const char *type);
static void parse_type(Parser *parser, char *type);
static void parse_param_list_opt(Parser *parser);
static void parse_param_list(Parser *parser);
static void parse_param_list_tail(Parser *parser);
static void parse_block(Parser *parser);
static void parse_stmt_list(Parser *parser);
static void parse_stmt(Parser *parser);
static int parse_var_decl_after(Parser *parser);
static void parse_stmt_id_tail(Parser *parser);
static void parse_assign_target(Parser *parser);
static void parse_expr_opt(Parser *parser);
static void parse_arg_list_opt(Parser *parser);
static void parse_arg_list(Parser *parser);
static void parse_arg_list_tail(Parser *parser);
static void parse_expr(Parser *parser);
static void parse_rel_expr(Parser *parser);
static void parse_rel_expr_tail(Parser *parser);
static void parse_rel_op(Parser *parser);
static void parse_add_expr(Parser *parser);
static void parse_add_expr_tail(Parser *parser);
static void parse_add_op(Parser *parser);
static void parse_term(Parser *parser);
static void parse_term_tail(Parser *parser);
static void parse_mul_op(Parser *parser);
static void parse_factor(Parser *parser);
static void parse_factor_after(Parser *parser);

static int lexeme_is(Parser *parser, const char *lexeme) {
    return strcmp(parser->current.lexeme, lexeme) == 0;
}

static int is_type_start(Parser *parser) {
    return parser->current.type == TOKEN_KEYWORD &&
           (lexeme_is(parser, "int") || lexeme_is(parser, "char") ||
            lexeme_is(parser, "boolean") || lexeme_is(parser, "void"));
}

static int is_expr_start(Parser *parser) {
    return parser->current.type == TOKEN_IDENTIFIER ||
           parser->current.type == TOKEN_NUMBER ||
           parser->current.type == TOKEN_CHAR_CONST ||
           lexeme_is(parser, "true") || lexeme_is(parser, "false") ||
           lexeme_is(parser, "(");
}

static int is_stmt_start(Parser *parser) {
    return is_type_start(parser) || parser->current.type == TOKEN_IDENTIFIER ||
           lexeme_is(parser, "return") || lexeme_is(parser, "if") ||
           lexeme_is(parser, "while") || lexeme_is(parser, "{") ||
           lexeme_is(parser, ";") || lexeme_is(parser, "print");
}

static int is_rel_op(Parser *parser) {
    return lexeme_is(parser, "==") || lexeme_is(parser, "!=") ||
           lexeme_is(parser, "<") || lexeme_is(parser, "<=") ||
           lexeme_is(parser, ">") || lexeme_is(parser, ">=");
}

static int is_add_op(Parser *parser) {
    return lexeme_is(parser, "+") || lexeme_is(parser, "-");
}

static int is_mul_op(Parser *parser) {
    return lexeme_is(parser, "*") || lexeme_is(parser, "/") || lexeme_is(parser, "%");
}

static void syntax_error(Parser *parser, const char *expected) {
    error_report(&parser->error_list, ERROR_SYNTACTIC,
                 parser->current.line, parser->current.column,
                 "expected %s, found %s '%s'", expected,
                 token_type_name(parser->current.type), parser->current.lexeme);
    parser->errors++;
}

static void semantic_error(Parser *parser, Token token, const char *message) {
    error_report(&parser->error_list, ERROR_SEMANTIC, token.line, token.column, "%s '%s'",
                 message, token.lexeme);
    parser->errors++;
}

static void declare_symbol(Parser *parser, Token token, SymbolKind kind, const char *type) {
    if (symbol_table_insert(&parser->symbols, token.lexeme, kind, type, token.line) == NULL) {
        Symbol *existing = symbol_table_lookup_current(&parser->symbols, token.lexeme);
        if (existing != NULL) {
            error_report(&parser->error_list, ERROR_SEMANTIC, token.line, token.column,
                         "redeclaration of '%s' in scope %d; first declared at line %d",
                         token.lexeme, existing->scope_level, existing->line);
            parser->errors++;
        }
    }
}

static Symbol *use_identifier(Parser *parser, Token token) {
    Symbol *symbol = symbol_table_lookup(&parser->symbols, token.lexeme);
    if (symbol == NULL) {
        semantic_error(parser, token, "undeclared identifier");
    }
    return symbol;
}

static void advance(Parser *parser) {
    parser->current = next_token(&parser->reader);
    if (parser->current.type == TOKEN_ERROR) {
        error_report(&parser->error_list, ERROR_LEXICAL,
                     parser->current.line, parser->current.column,
                     "%s '%s'", parser->current.attribute, parser->current.lexeme);
        parser->errors++;
    }
}

static void expect_lexeme(Parser *parser, const char *lexeme) {
    if (lexeme_is(parser, lexeme)) {
        advance(parser);
        return;
    }
    syntax_error(parser, lexeme);
    if (parser->current.type != TOKEN_EOF) {
        advance(parser);
    }
}

static void expect_token(Parser *parser, TokenType type, const char *name) {
    if (parser->current.type == type) {
        advance(parser);
        return;
    }
    syntax_error(parser, name);
    if (parser->current.type != TOKEN_EOF) {
        advance(parser);
    }
}

static void parse_program(Parser *parser) {
    Token class_name;
    expect_lexeme(parser, "class");
    class_name = parser->current;
    expect_token(parser, TOKEN_IDENTIFIER, "identifier");
    if (class_name.type == TOKEN_IDENTIFIER) {
        declare_symbol(parser, class_name, SYMBOL_CLASS, "class");
    }
    expect_lexeme(parser, "{");
    parse_member_list(parser);
    expect_lexeme(parser, "}");
}

static void parse_member_list(Parser *parser) {
    while (is_type_start(parser)) {
        parse_member(parser);
    }
}

static void parse_member(Parser *parser) {
    char type[SYMBOL_TYPE_MAX];
    Token name;
    parse_type(parser, type);
    name = parser->current;
    expect_token(parser, TOKEN_IDENTIFIER, "identifier");
    if (name.type == TOKEN_IDENTIFIER) {
        parse_member_after(parser, name, type);
    } else {
        parse_member_after(parser, name, type);
    }
}

static void parse_member_after(Parser *parser, Token name, const char *type) {
    if (lexeme_is(parser, ";")) {
        if (name.type == TOKEN_IDENTIFIER) {
            declare_symbol(parser, name, SYMBOL_VARIABLE, type);
        }
        expect_lexeme(parser, ";");
    } else if (lexeme_is(parser, "[")) {
        if (name.type == TOKEN_IDENTIFIER) {
            declare_symbol(parser, name, SYMBOL_ARRAY, type);
        }
        expect_lexeme(parser, "[");
        expect_token(parser, TOKEN_NUMBER, "number");
        expect_lexeme(parser, "]");
        expect_lexeme(parser, ";");
    } else if (lexeme_is(parser, "(")) {
        if (name.type == TOKEN_IDENTIFIER) {
            declare_symbol(parser, name, SYMBOL_FUNCTION, type);
        }
        expect_lexeme(parser, "(");
        symbol_table_enter_scope(&parser->symbols);
        parse_param_list_opt(parser);
        expect_lexeme(parser, ")");
        parse_block(parser);
        symbol_table_exit_scope(&parser->symbols);
    } else {
        syntax_error(parser, "`;`, `[`, or `(`");
    }
}

static void parse_type(Parser *parser, char *type) {
    if (is_type_start(parser)) {
        strncpy(type, parser->current.lexeme, SYMBOL_TYPE_MAX - 1);
        type[SYMBOL_TYPE_MAX - 1] = '\0';
        advance(parser);
    } else {
        strncpy(type, "unknown", SYMBOL_TYPE_MAX - 1);
        type[SYMBOL_TYPE_MAX - 1] = '\0';
        syntax_error(parser, "type");
        if (parser->current.type != TOKEN_EOF) {
            advance(parser);
        }
    }
}

static void parse_param_list_opt(Parser *parser) {
    if (is_type_start(parser)) {
        parse_param_list(parser);
    }
}

static void parse_param_list(Parser *parser) {
    char type[SYMBOL_TYPE_MAX];
    Token name;
    parse_type(parser, type);
    name = parser->current;
    expect_token(parser, TOKEN_IDENTIFIER, "identifier");
    if (name.type == TOKEN_IDENTIFIER) {
        declare_symbol(parser, name, SYMBOL_VARIABLE, type);
    }
    parse_param_list_tail(parser);
}

static void parse_param_list_tail(Parser *parser) {
    while (lexeme_is(parser, ",")) {
        char type[SYMBOL_TYPE_MAX];
        Token name;
        expect_lexeme(parser, ",");
        parse_type(parser, type);
        name = parser->current;
        expect_token(parser, TOKEN_IDENTIFIER, "identifier");
        if (name.type == TOKEN_IDENTIFIER) {
            declare_symbol(parser, name, SYMBOL_VARIABLE, type);
        }
    }
}

static void parse_block(Parser *parser) {
    expect_lexeme(parser, "{");
    symbol_table_enter_scope(&parser->symbols);
    parse_stmt_list(parser);
    expect_lexeme(parser, "}");
    symbol_table_exit_scope(&parser->symbols);
}

static void parse_stmt_list(Parser *parser) {
    while (is_stmt_start(parser)) {
        parse_stmt(parser);
    }
}

static void parse_stmt(Parser *parser) {
    if (is_type_start(parser)) {
        char type[SYMBOL_TYPE_MAX];
        Token name;
        int is_array;
        parse_type(parser, type);
        name = parser->current;
        expect_token(parser, TOKEN_IDENTIFIER, "identifier");
        is_array = parse_var_decl_after(parser);
        if (name.type == TOKEN_IDENTIFIER) {
            declare_symbol(parser, name, is_array ? SYMBOL_ARRAY : SYMBOL_VARIABLE, type);
        }
        expect_lexeme(parser, ";");
    } else if (parser->current.type == TOKEN_IDENTIFIER) {
        Token name = parser->current;
        use_identifier(parser, name);
        expect_token(parser, TOKEN_IDENTIFIER, "identifier");
        parse_stmt_id_tail(parser);
        expect_lexeme(parser, ";");
    } else if (lexeme_is(parser, "return")) {
        expect_lexeme(parser, "return");
        parse_expr_opt(parser);
        expect_lexeme(parser, ";");
    } else if (lexeme_is(parser, "if")) {
        expect_lexeme(parser, "if");
        expect_lexeme(parser, "(");
        parse_expr(parser);
        expect_lexeme(parser, ")");
        parse_stmt(parser);
    } else if (lexeme_is(parser, "while")) {
        expect_lexeme(parser, "while");
        expect_lexeme(parser, "(");
        parse_expr(parser);
        expect_lexeme(parser, ")");
        parse_stmt(parser);
    } else if (lexeme_is(parser, "{")) {
        parse_block(parser);
    } else if (lexeme_is(parser, ";")) {
        expect_lexeme(parser, ";");
    } else if (lexeme_is(parser, "print")) {
        expect_lexeme(parser, "print");
        expect_lexeme(parser, "(");
        parse_expr(parser);
        expect_lexeme(parser, ")");
        expect_lexeme(parser, ";");
    } else {
        syntax_error(parser, "statement");
        if (parser->current.type != TOKEN_EOF) {
            advance(parser);
        }
    }
}

static int parse_var_decl_after(Parser *parser) {
    if (lexeme_is(parser, "[")) {
        expect_lexeme(parser, "[");
        expect_token(parser, TOKEN_NUMBER, "number");
        expect_lexeme(parser, "]");
        return 1;
    }
    return 0;
}

static void parse_stmt_id_tail(Parser *parser) {
    if (lexeme_is(parser, "(")) {
        expect_lexeme(parser, "(");
        parse_arg_list_opt(parser);
        expect_lexeme(parser, ")");
    } else {
        parse_assign_target(parser);
        expect_lexeme(parser, "=");
        parse_expr(parser);
    }
}

static void parse_assign_target(Parser *parser) {
    if (lexeme_is(parser, "[")) {
        expect_lexeme(parser, "[");
        parse_expr(parser);
        expect_lexeme(parser, "]");
    }
}

static void parse_expr_opt(Parser *parser) {
    if (is_expr_start(parser)) {
        parse_expr(parser);
    }
}

static void parse_arg_list_opt(Parser *parser) {
    if (is_expr_start(parser)) {
        parse_arg_list(parser);
    }
}

static void parse_arg_list(Parser *parser) {
    parse_expr(parser);
    parse_arg_list_tail(parser);
}

static void parse_arg_list_tail(Parser *parser) {
    while (lexeme_is(parser, ",")) {
        expect_lexeme(parser, ",");
        parse_expr(parser);
    }
}

static void parse_expr(Parser *parser) {
    parse_rel_expr(parser);
}

static void parse_rel_expr(Parser *parser) {
    parse_add_expr(parser);
    parse_rel_expr_tail(parser);
}

static void parse_rel_expr_tail(Parser *parser) {
    if (is_rel_op(parser)) {
        parse_rel_op(parser);
        parse_add_expr(parser);
    }
}

static void parse_rel_op(Parser *parser) {
    if (is_rel_op(parser)) {
        advance(parser);
    } else {
        syntax_error(parser, "relational operator");
    }
}

static void parse_add_expr(Parser *parser) {
    parse_term(parser);
    parse_add_expr_tail(parser);
}

static void parse_add_expr_tail(Parser *parser) {
    while (is_add_op(parser)) {
        parse_add_op(parser);
        parse_term(parser);
    }
}

static void parse_add_op(Parser *parser) {
    if (is_add_op(parser)) {
        advance(parser);
    } else {
        syntax_error(parser, "add operator");
    }
}

static void parse_term(Parser *parser) {
    parse_factor(parser);
    parse_term_tail(parser);
}

static void parse_term_tail(Parser *parser) {
    while (is_mul_op(parser)) {
        parse_mul_op(parser);
        parse_factor(parser);
    }
}

static void parse_mul_op(Parser *parser) {
    if (is_mul_op(parser)) {
        advance(parser);
    } else {
        syntax_error(parser, "multiply operator");
    }
}

static void parse_factor(Parser *parser) {
    if (parser->current.type == TOKEN_IDENTIFIER) {
        Token name = parser->current;
        use_identifier(parser, name);
        expect_token(parser, TOKEN_IDENTIFIER, "identifier");
        parse_factor_after(parser);
    } else if (parser->current.type == TOKEN_NUMBER) {
        expect_token(parser, TOKEN_NUMBER, "number");
    } else if (parser->current.type == TOKEN_CHAR_CONST) {
        expect_token(parser, TOKEN_CHAR_CONST, "character constant");
    } else if (lexeme_is(parser, "true") || lexeme_is(parser, "false")) {
        advance(parser);
    } else if (lexeme_is(parser, "(")) {
        expect_lexeme(parser, "(");
        parse_expr(parser);
        expect_lexeme(parser, ")");
    } else {
        syntax_error(parser, "factor");
        if (parser->current.type != TOKEN_EOF) {
            advance(parser);
        }
    }
}

static void parse_factor_after(Parser *parser) {
    if (lexeme_is(parser, "(")) {
        expect_lexeme(parser, "(");
        parse_arg_list_opt(parser);
        expect_lexeme(parser, ")");
    } else if (lexeme_is(parser, "[")) {
        expect_lexeme(parser, "[");
        parse_expr(parser);
        expect_lexeme(parser, "]");
    }
}

int main(int argc, char **argv) {
    FILE *file;
    Parser parser;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <source.mj> [--dump-symbols]\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Cannot open source file: %s\n", argv[1]);
        return 1;
    }

    parser.errors = 0;
    parser.dump_symbols = argc == 3 && strcmp(argv[2], "--dump-symbols") == 0;
    error_list_init(&parser.error_list);
    symbol_table_init(&parser.symbols);
    init_reader(&parser.reader, file);
    advance(&parser);
    parse_program(&parser);
    if (parser.current.type != TOKEN_EOF) {
        syntax_error(&parser, "end of file");
    }

    fclose(file);
    if (parser.dump_symbols) {
        symbol_table_dump(&parser.symbols);
    }
    error_print_summary(&parser.error_list);
    symbol_table_free(&parser.symbols);
    if (parser.errors == 0) {
        printf("Recursive descent parser: ACCEPT\n");
        return 0;
    }
    printf("Recursive descent parser: REJECT (%d error(s))\n", parser.errors);
    return 1;
}
