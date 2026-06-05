# Mini Compiler Project Requirements

This document outlines the complete set of requirements for the CS-471L Compiler Construction Lab final project. The work is divided into 5 logical sections. Completing all 5 sections guarantees the full completion of the project.

## Section 1: Project Foundation and Lexical Analyzer
**Goal:** Establish the MicroJava (.mj) grammar, parsing tables, and implement the lexical analyzer.

- [x] Ensure the MicroJava (.mj) subset and BNF grammar are finalized.
- [x] Transform the grammar: Remove left recursion and left-factor it for top-down parsers.
- [x] Calculate FIRST and FOLLOW sets.
- [x] Construct the LL(1) parsing table.
- [x] Construct the LR parsing table (action + goto) and justify the choice (LR(0), SLR(1), LALR(1), canonical LR(1)).
- [x] Create at least three sample input programs.
- [x] **Deliverable D1 (Week 13):** Submit Group Sheet, BNF grammar, FIRST/FOLLOW sets, LL(1) table, LR table, and sample programs.
- [x] **Lexical Analyzer (Lab 3):**
  - [x] Read source character by character using double buffering (from Lab 2).
  - [x] Track line and column numbers for every token.
  - [x] Skip whitespace and comments.
  - [x] Recognize identifiers, keywords, numbers, character constants, operators, and punctuation.
  - [x] Produce a token stream (token, attribute pairs) to pass to the parser on demand.

## Section 2: Top-Down Parsers
**Goal:** Implement the Recursive Descent Parser and Non-Recursive Predictive Parser using the transformed grammar.

- [x] **Recursive Descent Parser (Lab 4):**
  - [x] Write one parsing routine per non-terminal.
  - [x] Call the lexer to read the next token within each routine.
  - [x] Report syntax errors if the input does not match expected rules.
  - [x] Use the left-factored, non-left-recursive grammar.
- [x] **Non-Recursive Predictive Parser (Lab 5):**
  - [x] Implement an explicit stack-based parser.
  - [x] Use the constructed LL(1) parsing table.
  - [x] Ensure it produces the same parse result as the Recursive Descent Parser for the same input.

## Section 3: Bottom-Up Parser
**Goal:** Implement the new LR Parser module.

- [x] **LR Parser (New Module):**
  - [x] Implement the standard shift-reduce algorithm using a stack of states and a stack of grammar symbols.
  - [x] Use the original (potentially left-recursive) BNF grammar.
  - [x] Use the chosen LR parsing table (action + goto).
  - [x] Generate a trace for at least one sample input, showing stack contents, remaining input, and actions taken at each step.

## Section 4: Semantic Analysis and Error Handling
**Goal:** Implement the Symbol Table Manager and the Bonus Error Handler.

- [x] **Symbol Table Manager (Lab 7, Extended):**
  - [x] Use a hash-based table for fast lookup.
  - [x] Support insert, lookup, and delete operations.
  - [x] Store name, kind (variable/constant/function/array/class), type, scope level, and source line number.
  - [x] Handle nested scopes (using a stack of symbol tables or linked scope chain).
  - [x] Ensure the parser calls the symbol table at every declaration and name usage.
  - [x] Implement a function to print/dump the symbol table contents for debugging.
- [x] **Error Handler (Bonus Task):**
  - [x] Detect and report lexical, syntactic, and semantic errors.
  - [x] Print line number, column number, and a short message for each error.
  - [x] Implement panic-mode recovery so parsing doesn't stop at the first error.
  - [x] *LR Parser:* Use error entries in the parsing table for recovery.
  - [x] *Predictive Parser:* Use synchronization sets (based on FOLLOW sets) to skip tokens.
  - [x] Print a summary of all detected errors at the end.
  - [x] *(Extra Bonus)* Implement phrase-level recovery and clear, helpful error messages.

## Section 5: Integration, Documentation, and Final Deliverables
**Goal:** Combine all modules into a single application, prepare documentation, and get ready for the demo.

- [ ] **Integration:**
  - [ ] Link all modules (Lexer, 3 Parsers, Symbol Table, Error Handler) into one program.
  - [ ] Ensure all modules build and run from a single command.
  - [ ] Verify that all 3 parsers accept the same valid input and reject the same invalid input.
- [ ] **Ensure Project Setup & Build Script:**
  - [ ] Create a `Makefile` or batch file.
  - [ ] Create folder structure: `src/`, `docs/`, `test/`, `output/`.
  - [ ] Write a `README.md` with build and run instructions.
- [ ] **Deliverable D3: Project Report in root in latex format (Week 16):**
  - [ ] Include grammar specification.
  - [ ] Describe each module.
  - [ ] Detail construction of LR parsing table (show working).
  - [ ] Discuss module integration.
  - [ ] Include sample inputs and outputs.
  - [ ] List limitations and references.
  - [ ] Include LL(1) table, FIRST/FOLLOW sets, LR action/goto tables, and sample traces.
