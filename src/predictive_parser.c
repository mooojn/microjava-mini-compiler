#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_handler.h"
#include "lexer.h"

#define STACK_MAX 4096
#define RHS_MAX 8

typedef enum {
    T_CLASS,
    T_ID,
    T_LBRACE,
    T_RBRACE,
    T_SEMI,
    T_LBRACKET,
    T_NUMBER,
    T_RBRACKET,
    T_LPAREN,
    T_RPAREN,
    T_INT,
    T_CHAR,
    T_BOOLEAN,
    T_VOID,
    T_COMMA,
    T_RETURN,
    T_IF,
    T_WHILE,
    T_PRINT,
    T_ASSIGN,
    T_EQ,
    T_NE,
    T_LT,
    T_LE,
    T_GT,
    T_GE,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_PERCENT,
    T_CHAR_CONST,
    T_TRUE,
    T_FALSE,
    T_EOF,
    N_PROGRAM,
    N_MEMBER_LIST,
    N_MEMBER,
    N_MEMBER_AFTER,
    N_TYPE,
    N_PARAM_LIST_OPT,
    N_PARAM_LIST,
    N_PARAM_LIST_TAIL,
    N_BLOCK,
    N_STMT_LIST,
    N_STMT,
    N_VAR_DECL_AFTER,
    N_STMT_ID_TAIL,
    N_ASSIGN_TARGET,
    N_EXPR_OPT,
    N_ARG_LIST_OPT,
    N_ARG_LIST,
    N_ARG_LIST_TAIL,
    N_EXPR,
    N_REL_EXPR,
    N_REL_EXPR_TAIL,
    N_REL_OP,
    N_ADD_EXPR,
    N_ADD_EXPR_TAIL,
    N_ADD_OP,
    N_TERM,
    N_TERM_TAIL,
    N_MUL_OP,
    N_FACTOR,
    N_FACTOR_AFTER,
    SYM_INVALID
} Symbol;

typedef struct {
    Symbol symbols[RHS_MAX];
    int count;
} Production;

static const char *symbol_name(Symbol symbol) {
    switch (symbol) {
        case T_CLASS: return "class";
        case T_ID: return "id";
        case T_LBRACE: return "{";
        case T_RBRACE: return "}";
        case T_SEMI: return ";";
        case T_LBRACKET: return "[";
        case T_NUMBER: return "number";
        case T_RBRACKET: return "]";
        case T_LPAREN: return "(";
        case T_RPAREN: return ")";
        case T_INT: return "int";
        case T_CHAR: return "char";
        case T_BOOLEAN: return "boolean";
        case T_VOID: return "void";
        case T_COMMA: return ",";
        case T_RETURN: return "return";
        case T_IF: return "if";
        case T_WHILE: return "while";
        case T_PRINT: return "print";
        case T_ASSIGN: return "=";
        case T_EQ: return "==";
        case T_NE: return "!=";
        case T_LT: return "<";
        case T_LE: return "<=";
        case T_GT: return ">";
        case T_GE: return ">=";
        case T_PLUS: return "+";
        case T_MINUS: return "-";
        case T_STAR: return "*";
        case T_SLASH: return "/";
        case T_PERCENT: return "%";
        case T_CHAR_CONST: return "char_const";
        case T_TRUE: return "true";
        case T_FALSE: return "false";
        case T_EOF: return "$";
        case N_PROGRAM: return "Program";
        case N_MEMBER_LIST: return "MemberList";
        case N_MEMBER: return "Member";
        case N_MEMBER_AFTER: return "MemberAfter";
        case N_TYPE: return "Type";
        case N_PARAM_LIST_OPT: return "ParamListOpt";
        case N_PARAM_LIST: return "ParamList";
        case N_PARAM_LIST_TAIL: return "ParamListTail";
        case N_BLOCK: return "Block";
        case N_STMT_LIST: return "StmtList";
        case N_STMT: return "Stmt";
        case N_VAR_DECL_AFTER: return "VarDeclAfter";
        case N_STMT_ID_TAIL: return "StmtIdTail";
        case N_ASSIGN_TARGET: return "AssignTarget";
        case N_EXPR_OPT: return "ExprOpt";
        case N_ARG_LIST_OPT: return "ArgListOpt";
        case N_ARG_LIST: return "ArgList";
        case N_ARG_LIST_TAIL: return "ArgListTail";
        case N_EXPR: return "Expr";
        case N_REL_EXPR: return "RelExpr";
        case N_REL_EXPR_TAIL: return "RelExprTail";
        case N_REL_OP: return "RelOp";
        case N_ADD_EXPR: return "AddExpr";
        case N_ADD_EXPR_TAIL: return "AddExprTail";
        case N_ADD_OP: return "AddOp";
        case N_TERM: return "Term";
        case N_TERM_TAIL: return "TermTail";
        case N_MUL_OP: return "MulOp";
        case N_FACTOR: return "Factor";
        case N_FACTOR_AFTER: return "FactorAfter";
        default: return "invalid";
    }
}

static int is_terminal(Symbol symbol) {
    return symbol >= T_CLASS && symbol <= T_EOF;
}

static int lexeme_equals(Token token, const char *lexeme) {
    return strcmp(token.lexeme, lexeme) == 0;
}

static Symbol lookahead_symbol(Token token) {
    if (token.type == TOKEN_IDENTIFIER) return T_ID;
    if (token.type == TOKEN_NUMBER) return T_NUMBER;
    if (token.type == TOKEN_CHAR_CONST) return T_CHAR_CONST;
    if (token.type == TOKEN_EOF) return T_EOF;
    if (lexeme_equals(token, "class")) return T_CLASS;
    if (lexeme_equals(token, "{")) return T_LBRACE;
    if (lexeme_equals(token, "}")) return T_RBRACE;
    if (lexeme_equals(token, ";")) return T_SEMI;
    if (lexeme_equals(token, "[")) return T_LBRACKET;
    if (lexeme_equals(token, "]")) return T_RBRACKET;
    if (lexeme_equals(token, "(")) return T_LPAREN;
    if (lexeme_equals(token, ")")) return T_RPAREN;
    if (lexeme_equals(token, "int")) return T_INT;
    if (lexeme_equals(token, "char")) return T_CHAR;
    if (lexeme_equals(token, "boolean")) return T_BOOLEAN;
    if (lexeme_equals(token, "void")) return T_VOID;
    if (lexeme_equals(token, ",")) return T_COMMA;
    if (lexeme_equals(token, "return")) return T_RETURN;
    if (lexeme_equals(token, "if")) return T_IF;
    if (lexeme_equals(token, "while")) return T_WHILE;
    if (lexeme_equals(token, "print")) return T_PRINT;
    if (lexeme_equals(token, "=")) return T_ASSIGN;
    if (lexeme_equals(token, "==")) return T_EQ;
    if (lexeme_equals(token, "!=")) return T_NE;
    if (lexeme_equals(token, "<")) return T_LT;
    if (lexeme_equals(token, "<=")) return T_LE;
    if (lexeme_equals(token, ">")) return T_GT;
    if (lexeme_equals(token, ">=")) return T_GE;
    if (lexeme_equals(token, "+")) return T_PLUS;
    if (lexeme_equals(token, "-")) return T_MINUS;
    if (lexeme_equals(token, "*")) return T_STAR;
    if (lexeme_equals(token, "/")) return T_SLASH;
    if (lexeme_equals(token, "%")) return T_PERCENT;
    if (lexeme_equals(token, "true")) return T_TRUE;
    if (lexeme_equals(token, "false")) return T_FALSE;
    return SYM_INVALID;
}

static int is_type_start(Symbol lookahead) {
    return lookahead == T_INT || lookahead == T_CHAR ||
           lookahead == T_BOOLEAN || lookahead == T_VOID;
}

static int is_stmt_start(Symbol lookahead) {
    return is_type_start(lookahead) || lookahead == T_ID ||
           lookahead == T_RETURN || lookahead == T_IF ||
           lookahead == T_WHILE || lookahead == T_LBRACE ||
           lookahead == T_SEMI || lookahead == T_PRINT;
}

static int is_expr_start(Symbol lookahead) {
    return lookahead == T_ID || lookahead == T_NUMBER ||
           lookahead == T_CHAR_CONST || lookahead == T_TRUE ||
           lookahead == T_FALSE || lookahead == T_LPAREN;
}

static int is_rel_op(Symbol lookahead) {
    return lookahead == T_EQ || lookahead == T_NE || lookahead == T_LT ||
           lookahead == T_LE || lookahead == T_GT || lookahead == T_GE;
}

static int is_add_op(Symbol lookahead) {
    return lookahead == T_PLUS || lookahead == T_MINUS;
}

static int is_mul_op(Symbol lookahead) {
    return lookahead == T_STAR || lookahead == T_SLASH || lookahead == T_PERCENT;
}

static int is_expr_follow(Symbol lookahead) {
    return lookahead == T_RPAREN || lookahead == T_RBRACKET ||
           lookahead == T_SEMI || lookahead == T_COMMA;
}

static int is_sync_symbol(Symbol nonterminal, Symbol lookahead) {
    if (lookahead == T_EOF) return 1;
    switch (nonterminal) {
        case N_MEMBER_LIST:
        case N_BLOCK:
        case N_STMT_LIST:
            return lookahead == T_RBRACE;
        case N_MEMBER:
        case N_STMT:
            return lookahead == T_SEMI || lookahead == T_RBRACE;
        case N_MEMBER_AFTER:
        case N_VAR_DECL_AFTER:
        case N_EXPR_OPT:
            return lookahead == T_SEMI;
        case N_PARAM_LIST_OPT:
        case N_PARAM_LIST:
        case N_PARAM_LIST_TAIL:
        case N_ARG_LIST_OPT:
        case N_ARG_LIST:
        case N_ARG_LIST_TAIL:
            return lookahead == T_RPAREN;
        case N_ASSIGN_TARGET:
        case N_EXPR:
        case N_REL_EXPR:
        case N_REL_EXPR_TAIL:
        case N_ADD_EXPR:
        case N_ADD_EXPR_TAIL:
        case N_TERM:
        case N_TERM_TAIL:
        case N_FACTOR:
        case N_FACTOR_AFTER:
            return is_expr_follow(lookahead);
        default:
            return 0;
    }
}

static Production production(int count, Symbol a, Symbol b, Symbol c, Symbol d, Symbol e, Symbol f, Symbol g) {
    Production p;
    Symbol symbols[RHS_MAX] = {a, b, c, d, e, f, g, SYM_INVALID};
    int i;
    p.count = count;
    for (i = 0; i < count; i++) {
        p.symbols[i] = symbols[i];
    }
    return p;
}

static Production table_lookup(Symbol nonterminal, Symbol lookahead) {
    switch (nonterminal) {
        case N_PROGRAM:
            if (lookahead == T_CLASS) return production(5, T_CLASS, T_ID, T_LBRACE, N_MEMBER_LIST, T_RBRACE, SYM_INVALID, SYM_INVALID);
            break;
        case N_MEMBER_LIST:
            if (is_type_start(lookahead)) return production(2, N_MEMBER, N_MEMBER_LIST, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_RBRACE) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_MEMBER:
            if (is_type_start(lookahead)) return production(3, N_TYPE, T_ID, N_MEMBER_AFTER, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_MEMBER_AFTER:
            if (lookahead == T_SEMI) return production(1, T_SEMI, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_LBRACKET) return production(4, T_LBRACKET, T_NUMBER, T_RBRACKET, T_SEMI, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_LPAREN) return production(4, T_LPAREN, N_PARAM_LIST_OPT, T_RPAREN, N_BLOCK, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_TYPE:
            if (lookahead == T_INT) return production(1, T_INT, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_CHAR) return production(1, T_CHAR, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_BOOLEAN) return production(1, T_BOOLEAN, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_VOID) return production(1, T_VOID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_PARAM_LIST_OPT:
            if (is_type_start(lookahead)) return production(1, N_PARAM_LIST, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_RPAREN) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_PARAM_LIST:
            if (is_type_start(lookahead)) return production(3, N_TYPE, T_ID, N_PARAM_LIST_TAIL, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_PARAM_LIST_TAIL:
            if (lookahead == T_COMMA) return production(4, T_COMMA, N_TYPE, T_ID, N_PARAM_LIST_TAIL, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_RPAREN) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_BLOCK:
            if (lookahead == T_LBRACE) return production(3, T_LBRACE, N_STMT_LIST, T_RBRACE, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_STMT_LIST:
            if (is_stmt_start(lookahead)) return production(2, N_STMT, N_STMT_LIST, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_RBRACE) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_STMT:
            if (is_type_start(lookahead)) return production(4, N_TYPE, T_ID, N_VAR_DECL_AFTER, T_SEMI, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_ID) return production(3, T_ID, N_STMT_ID_TAIL, T_SEMI, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_RETURN) return production(3, T_RETURN, N_EXPR_OPT, T_SEMI, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_IF) return production(5, T_IF, T_LPAREN, N_EXPR, T_RPAREN, N_STMT, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_WHILE) return production(5, T_WHILE, T_LPAREN, N_EXPR, T_RPAREN, N_STMT, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_LBRACE) return production(1, N_BLOCK, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_SEMI) return production(1, T_SEMI, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_PRINT) return production(5, T_PRINT, T_LPAREN, N_EXPR, T_RPAREN, T_SEMI, SYM_INVALID, SYM_INVALID);
            break;
        case N_VAR_DECL_AFTER:
            if (lookahead == T_LBRACKET) return production(3, T_LBRACKET, T_NUMBER, T_RBRACKET, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_SEMI) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_STMT_ID_TAIL:
            if (lookahead == T_LPAREN) return production(3, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_LBRACKET || lookahead == T_ASSIGN) return production(3, N_ASSIGN_TARGET, T_ASSIGN, N_EXPR, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_ASSIGN_TARGET:
            if (lookahead == T_LBRACKET) return production(3, T_LBRACKET, N_EXPR, T_RBRACKET, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_ASSIGN) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_EXPR_OPT:
            if (is_expr_start(lookahead)) return production(1, N_EXPR, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_SEMI) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_ARG_LIST_OPT:
            if (is_expr_start(lookahead)) return production(1, N_ARG_LIST, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_RPAREN) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_ARG_LIST:
            if (is_expr_start(lookahead)) return production(2, N_EXPR, N_ARG_LIST_TAIL, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_ARG_LIST_TAIL:
            if (lookahead == T_COMMA) return production(3, T_COMMA, N_EXPR, N_ARG_LIST_TAIL, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_RPAREN) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_EXPR:
            if (is_expr_start(lookahead)) return production(1, N_REL_EXPR, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_REL_EXPR:
            if (is_expr_start(lookahead)) return production(2, N_ADD_EXPR, N_REL_EXPR_TAIL, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_REL_EXPR_TAIL:
            if (is_rel_op(lookahead)) return production(2, N_REL_OP, N_ADD_EXPR, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (is_expr_follow(lookahead)) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_REL_OP:
            if (lookahead == T_EQ) return production(1, T_EQ, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_NE) return production(1, T_NE, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_LT) return production(1, T_LT, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_LE) return production(1, T_LE, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_GT) return production(1, T_GT, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_GE) return production(1, T_GE, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_ADD_EXPR:
            if (is_expr_start(lookahead)) return production(2, N_TERM, N_ADD_EXPR_TAIL, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_ADD_EXPR_TAIL:
            if (is_add_op(lookahead)) return production(3, N_ADD_OP, N_TERM, N_ADD_EXPR_TAIL, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (is_rel_op(lookahead) || is_expr_follow(lookahead)) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_ADD_OP:
            if (lookahead == T_PLUS) return production(1, T_PLUS, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_MINUS) return production(1, T_MINUS, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_TERM:
            if (is_expr_start(lookahead)) return production(2, N_FACTOR, N_TERM_TAIL, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_TERM_TAIL:
            if (is_mul_op(lookahead)) return production(3, N_MUL_OP, N_FACTOR, N_TERM_TAIL, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (is_add_op(lookahead) || is_rel_op(lookahead) || is_expr_follow(lookahead)) return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_MUL_OP:
            if (lookahead == T_STAR) return production(1, T_STAR, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_SLASH) return production(1, T_SLASH, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_PERCENT) return production(1, T_PERCENT, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_FACTOR:
            if (lookahead == T_ID) return production(2, T_ID, N_FACTOR_AFTER, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_NUMBER) return production(1, T_NUMBER, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_CHAR_CONST) return production(1, T_CHAR_CONST, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_TRUE) return production(1, T_TRUE, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_FALSE) return production(1, T_FALSE, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_LPAREN) return production(3, T_LPAREN, N_EXPR, T_RPAREN, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            break;
        case N_FACTOR_AFTER:
            if (lookahead == T_LPAREN) return production(3, T_LPAREN, N_ARG_LIST_OPT, T_RPAREN, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (lookahead == T_LBRACKET) return production(3, T_LBRACKET, N_EXPR, T_RBRACKET, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            if (is_mul_op(lookahead) || is_add_op(lookahead) || is_rel_op(lookahead) || is_expr_follow(lookahead)) {
                return production(0, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
            }
            break;
        default:
            break;
    }
    return production(-1, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID, SYM_INVALID);
}

static void push(Symbol *stack, int *top, Symbol symbol) {
    if (*top >= STACK_MAX - 1) {
        fprintf(stderr, "Parser stack overflow\n");
        exit(1);
    }
    stack[++(*top)] = symbol;
}

int main(int argc, char **argv) {
    FILE *file;
    DoubleBufferReader reader;
    Token current;
    Symbol stack[STACK_MAX];
    int top = -1;
    int errors = 0;
    ErrorList error_list;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source.mj>\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Cannot open source file: %s\n", argv[1]);
        return 1;
    }

    init_reader(&reader, file);
    error_list_init(&error_list);
    current = next_token(&reader);
    push(stack, &top, T_EOF);
    push(stack, &top, N_PROGRAM);

    while (top >= 0) {
        Symbol expected = stack[top--];
        Symbol lookahead = lookahead_symbol(current);

        if (current.type == TOKEN_ERROR) {
            error_report(&error_list, ERROR_LEXICAL, current.line, current.column,
                         "%s '%s'", current.attribute, current.lexeme);
            errors++;
            current = next_token(&reader);
            continue;
        }

        if (is_terminal(expected)) {
            if (expected == lookahead) {
                current = next_token(&reader);
            } else {
                error_report(&error_list, ERROR_SYNTACTIC, current.line, current.column,
                             "expected %s, found %s '%s'", symbol_name(expected),
                             token_type_name(current.type), current.lexeme);
                errors++;
                if (current.type != TOKEN_EOF && !is_sync_symbol(expected, lookahead)) {
                    current = next_token(&reader);
                }
            }
        } else {
            Production p = table_lookup(expected, lookahead);
            int i;
            if (p.count < 0) {
                error_report(&error_list, ERROR_SYNTACTIC, current.line, current.column,
                             "no LL(1) rule for %s with lookahead %s '%s'",
                             symbol_name(expected), token_type_name(current.type), current.lexeme);
                errors++;
                if (current.type != TOKEN_EOF && !is_sync_symbol(expected, lookahead)) {
                    current = next_token(&reader);
                }
                continue;
            }
            for (i = p.count - 1; i >= 0; i--) {
                push(stack, &top, p.symbols[i]);
            }
        }
    }

    fclose(file);
    error_print_summary(&error_list);
    if (errors == 0) {
        printf("Predictive parser: ACCEPT\n");
        return 0;
    }
    printf("Predictive parser: REJECT (%d error(s))\n", errors);
    return 1;
}
