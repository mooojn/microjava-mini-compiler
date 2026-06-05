#ifndef MICROJAVA_LEXER_H
#define MICROJAVA_LEXER_H

#include <stdio.h>

#define BUFFER_SIZE 4096
#define MAX_LEXEME 256

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_CHAR_CONST,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME];
    char attribute[MAX_LEXEME];
    int line;
    int column;
} Token;

typedef struct {
    FILE *file;
    char buffers[2][BUFFER_SIZE];
    int lengths[2];
    int active;
    int offset;
    int eof_seen;
    int line;
    int column;
} DoubleBufferReader;

void init_reader(DoubleBufferReader *reader, FILE *file);
Token next_token(DoubleBufferReader *reader);
const char *token_type_name(TokenType type);

#endif
