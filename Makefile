CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: lexer recursive predictive lr compiler

lexer: src/lexer.c
	$(CC) $(CFLAGS) -DLEXER_STANDALONE src/lexer.c -o lexer

recursive: src/recursive_descent_parser.c src/lexer.c src/lexer.h src/symbol_table.c src/symbol_table.h src/error_handler.c src/error_handler.h
	$(CC) $(CFLAGS) src/recursive_descent_parser.c src/lexer.c src/symbol_table.c src/error_handler.c -o recursive

predictive: src/predictive_parser.c src/lexer.c src/lexer.h src/error_handler.c src/error_handler.h
	$(CC) $(CFLAGS) src/predictive_parser.c src/lexer.c src/error_handler.c -o predictive

lr: src/lr_parser.c src/lexer.c src/lexer.h src/lr_tables.h src/error_handler.c src/error_handler.h
	$(CC) $(CFLAGS) src/lr_parser.c src/lexer.c src/error_handler.c -o lr

obj:
	if not exist obj mkdir obj

compiler: obj src/compiler.c src/lexer.c src/lexer.h src/recursive_descent_parser.c src/predictive_parser.c src/lr_parser.c src/lr_tables.h src/symbol_table.c src/symbol_table.h src/error_handler.c src/error_handler.h
	$(CC) $(CFLAGS) -DLEXER_STANDALONE -Dmain=lexer_main -c src/lexer.c -o obj/lexer_module.o
	$(CC) $(CFLAGS) -Dmain=recursive_main -c src/recursive_descent_parser.c -o obj/recursive_module.o
	$(CC) $(CFLAGS) -Dmain=predictive_main -c src/predictive_parser.c -o obj/predictive_module.o
	$(CC) $(CFLAGS) -Dmain=lr_main -c src/lr_parser.c -o obj/lr_module.o
	$(CC) $(CFLAGS) -c src/symbol_table.c -o obj/symbol_table.o
	$(CC) $(CFLAGS) -c src/error_handler.c -o obj/error_handler.o
	$(CC) $(CFLAGS) -c src/compiler.c -o obj/compiler.o
	$(CC) obj/compiler.o obj/lexer_module.o obj/recursive_module.o obj/predictive_module.o obj/lr_module.o obj/symbol_table.o obj/error_handler.o -o compiler

run: lexer
	./lexer test/samples/hello.mj

parse: recursive predictive lr
	./recursive test/samples/hello.mj
	./predictive test/samples/hello.mj
	./lr test/samples/hello.mj

integrated: compiler
	./compiler all test/samples/hello.mj

clean:
	del /Q lexer.exe lexer recursive.exe recursive predictive.exe predictive lr.exe lr compiler.exe compiler obj\*.o 2>NUL || exit 0
