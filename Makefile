CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: lexer recursive predictive lr

lexer: src/lexer.c
	$(CC) $(CFLAGS) -DLEXER_STANDALONE src/lexer.c -o lexer

recursive: src/recursive_descent_parser.c src/lexer.c src/lexer.h src/symbol_table.c src/symbol_table.h src/error_handler.c src/error_handler.h
	$(CC) $(CFLAGS) src/recursive_descent_parser.c src/lexer.c src/symbol_table.c src/error_handler.c -o recursive

predictive: src/predictive_parser.c src/lexer.c src/lexer.h src/error_handler.c src/error_handler.h
	$(CC) $(CFLAGS) src/predictive_parser.c src/lexer.c src/error_handler.c -o predictive

lr: src/lr_parser.c src/lexer.c src/lexer.h src/lr_tables.h src/error_handler.c src/error_handler.h
	$(CC) $(CFLAGS) src/lr_parser.c src/lexer.c src/error_handler.c -o lr

run: lexer
	./lexer test/samples/hello.mj

parse: recursive predictive lr
	./recursive test/samples/hello.mj
	./predictive test/samples/hello.mj
	./lr test/samples/hello.mj

clean:
	del /Q lexer.exe lexer recursive.exe recursive predictive.exe predictive lr.exe lr 2>NUL || exit 0
