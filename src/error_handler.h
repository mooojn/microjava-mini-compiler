#ifndef MICROJAVA_ERROR_HANDLER_H
#define MICROJAVA_ERROR_HANDLER_H

#include <stdarg.h>

#define ERROR_MAX 512
#define ERROR_MESSAGE_MAX 256

typedef enum {
    ERROR_LEXICAL,
    ERROR_SYNTACTIC,
    ERROR_SEMANTIC
} ErrorKind;

typedef struct {
    ErrorKind kind;
    int line;
    int column;
    char message[ERROR_MESSAGE_MAX];
} CompilerError;

typedef struct {
    CompilerError items[ERROR_MAX];
    int count;
    int overflow;
} ErrorList;

void error_list_init(ErrorList *errors);
void error_report(ErrorList *errors, ErrorKind kind, int line, int column, const char *format, ...);
void error_vreport(ErrorList *errors, ErrorKind kind, int line, int column, const char *format, va_list args);
void error_print_summary(const ErrorList *errors);
const char *error_kind_name(ErrorKind kind);

#endif
