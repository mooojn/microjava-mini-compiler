# CS471L Mini Compiler Project

This repository contains the completed CS-471L Compiler Construction Lab final project for a compact MicroJava `.mj` subset.

The project includes:

- MicroJava grammar, FIRST/FOLLOW sets, LL(1) table, LR table, and sample programs.
- Double-buffered lexical analyzer with token attributes and line/column tracking.
- Recursive descent parser.
- Non-recursive predictive parser using an explicit stack and LL(1) table.
- SLR(1) bottom-up parser with shift/reduce trace output.
- Hash-based symbol table manager with nested scopes.
- Shared error handler for lexical, syntactic, and semantic errors.
- Integrated `compiler.exe` dispatcher for lexer, all three parsers, symbol table, and error handler.
- Sample valid and invalid test programs.

## Project Structure

```text
src/                  C source files and grammar table generator
docs/                 Grammar, parsing tables, FIRST/FOLLOW sets, and traces
output/               Generated build objects and demonstration outputs
test/samples/         Valid and invalid MicroJava sample programs
reqs.md               Completed project requirements checklist
Makefile              Build targets for all executables
build.bat             Windows one-command build script
report.tex            Week 16 final report in LaTeX format
```

## Build

If `make` is installed:

```powershell
make
```

On Windows, the simplest complete build command is:

```powershell
.\build.bat
```

On Windows without `make`, build each executable directly:

```powershell
gcc -Wall -Wextra -std=c11 -DLEXER_STANDALONE src/lexer.c -o lexer
gcc -Wall -Wextra -std=c11 src/recursive_descent_parser.c src/lexer.c src/symbol_table.c src/error_handler.c -o recursive
gcc -Wall -Wextra -std=c11 src/predictive_parser.c src/lexer.c src/error_handler.c -o predictive
gcc -Wall -Wextra -std=c11 src/lr_parser.c src/lexer.c src/error_handler.c -o lr
```

## Run

Run the lexer:

```powershell
.\lexer.exe test\samples\hello.mj
```

Run all parsers on a valid sample:

```powershell
.\compiler.exe all test\samples\hello.mj
.\recursive.exe test\samples\hello.mj
.\predictive.exe test\samples\hello.mj
.\lr.exe test\samples\hello.mj
```

Run semantic analysis and symbol-table dumping:

```powershell
.\recursive.exe test\samples\invalid_semantic.mj
.\recursive.exe test\samples\hello.mj --dump-symbols
```

Run syntax-error recovery examples:

```powershell
.\recursive.exe test\samples\invalid_missing_semicolon.mj
.\predictive.exe test\samples\invalid_missing_semicolon.mj
.\lr.exe test\samples\invalid_missing_semicolon.mj
```

Run individual modules through the integrated executable:

```powershell
.\compiler.exe lexer test\samples\hello.mj
.\compiler.exe recursive test\samples\hello.mj --dump-symbols
.\compiler.exe predictive test\samples\hello.mj
.\compiler.exe lr test\samples\hello.mj
```

## Generate Grammar Artifacts

```powershell
python src/grammar_tables.py
```

Generated grammar artifacts are written under `docs/section1/`.

## Completion Status

All five planned project sections are implemented or prepared for final submission:

- Section 1: Project foundation and lexical analyzer.
- Section 2: Top-down parsers.
- Section 3: Bottom-up LR parser.
- Section 4: Semantic analysis and error handling.
- Section 5: Integration, documentation, build setup, and final deliverable preparation.
