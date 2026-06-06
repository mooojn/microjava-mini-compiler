# Section 1 Grammar

This project uses a compact MicroJava subset with classes, fields, methods, local declarations, assignments, calls, loops, conditionals, returns, printing, arrays, arithmetic, relational expressions, and character constants.

## Original BNF

The original grammar keeps expression precedence left-recursive for the LR parser.

- `Program ::= class id { MemberList }`
- `MemberList ::= Member MemberList | epsilon`
- `Member ::= Type id MemberAfter`
- `MemberAfter ::= ; | [ number ] ; | ( ParamListOpt ) Block`
- `Type ::= int | char | boolean | void`
- `ParamListOpt ::= ParamList | epsilon`
- `ParamList ::= Type id ParamListTail`
- `ParamListTail ::= , Type id ParamListTail | epsilon`
- `Block ::= { StmtList }`
- `StmtList ::= Stmt StmtList | epsilon`
- `Stmt ::= Type id VarDeclAfter ; | id StmtIdTail ; | return ExprOpt ; | if ( Expr ) Stmt | while ( Expr ) Stmt | Block | ; | print ( Expr ) ;`
- `VarDeclAfter ::= [ number ] | epsilon`
- `StmtIdTail ::= AssignTarget = Expr | ( ArgListOpt )`
- `AssignTarget ::= [ Expr ] | epsilon`
- `ExprOpt ::= Expr | epsilon`
- `ArgListOpt ::= ArgList | epsilon`
- `ArgList ::= Expr ArgListTail`
- `ArgListTail ::= , Expr ArgListTail | epsilon`
- `RelOp ::= == | != | < | <= | > | >=`
- `AddOp ::= + | -`
- `MulOp ::= * | / | %`
- `Factor ::= id FactorAfter | number | char_const | true | false | ( Expr )`
- `FactorAfter ::= ( ArgListOpt ) | [ Expr ] | epsilon`
- `Expr ::= Expr RelOp AddExpr | AddExpr`
- `AddExpr ::= AddExpr AddOp Term | Term`
- `Term ::= Term MulOp Factor | Factor`

## LL(1) Transformed Grammar

- `Program ::= class id { MemberList }`
- `MemberList ::= Member MemberList | epsilon`
- `Member ::= Type id MemberAfter`
- `MemberAfter ::= ; | [ number ] ; | ( ParamListOpt ) Block`
- `Type ::= int | char | boolean | void`
- `ParamListOpt ::= ParamList | epsilon`
- `ParamList ::= Type id ParamListTail`
- `ParamListTail ::= , Type id ParamListTail | epsilon`
- `Block ::= { StmtList }`
- `StmtList ::= Stmt StmtList | epsilon`
- `Stmt ::= Type id VarDeclAfter ; | id StmtIdTail ; | return ExprOpt ; | if ( Expr ) Stmt | while ( Expr ) Stmt | Block | ; | print ( Expr ) ;`
- `VarDeclAfter ::= [ number ] | epsilon`
- `StmtIdTail ::= AssignTarget = Expr | ( ArgListOpt )`
- `AssignTarget ::= [ Expr ] | epsilon`
- `ExprOpt ::= Expr | epsilon`
- `ArgListOpt ::= ArgList | epsilon`
- `ArgList ::= Expr ArgListTail`
- `ArgListTail ::= , Expr ArgListTail | epsilon`
- `Expr ::= RelExpr`
- `RelExpr ::= AddExpr RelExprTail`
- `RelExprTail ::= RelOp AddExpr | epsilon`
- `RelOp ::= == | != | < | <= | > | >=`
- `AddExpr ::= Term AddExprTail`
- `AddExprTail ::= AddOp Term AddExprTail | epsilon`
- `AddOp ::= + | -`
- `Term ::= Factor TermTail`
- `TermTail ::= MulOp Factor TermTail | epsilon`
- `MulOp ::= * | / | %`
- `Factor ::= id FactorAfter | number | char_const | true | false | ( Expr )`
- `FactorAfter ::= ( ArgListOpt ) | [ Expr ] | epsilon`

Transformation notes:
- Direct left recursion in expressions was replaced by `AddExprTail`, `TermTail`, and related tail non-terminals.
- Common prefixes such as member declarations and statement forms were left-factored.
- Empty productions are written as `epsilon`.
