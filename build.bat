@echo off
setlocal

if not exist obj mkdir obj

gcc -Wall -Wextra -std=c11 -DLEXER_STANDALONE src\lexer.c -o lexer.exe
gcc -Wall -Wextra -std=c11 src\recursive_descent_parser.c src\lexer.c src\symbol_table.c src\error_handler.c -o recursive.exe
gcc -Wall -Wextra -std=c11 src\predictive_parser.c src\lexer.c src\error_handler.c -o predictive.exe
gcc -Wall -Wextra -std=c11 src\lr_parser.c src\lexer.c src\error_handler.c -o lr.exe

gcc -Wall -Wextra -std=c11 -DLEXER_STANDALONE -Dmain=lexer_main -c src\lexer.c -o obj\lexer_module.o
gcc -Wall -Wextra -std=c11 -Dmain=recursive_main -c src\recursive_descent_parser.c -o obj\recursive_module.o
gcc -Wall -Wextra -std=c11 -Dmain=predictive_main -c src\predictive_parser.c -o obj\predictive_module.o
gcc -Wall -Wextra -std=c11 -Dmain=lr_main -c src\lr_parser.c -o obj\lr_module.o
gcc -Wall -Wextra -std=c11 -c src\symbol_table.c -o obj\symbol_table.o
gcc -Wall -Wextra -std=c11 -c src\error_handler.c -o obj\error_handler.o
gcc -Wall -Wextra -std=c11 -c src\compiler.c -o obj\compiler.o
gcc obj\compiler.o obj\lexer_module.o obj\recursive_module.o obj\predictive_module.o obj\lr_module.o obj\symbol_table.o obj\error_handler.o -o compiler.exe

echo Build complete.
