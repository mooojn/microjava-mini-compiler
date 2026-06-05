#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static const char *keywords[] = {
    "class", "public", "static", "void", "int", "char", "boolean",
    "if", "else", "while", "return", "true", "false", "new",
    "print", "read"
};

static void fill_buffer(DoubleBufferReader *reader, int index) {
    if (reader->eof_seen) {
        reader->lengths[index] = 0;
        return;
    }

    reader->lengths[index] = (int)fread(reader->buffers[index], 1, BUFFER_SIZE, reader->file);
    if (reader->lengths[index] < BUFFER_SIZE) {
        reader->eof_seen = 1;
    }
}

void init_reader(DoubleBufferReader *reader, FILE *file) {
    reader->file = file;
    reader->active = 0;
    reader->offset = 0;
    reader->eof_seen = 0;
    reader->line = 1;
    reader->column = 1;
    fill_buffer(reader, 0);
    fill_buffer(reader, 1);
}

static char peek_char(DoubleBufferReader *reader, int distance) {
    int active = reader->active;
    int offset = reader->offset + distance;

    while (offset >= reader->lengths[active]) {
        offset -= reader->lengths[active];
        active = 1 - active;
        if (reader->lengths[active] == 0) {
            return '\0';
        }
    }

    return reader->buffers[active][offset];
}

static char advance_char(DoubleBufferReader *reader) {
    char ch = peek_char(reader, 0);
    int exhausted;

    if (ch == '\0') {
        return '\0';
    }

    reader->offset++;
    if (ch == '\n') {
        reader->line++;
        reader->column = 1;
    } else {
        reader->column++;
    }

    if (reader->offset >= reader->lengths[reader->active]) {
        exhausted = reader->active;
        reader->active = 1 - reader->active;
        reader->offset = 0;
        fill_buffer(reader, exhausted);
    }

    return ch;
}

const char *token_type_name(TokenType type) {
    switch (type) {
        case TOKEN_KEYWORD: return "KEYWORD";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_CHAR_CONST: return "CHAR_CONST";
        case TOKEN_OPERATOR: return "OPERATOR";
        case TOKEN_PUNCTUATION: return "PUNCTUATION";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

static int is_keyword(const char *lexeme) {
    size_t i;
    for (i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(lexeme, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int is_one_char_operator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' ||
           ch == '=' || ch == '<' || ch == '>' || ch == '!';
}

static int is_punctuation(char ch) {
    return ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == '[' ||
           ch == ']' || ch == ';' || ch == ',' || ch == '.';
}

static int is_two_char_operator(char first, char second) {
    return (first == '=' && second == '=') ||
           (first == '!' && second == '=') ||
           (first == '<' && second == '=') ||
           (first == '>' && second == '=') ||
           (first == '&' && second == '&') ||
           (first == '|' && second == '|');
}

static void append_char(char *text, int *length, char ch) {
    if (*length < MAX_LEXEME - 1) {
        text[*length] = ch;
        (*length)++;
        text[*length] = '\0';
    }
}

static Token make_token(TokenType type, const char *lexeme, int line, int column, const char *attribute) {
    Token token;
    token.type = type;
    token.line = line;
    token.column = column;
    strncpy(token.lexeme, lexeme, MAX_LEXEME - 1);
    token.lexeme[MAX_LEXEME - 1] = '\0';
    strncpy(token.attribute, attribute, MAX_LEXEME - 1);
    token.attribute[MAX_LEXEME - 1] = '\0';
    return token;
}

static void skip_whitespace_and_comments(DoubleBufferReader *reader) {
    int again = 1;

    while (again) {
        again = 0;
        while (peek_char(reader, 0) != '\0' && isspace((unsigned char)peek_char(reader, 0))) {
            advance_char(reader);
        }

        if (peek_char(reader, 0) == '/' && peek_char(reader, 1) == '/') {
            while (peek_char(reader, 0) != '\0' && peek_char(reader, 0) != '\n') {
                advance_char(reader);
            }
            again = 1;
        } else if (peek_char(reader, 0) == '/' && peek_char(reader, 1) == '*') {
            advance_char(reader);
            advance_char(reader);
            while (peek_char(reader, 0) != '\0') {
                if (peek_char(reader, 0) == '*' && peek_char(reader, 1) == '/') {
                    advance_char(reader);
                    advance_char(reader);
                    break;
                }
                advance_char(reader);
            }
            again = 1;
        }
    }
}

static Token read_identifier_or_keyword(DoubleBufferReader *reader) {
    char lexeme[MAX_LEXEME] = "";
    int length = 0;
    int line = reader->line;
    int column = reader->column;

    while (peek_char(reader, 0) != '\0' &&
           (isalnum((unsigned char)peek_char(reader, 0)) || peek_char(reader, 0) == '_')) {
        append_char(lexeme, &length, advance_char(reader));
    }

    if (is_keyword(lexeme)) {
        return make_token(TOKEN_KEYWORD, lexeme, line, column, lexeme);
    }
    return make_token(TOKEN_IDENTIFIER, lexeme, line, column, lexeme);
}

static Token read_number(DoubleBufferReader *reader) {
    char lexeme[MAX_LEXEME] = "";
    int length = 0;
    int line = reader->line;
    int column = reader->column;

    while (peek_char(reader, 0) != '\0' && isdigit((unsigned char)peek_char(reader, 0))) {
        append_char(lexeme, &length, advance_char(reader));
    }

    return make_token(TOKEN_NUMBER, lexeme, line, column, lexeme);
}

static Token read_char_const(DoubleBufferReader *reader) {
    char lexeme[MAX_LEXEME] = "";
    char attribute[MAX_LEXEME] = "";
    int length = 0;
    int line = reader->line;
    int column = reader->column;

    append_char(lexeme, &length, advance_char(reader));
    if (peek_char(reader, 0) == '\0') {
        return make_token(TOKEN_ERROR, lexeme, line, column, "Unterminated character constant");
    }

    append_char(lexeme, &length, advance_char(reader));
    if (lexeme[length - 1] == '\\' && peek_char(reader, 0) != '\0') {
        append_char(lexeme, &length, advance_char(reader));
    }

    if (peek_char(reader, 0) != '\'') {
        while (peek_char(reader, 0) != '\0' && peek_char(reader, 0) != '\'' && peek_char(reader, 0) != '\n') {
            append_char(lexeme, &length, advance_char(reader));
        }
        if (peek_char(reader, 0) == '\'') {
            append_char(lexeme, &length, advance_char(reader));
        }
        return make_token(TOKEN_ERROR, lexeme, line, column, "Invalid character constant");
    }

    append_char(lexeme, &length, advance_char(reader));
    if (length == 3) {
        attribute[0] = lexeme[1];
        attribute[1] = '\0';
    } else {
        strncpy(attribute, lexeme + 1, (size_t)(length - 2));
        attribute[length - 2] = '\0';
    }
    return make_token(TOKEN_CHAR_CONST, lexeme, line, column, attribute);
}

Token next_token(DoubleBufferReader *reader) {
    char ch;
    char second;
    char lexeme[3] = "";
    int line;
    int column;

    skip_whitespace_and_comments(reader);

    line = reader->line;
    column = reader->column;
    ch = peek_char(reader, 0);

    if (ch == '\0') {
        return make_token(TOKEN_EOF, "", line, column, "");
    }
    if (isalpha((unsigned char)ch) || ch == '_') {
        return read_identifier_or_keyword(reader);
    }
    if (isdigit((unsigned char)ch)) {
        return read_number(reader);
    }
    if (ch == '\'') {
        return read_char_const(reader);
    }

    second = peek_char(reader, 1);
    if (is_two_char_operator(ch, second)) {
        lexeme[0] = advance_char(reader);
        lexeme[1] = advance_char(reader);
        lexeme[2] = '\0';
        return make_token(TOKEN_OPERATOR, lexeme, line, column, lexeme);
    }
    if (is_one_char_operator(ch)) {
        lexeme[0] = advance_char(reader);
        lexeme[1] = '\0';
        return make_token(TOKEN_OPERATOR, lexeme, line, column, lexeme);
    }
    if (is_punctuation(ch)) {
        lexeme[0] = advance_char(reader);
        lexeme[1] = '\0';
        return make_token(TOKEN_PUNCTUATION, lexeme, line, column, lexeme);
    }

    lexeme[0] = advance_char(reader);
    lexeme[1] = '\0';
    return make_token(TOKEN_ERROR, lexeme, line, column, "Unexpected character");
}

#ifdef LEXER_STANDALONE
int main(int argc, char **argv) {
    FILE *file;
    DoubleBufferReader reader;
    Token token;

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
    do {
        token = next_token(&reader);
        printf("%d:%d\t%s\t'%s'\t", token.line, token.column, token_type_name(token.type), token.lexeme);
        if (token.attribute[0] == '\0') {
            printf("NULL\n");
        } else {
            printf("'%s'\n", token.attribute);
        }
    } while (token.type != TOKEN_EOF);

    fclose(file);
    return 0;
}
#endif
