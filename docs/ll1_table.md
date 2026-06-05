# LL(1) Parsing Table

| Non-terminal | Lookahead | Production |
|---|---|---|
| `AddExpr` | `(` | `AddExpr -> Term AddExprTail` |
| `AddExpr` | `char_const` | `AddExpr -> Term AddExprTail` |
| `AddExpr` | `false` | `AddExpr -> Term AddExprTail` |
| `AddExpr` | `id` | `AddExpr -> Term AddExprTail` |
| `AddExpr` | `number` | `AddExpr -> Term AddExprTail` |
| `AddExpr` | `true` | `AddExpr -> Term AddExprTail` |
| `AddExprTail` | `!=` | `AddExprTail -> epsilon` |
| `AddExprTail` | `)` | `AddExprTail -> epsilon` |
| `AddExprTail` | `+` | `AddExprTail -> AddOp Term AddExprTail` |
| `AddExprTail` | `,` | `AddExprTail -> epsilon` |
| `AddExprTail` | `-` | `AddExprTail -> AddOp Term AddExprTail` |
| `AddExprTail` | `;` | `AddExprTail -> epsilon` |
| `AddExprTail` | `<` | `AddExprTail -> epsilon` |
| `AddExprTail` | `<=` | `AddExprTail -> epsilon` |
| `AddExprTail` | `==` | `AddExprTail -> epsilon` |
| `AddExprTail` | `>` | `AddExprTail -> epsilon` |
| `AddExprTail` | `>=` | `AddExprTail -> epsilon` |
| `AddExprTail` | `]` | `AddExprTail -> epsilon` |
| `AddOp` | `+` | `AddOp -> +` |
| `AddOp` | `-` | `AddOp -> -` |
| `ArgList` | `(` | `ArgList -> Expr ArgListTail` |
| `ArgList` | `char_const` | `ArgList -> Expr ArgListTail` |
| `ArgList` | `false` | `ArgList -> Expr ArgListTail` |
| `ArgList` | `id` | `ArgList -> Expr ArgListTail` |
| `ArgList` | `number` | `ArgList -> Expr ArgListTail` |
| `ArgList` | `true` | `ArgList -> Expr ArgListTail` |
| `ArgListOpt` | `(` | `ArgListOpt -> ArgList` |
| `ArgListOpt` | `)` | `ArgListOpt -> epsilon` |
| `ArgListOpt` | `char_const` | `ArgListOpt -> ArgList` |
| `ArgListOpt` | `false` | `ArgListOpt -> ArgList` |
| `ArgListOpt` | `id` | `ArgListOpt -> ArgList` |
| `ArgListOpt` | `number` | `ArgListOpt -> ArgList` |
| `ArgListOpt` | `true` | `ArgListOpt -> ArgList` |
| `ArgListTail` | `)` | `ArgListTail -> epsilon` |
| `ArgListTail` | `,` | `ArgListTail -> , Expr ArgListTail` |
| `AssignTarget` | `=` | `AssignTarget -> epsilon` |
| `AssignTarget` | `[` | `AssignTarget -> [ Expr ]` |
| `Block` | `{` | `Block -> { StmtList }` |
| `Expr` | `(` | `Expr -> RelExpr` |
| `Expr` | `char_const` | `Expr -> RelExpr` |
| `Expr` | `false` | `Expr -> RelExpr` |
| `Expr` | `id` | `Expr -> RelExpr` |
| `Expr` | `number` | `Expr -> RelExpr` |
| `Expr` | `true` | `Expr -> RelExpr` |
| `ExprOpt` | `(` | `ExprOpt -> Expr` |
| `ExprOpt` | `;` | `ExprOpt -> epsilon` |
| `ExprOpt` | `char_const` | `ExprOpt -> Expr` |
| `ExprOpt` | `false` | `ExprOpt -> Expr` |
| `ExprOpt` | `id` | `ExprOpt -> Expr` |
| `ExprOpt` | `number` | `ExprOpt -> Expr` |
| `ExprOpt` | `true` | `ExprOpt -> Expr` |
| `Factor` | `(` | `Factor -> ( Expr )` |
| `Factor` | `char_const` | `Factor -> char_const` |
| `Factor` | `false` | `Factor -> false` |
| `Factor` | `id` | `Factor -> id FactorAfter` |
| `Factor` | `number` | `Factor -> number` |
| `Factor` | `true` | `Factor -> true` |
| `FactorAfter` | `!=` | `FactorAfter -> epsilon` |
| `FactorAfter` | `%` | `FactorAfter -> epsilon` |
| `FactorAfter` | `(` | `FactorAfter -> ( ArgListOpt )` |
| `FactorAfter` | `)` | `FactorAfter -> epsilon` |
| `FactorAfter` | `*` | `FactorAfter -> epsilon` |
| `FactorAfter` | `+` | `FactorAfter -> epsilon` |
| `FactorAfter` | `,` | `FactorAfter -> epsilon` |
| `FactorAfter` | `-` | `FactorAfter -> epsilon` |
| `FactorAfter` | `/` | `FactorAfter -> epsilon` |
| `FactorAfter` | `;` | `FactorAfter -> epsilon` |
| `FactorAfter` | `<` | `FactorAfter -> epsilon` |
| `FactorAfter` | `<=` | `FactorAfter -> epsilon` |
| `FactorAfter` | `==` | `FactorAfter -> epsilon` |
| `FactorAfter` | `>` | `FactorAfter -> epsilon` |
| `FactorAfter` | `>=` | `FactorAfter -> epsilon` |
| `FactorAfter` | `[` | `FactorAfter -> [ Expr ]` |
| `FactorAfter` | `]` | `FactorAfter -> epsilon` |
| `Member` | `boolean` | `Member -> Type id MemberAfter` |
| `Member` | `char` | `Member -> Type id MemberAfter` |
| `Member` | `int` | `Member -> Type id MemberAfter` |
| `Member` | `void` | `Member -> Type id MemberAfter` |
| `MemberAfter` | `(` | `MemberAfter -> ( ParamListOpt ) Block` |
| `MemberAfter` | `;` | `MemberAfter -> ;` |
| `MemberAfter` | `[` | `MemberAfter -> [ number ] ;` |
| `MemberList` | `boolean` | `MemberList -> Member MemberList` |
| `MemberList` | `char` | `MemberList -> Member MemberList` |
| `MemberList` | `int` | `MemberList -> Member MemberList` |
| `MemberList` | `void` | `MemberList -> Member MemberList` |
| `MemberList` | `}` | `MemberList -> epsilon` |
| `MulOp` | `%` | `MulOp -> %` |
| `MulOp` | `*` | `MulOp -> *` |
| `MulOp` | `/` | `MulOp -> /` |
| `ParamList` | `boolean` | `ParamList -> Type id ParamListTail` |
| `ParamList` | `char` | `ParamList -> Type id ParamListTail` |
| `ParamList` | `int` | `ParamList -> Type id ParamListTail` |
| `ParamList` | `void` | `ParamList -> Type id ParamListTail` |
| `ParamListOpt` | `)` | `ParamListOpt -> epsilon` |
| `ParamListOpt` | `boolean` | `ParamListOpt -> ParamList` |
| `ParamListOpt` | `char` | `ParamListOpt -> ParamList` |
| `ParamListOpt` | `int` | `ParamListOpt -> ParamList` |
| `ParamListOpt` | `void` | `ParamListOpt -> ParamList` |
| `ParamListTail` | `)` | `ParamListTail -> epsilon` |
| `ParamListTail` | `,` | `ParamListTail -> , Type id ParamListTail` |
| `Program` | `class` | `Program -> class id { MemberList }` |
| `RelExpr` | `(` | `RelExpr -> AddExpr RelExprTail` |
| `RelExpr` | `char_const` | `RelExpr -> AddExpr RelExprTail` |
| `RelExpr` | `false` | `RelExpr -> AddExpr RelExprTail` |
| `RelExpr` | `id` | `RelExpr -> AddExpr RelExprTail` |
| `RelExpr` | `number` | `RelExpr -> AddExpr RelExprTail` |
| `RelExpr` | `true` | `RelExpr -> AddExpr RelExprTail` |
| `RelExprTail` | `!=` | `RelExprTail -> RelOp AddExpr` |
| `RelExprTail` | `)` | `RelExprTail -> epsilon` |
| `RelExprTail` | `,` | `RelExprTail -> epsilon` |
| `RelExprTail` | `;` | `RelExprTail -> epsilon` |
| `RelExprTail` | `<` | `RelExprTail -> RelOp AddExpr` |
| `RelExprTail` | `<=` | `RelExprTail -> RelOp AddExpr` |
| `RelExprTail` | `==` | `RelExprTail -> RelOp AddExpr` |
| `RelExprTail` | `>` | `RelExprTail -> RelOp AddExpr` |
| `RelExprTail` | `>=` | `RelExprTail -> RelOp AddExpr` |
| `RelExprTail` | `]` | `RelExprTail -> epsilon` |
| `RelOp` | `!=` | `RelOp -> !=` |
| `RelOp` | `<` | `RelOp -> <` |
| `RelOp` | `<=` | `RelOp -> <=` |
| `RelOp` | `==` | `RelOp -> ==` |
| `RelOp` | `>` | `RelOp -> >` |
| `RelOp` | `>=` | `RelOp -> >=` |
| `Stmt` | `;` | `Stmt -> ;` |
| `Stmt` | `boolean` | `Stmt -> Type id VarDeclAfter ;` |
| `Stmt` | `char` | `Stmt -> Type id VarDeclAfter ;` |
| `Stmt` | `id` | `Stmt -> id StmtIdTail ;` |
| `Stmt` | `if` | `Stmt -> if ( Expr ) Stmt` |
| `Stmt` | `int` | `Stmt -> Type id VarDeclAfter ;` |
| `Stmt` | `print` | `Stmt -> print ( Expr ) ;` |
| `Stmt` | `return` | `Stmt -> return ExprOpt ;` |
| `Stmt` | `void` | `Stmt -> Type id VarDeclAfter ;` |
| `Stmt` | `while` | `Stmt -> while ( Expr ) Stmt` |
| `Stmt` | `{` | `Stmt -> Block` |
| `StmtIdTail` | `(` | `StmtIdTail -> ( ArgListOpt )` |
| `StmtIdTail` | `=` | `StmtIdTail -> AssignTarget = Expr` |
| `StmtIdTail` | `[` | `StmtIdTail -> AssignTarget = Expr` |
| `StmtList` | `;` | `StmtList -> Stmt StmtList` |
| `StmtList` | `boolean` | `StmtList -> Stmt StmtList` |
| `StmtList` | `char` | `StmtList -> Stmt StmtList` |
| `StmtList` | `id` | `StmtList -> Stmt StmtList` |
| `StmtList` | `if` | `StmtList -> Stmt StmtList` |
| `StmtList` | `int` | `StmtList -> Stmt StmtList` |
| `StmtList` | `print` | `StmtList -> Stmt StmtList` |
| `StmtList` | `return` | `StmtList -> Stmt StmtList` |
| `StmtList` | `void` | `StmtList -> Stmt StmtList` |
| `StmtList` | `while` | `StmtList -> Stmt StmtList` |
| `StmtList` | `{` | `StmtList -> Stmt StmtList` |
| `StmtList` | `}` | `StmtList -> epsilon` |
| `Term` | `(` | `Term -> Factor TermTail` |
| `Term` | `char_const` | `Term -> Factor TermTail` |
| `Term` | `false` | `Term -> Factor TermTail` |
| `Term` | `id` | `Term -> Factor TermTail` |
| `Term` | `number` | `Term -> Factor TermTail` |
| `Term` | `true` | `Term -> Factor TermTail` |
| `TermTail` | `!=` | `TermTail -> epsilon` |
| `TermTail` | `%` | `TermTail -> MulOp Factor TermTail` |
| `TermTail` | `)` | `TermTail -> epsilon` |
| `TermTail` | `*` | `TermTail -> MulOp Factor TermTail` |
| `TermTail` | `+` | `TermTail -> epsilon` |
| `TermTail` | `,` | `TermTail -> epsilon` |
| `TermTail` | `-` | `TermTail -> epsilon` |
| `TermTail` | `/` | `TermTail -> MulOp Factor TermTail` |
| `TermTail` | `;` | `TermTail -> epsilon` |
| `TermTail` | `<` | `TermTail -> epsilon` |
| `TermTail` | `<=` | `TermTail -> epsilon` |
| `TermTail` | `==` | `TermTail -> epsilon` |
| `TermTail` | `>` | `TermTail -> epsilon` |
| `TermTail` | `>=` | `TermTail -> epsilon` |
| `TermTail` | `]` | `TermTail -> epsilon` |
| `Type` | `boolean` | `Type -> boolean` |
| `Type` | `char` | `Type -> char` |
| `Type` | `int` | `Type -> int` |
| `Type` | `void` | `Type -> void` |
| `VarDeclAfter` | `;` | `VarDeclAfter -> epsilon` |
| `VarDeclAfter` | `[` | `VarDeclAfter -> [ number ]` |
