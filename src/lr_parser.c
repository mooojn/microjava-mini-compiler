#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_handler.h"
#include "lexer.h"
#include "lr_tables.h"

#define TOKEN_STREAM_MAX 4096
#define PARSER_STACK_MAX 4096
#define REMAINING_PREVIEW 8

typedef struct {
    int state;
    const char *symbol;
} StackEntry;

static int lexeme_equals(Token token, const char *lexeme) {
    return strcmp(token.lexeme, lexeme) == 0;
}

static const char *terminal_for_token(Token token) {
    if (token.type == TOKEN_IDENTIFIER) return "id";
    if (token.type == TOKEN_NUMBER) return "number";
    if (token.type == TOKEN_CHAR_CONST) return "char_const";
    if (token.type == TOKEN_EOF) return "$";
    if (lexeme_equals(token, "class")) return "class";
    if (lexeme_equals(token, "{")) return "{";
    if (lexeme_equals(token, "}")) return "}";
    if (lexeme_equals(token, ";")) return ";";
    if (lexeme_equals(token, "[")) return "[";
    if (lexeme_equals(token, "]")) return "]";
    if (lexeme_equals(token, "(")) return "(";
    if (lexeme_equals(token, ")")) return ")";
    if (lexeme_equals(token, "int")) return "int";
    if (lexeme_equals(token, "char")) return "char";
    if (lexeme_equals(token, "boolean")) return "boolean";
    if (lexeme_equals(token, "void")) return "void";
    if (lexeme_equals(token, ",")) return ",";
    if (lexeme_equals(token, "return")) return "return";
    if (lexeme_equals(token, "if")) return "if";
    if (lexeme_equals(token, "while")) return "while";
    if (lexeme_equals(token, "print")) return "print";
    if (lexeme_equals(token, "=")) return "=";
    if (lexeme_equals(token, "==")) return "==";
    if (lexeme_equals(token, "!=")) return "!=";
    if (lexeme_equals(token, "<")) return "<";
    if (lexeme_equals(token, "<=")) return "<=";
    if (lexeme_equals(token, ">")) return ">";
    if (lexeme_equals(token, ">=")) return ">=";
    if (lexeme_equals(token, "+")) return "+";
    if (lexeme_equals(token, "-")) return "-";
    if (lexeme_equals(token, "*")) return "*";
    if (lexeme_equals(token, "/")) return "/";
    if (lexeme_equals(token, "%")) return "%";
    if (lexeme_equals(token, "true")) return "true";
    if (lexeme_equals(token, "false")) return "false";
    return "?";
}

static const LRActionEntry *find_action(int state, const char *terminal) {
    int i;
    for (i = 0; i < LR_ACTION_COUNT; i++) {
        if (LR_ACTIONS[i].state == state && strcmp(LR_ACTIONS[i].terminal, terminal) == 0) {
            return &LR_ACTIONS[i];
        }
    }
    return NULL;
}

static int find_goto(int state, const char *nonterminal) {
    int i;
    for (i = 0; i < LR_GOTO_COUNT; i++) {
        if (LR_GOTOS[i].state == state && strcmp(LR_GOTOS[i].nonterminal, nonterminal) == 0) {
            return LR_GOTOS[i].next_state;
        }
    }
    return -1;
}

static void print_state_stack(StackEntry *stack, int top) {
    int i;
    printf("[");
    for (i = 0; i <= top; i++) {
        printf("%d", stack[i].state);
        if (i < top) printf(" ");
    }
    printf("]");
}

static void print_symbol_stack(StackEntry *stack, int top) {
    int i;
    printf("[");
    for (i = 1; i <= top; i++) {
        printf("%s", stack[i].symbol);
        if (i < top) printf(" ");
    }
    printf("]");
}

static void print_remaining(Token *tokens, int index, int count) {
    int i;
    printf("[");
    for (i = index; i < count && i < index + REMAINING_PREVIEW; i++) {
        const char *terminal = terminal_for_token(tokens[i]);
        printf("%s", terminal);
        if (i + 1 < count && i + 1 < index + REMAINING_PREVIEW) printf(" ");
    }
    if (index + REMAINING_PREVIEW < count) {
        printf(" ...");
    }
    printf("]");
}

static void print_trace_row(int step, StackEntry *stack, int top, Token *tokens, int index, int count, const char *action) {
    printf("%-5d ", step);
    print_state_stack(stack, top);
    printf("  ");
    print_symbol_stack(stack, top);
    printf("  ");
    print_remaining(tokens, index, count);
    printf("  %s\n", action);
}

static int read_tokens(const char *path, Token *tokens, int *count, ErrorList *errors) {
    FILE *file = fopen(path, "rb");
    DoubleBufferReader reader;
    Token token;

    if (file == NULL) {
        fprintf(stderr, "Cannot open source file: %s\n", path);
        return 0;
    }

    init_reader(&reader, file);
    *count = 0;
    do {
        if (*count >= TOKEN_STREAM_MAX) {
            fprintf(stderr, "Token stream too large\n");
            fclose(file);
            return 0;
        }
        token = next_token(&reader);
        if (token.type == TOKEN_ERROR) {
            error_report(errors, ERROR_LEXICAL, token.line, token.column,
                         "%s '%s'", token.attribute, token.lexeme);
            continue;
        }
        tokens[(*count)++] = token;
    } while (token.type != TOKEN_EOF);

    fclose(file);
    return 1;
}

int main(int argc, char **argv) {
    Token *tokens;
    int token_count = 0;
    int input_index = 0;
    StackEntry *stack;
    int top = 0;
    int step = 1;
    int errors = 0;
    ErrorList error_list;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source.mj>\n", argv[0]);
        return 1;
    }

    tokens = calloc(TOKEN_STREAM_MAX, sizeof(Token));
    stack = calloc(PARSER_STACK_MAX, sizeof(StackEntry));
    if (tokens == NULL || stack == NULL) {
        fprintf(stderr, "Out of memory\n");
        free(tokens);
        free(stack);
        return 1;
    }

    error_list_init(&error_list);
    if (!read_tokens(argv[1], tokens, &token_count, &error_list)) {
        free(tokens);
        free(stack);
        return 1;
    }

    stack[0].state = 0;
    stack[0].symbol = "$";

    printf("SLR(1) parser trace\n");
    printf("Grammar: original left-recursive expression grammar\n");
    printf("Step  States  Symbols  Remaining input  Action\n");

    while (1) {
        const char *terminal = terminal_for_token(tokens[input_index]);
        const LRActionEntry *action = find_action(stack[top].state, terminal);
        char action_text[256];

        if (action == NULL) {
            snprintf(action_text, sizeof(action_text), "error: no action for state %d and lookahead %s",
                     stack[top].state, terminal);
            print_trace_row(step++, stack, top, tokens, input_index, token_count, action_text);
            error_report(&error_list, ERROR_SYNTACTIC, tokens[input_index].line, tokens[input_index].column,
                         "LR error entry for state %d and lookahead %s near %s '%s'",
                         stack[top].state, terminal,
                         token_type_name(tokens[input_index].type), tokens[input_index].lexeme);
            errors++;
            if (tokens[input_index].type != TOKEN_EOF) {
                input_index++;
                continue;
            }
            error_print_summary(&error_list);
            printf("LR parser: REJECT\n");
            free(tokens);
            free(stack);
            return 1;
        }

        if (action->kind == 's') {
            snprintf(action_text, sizeof(action_text), "shift %d", action->value);
            print_trace_row(step++, stack, top, tokens, input_index, token_count, action_text);
            if (top >= PARSER_STACK_MAX - 1) {
                fprintf(stderr, "Parser stack overflow\n");
                free(tokens);
                free(stack);
                return 1;
            }
            top++;
            stack[top].symbol = terminal;
            stack[top].state = action->value;
            input_index++;
        } else if (action->kind == 'r') {
            LRProduction production = LR_PRODUCTIONS[action->value];
            int goto_state;

            snprintf(action_text, sizeof(action_text), "reduce r%d: %s -> %s",
                     action->value, production.lhs,
                     production.rhs_len == 0 ? "epsilon" : production.rhs[0]);
            print_trace_row(step++, stack, top, tokens, input_index, token_count, action_text);

            if (production.rhs_len > top) {
                fprintf(stderr, "Parser stack underflow during reduction\n");
                free(tokens);
                free(stack);
                return 1;
            }
            top -= production.rhs_len;
            goto_state = find_goto(stack[top].state, production.lhs);
            if (goto_state < 0) {
                fprintf(stderr, "Missing goto for state %d and nonterminal %s\n",
                        stack[top].state, production.lhs);
                free(tokens);
                free(stack);
                return 1;
            }
            top++;
            stack[top].symbol = production.lhs;
            stack[top].state = goto_state;
        } else if (action->kind == 'a') {
            print_trace_row(step, stack, top, tokens, input_index, token_count, "accept");
            error_print_summary(&error_list);
            if (errors > 0 || error_list.count > 0) {
                printf("LR parser: REJECT (%d error(s))\n", error_list.count);
                free(tokens);
                free(stack);
                return 1;
            }
            printf("LR parser: ACCEPT\n");
            free(tokens);
            free(stack);
            return 0;
        } else {
            fprintf(stderr, "Unknown LR action kind: %c\n", action->kind);
            free(tokens);
            free(stack);
            return 1;
        }
    }
}
