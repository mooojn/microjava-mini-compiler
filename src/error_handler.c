#include <stdio.h>
#include <string.h>

#include "error_handler.h"

void error_list_init(ErrorList *errors) {
    errors->count = 0;
    errors->overflow = 0;
}

const char *error_kind_name(ErrorKind kind) {
    switch (kind) {
        case ERROR_LEXICAL: return "Lexical";
        case ERROR_SYNTACTIC: return "Syntax";
        case ERROR_SEMANTIC: return "Semantic";
        default: return "Unknown";
    }
}

void error_vreport(ErrorList *errors, ErrorKind kind, int line, int column, const char *format, va_list args) {
    char message[ERROR_MESSAGE_MAX];

    vsnprintf(message, sizeof(message), format, args);
    fprintf(stderr, "%s error at %d:%d: %s\n", error_kind_name(kind), line, column, message);

    if (errors == NULL) {
        return;
    }
    if (errors->count >= ERROR_MAX) {
        errors->overflow = 1;
        return;
    }

    errors->items[errors->count].kind = kind;
    errors->items[errors->count].line = line;
    errors->items[errors->count].column = column;
    strncpy(errors->items[errors->count].message, message, ERROR_MESSAGE_MAX - 1);
    errors->items[errors->count].message[ERROR_MESSAGE_MAX - 1] = '\0';
    errors->count++;
}

void error_report(ErrorList *errors, ErrorKind kind, int line, int column, const char *format, ...) {
    va_list args;
    va_start(args, format);
    error_vreport(errors, kind, line, column, format, args);
    va_end(args);
}

void error_print_summary(const ErrorList *errors) {
    int lexical = 0;
    int syntactic = 0;
    int semantic = 0;
    int i;

    if (errors == NULL) {
        return;
    }

    for (i = 0; i < errors->count; i++) {
        if (errors->items[i].kind == ERROR_LEXICAL) lexical++;
        else if (errors->items[i].kind == ERROR_SYNTACTIC) syntactic++;
        else if (errors->items[i].kind == ERROR_SEMANTIC) semantic++;
    }

    printf("Error summary: %d total (%d lexical, %d syntax, %d semantic)",
           errors->count, lexical, syntactic, semantic);
    if (errors->overflow) {
        printf(" plus additional errors not stored");
    }
    printf("\n");
}
