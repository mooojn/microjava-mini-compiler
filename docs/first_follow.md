# FIRST and FOLLOW Sets

| Non-terminal | FIRST | FOLLOW |
|---|---|---|
| `Program` | `{ class }` | `{ $ }` |
| `MemberList` | `{ boolean, char, epsilon, int, void }` | `{ } }` |
| `Member` | `{ boolean, char, int, void }` | `{ boolean, char, int, void, } }` |
| `MemberAfter` | `{ (, ;, [ }` | `{ boolean, char, int, void, } }` |
| `Type` | `{ boolean, char, int, void }` | `{ id }` |
| `ParamListOpt` | `{ boolean, char, epsilon, int, void }` | `{ ) }` |
| `ParamList` | `{ boolean, char, int, void }` | `{ ) }` |
| `ParamListTail` | `{ ,, epsilon }` | `{ ) }` |
| `Block` | `{ { }` | `{ ;, boolean, char, id, if, int, print, return, void, while, {, } }` |
| `StmtList` | `{ ;, boolean, char, epsilon, id, if, int, print, return, void, while, { }` | `{ } }` |
| `Stmt` | `{ ;, boolean, char, id, if, int, print, return, void, while, { }` | `{ ;, boolean, char, id, if, int, print, return, void, while, {, } }` |
| `VarDeclAfter` | `{ [, epsilon }` | `{ ; }` |
| `StmtIdTail` | `{ (, =, [ }` | `{ ; }` |
| `AssignTarget` | `{ [, epsilon }` | `{ = }` |
| `ExprOpt` | `{ (, char_const, epsilon, false, id, number, true }` | `{ ; }` |
| `ArgListOpt` | `{ (, char_const, epsilon, false, id, number, true }` | `{ ) }` |
| `ArgList` | `{ (, char_const, false, id, number, true }` | `{ ) }` |
| `ArgListTail` | `{ ,, epsilon }` | `{ ) }` |
| `Expr` | `{ (, char_const, false, id, number, true }` | `{ ), ,, ;, ] }` |
| `RelExpr` | `{ (, char_const, false, id, number, true }` | `{ ), ,, ;, ] }` |
| `RelExprTail` | `{ !=, <, <=, ==, >, >=, epsilon }` | `{ ), ,, ;, ] }` |
| `RelOp` | `{ !=, <, <=, ==, >, >= }` | `{ (, char_const, false, id, number, true }` |
| `AddExpr` | `{ (, char_const, false, id, number, true }` | `{ !=, ), ,, ;, <, <=, ==, >, >=, ] }` |
| `AddExprTail` | `{ +, -, epsilon }` | `{ !=, ), ,, ;, <, <=, ==, >, >=, ] }` |
| `AddOp` | `{ +, - }` | `{ (, char_const, false, id, number, true }` |
| `Term` | `{ (, char_const, false, id, number, true }` | `{ !=, ), +, ,, -, ;, <, <=, ==, >, >=, ] }` |
| `TermTail` | `{ %, *, /, epsilon }` | `{ !=, ), +, ,, -, ;, <, <=, ==, >, >=, ] }` |
| `MulOp` | `{ %, *, / }` | `{ (, char_const, false, id, number, true }` |
| `Factor` | `{ (, char_const, false, id, number, true }` | `{ !=, %, ), *, +, ,, -, /, ;, <, <=, ==, >, >=, ] }` |
| `FactorAfter` | `{ (, [, epsilon }` | `{ !=, %, ), *, +, ,, -, /, ;, <, <=, ==, >, >=, ] }` |
