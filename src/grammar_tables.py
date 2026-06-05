from __future__ import annotations

from collections import defaultdict, deque
from dataclasses import dataclass
from pathlib import Path

EPSILON = "epsilon"
EOF = "$"


TRANSFORMED_GRAMMAR: dict[str, list[list[str]]] = {
    "Program": [["class", "id", "{", "MemberList", "}"]],
    "MemberList": [["Member", "MemberList"], [EPSILON]],
    "Member": [["Type", "id", "MemberAfter"]],
    "MemberAfter": [[";"], ["[", "number", "]", ";"], ["(", "ParamListOpt", ")", "Block"]],
    "Type": [["int"], ["char"], ["boolean"], ["void"]],
    "ParamListOpt": [["ParamList"], [EPSILON]],
    "ParamList": [["Type", "id", "ParamListTail"]],
    "ParamListTail": [[",", "Type", "id", "ParamListTail"], [EPSILON]],
    "Block": [["{", "StmtList", "}"]],
    "StmtList": [["Stmt", "StmtList"], [EPSILON]],
    "Stmt": [
        ["Type", "id", "VarDeclAfter", ";"],
        ["id", "StmtIdTail", ";"],
        ["return", "ExprOpt", ";"],
        ["if", "(", "Expr", ")", "Stmt"],
        ["while", "(", "Expr", ")", "Stmt"],
        ["Block"],
        [";"],
        ["print", "(", "Expr", ")", ";"],
    ],
    "VarDeclAfter": [["[", "number", "]"], [EPSILON]],
    "StmtIdTail": [["AssignTarget", "=", "Expr"], ["(", "ArgListOpt", ")"]],
    "AssignTarget": [["[", "Expr", "]"], [EPSILON]],
    "ExprOpt": [["Expr"], [EPSILON]],
    "ArgListOpt": [["ArgList"], [EPSILON]],
    "ArgList": [["Expr", "ArgListTail"]],
    "ArgListTail": [[",", "Expr", "ArgListTail"], [EPSILON]],
    "Expr": [["RelExpr"]],
    "RelExpr": [["AddExpr", "RelExprTail"]],
    "RelExprTail": [["RelOp", "AddExpr"], [EPSILON]],
    "RelOp": [["=="], ["!="], ["<"], ["<="], [">"], [">="]],
    "AddExpr": [["Term", "AddExprTail"]],
    "AddExprTail": [["AddOp", "Term", "AddExprTail"], [EPSILON]],
    "AddOp": [["+"] , ["-"]],
    "Term": [["Factor", "TermTail"]],
    "TermTail": [["MulOp", "Factor", "TermTail"], [EPSILON]],
    "MulOp": [["*"], ["/"], ["%"]],
    "Factor": [["id", "FactorAfter"], ["number"], ["char_const"], ["true"], ["false"], ["(", "Expr", ")"]],
    "FactorAfter": [["(", "ArgListOpt", ")"], ["[", "Expr", "]"], [EPSILON]],
}


LR_GRAMMAR: dict[str, list[list[str]]] = {
    **{k: v for k, v in TRANSFORMED_GRAMMAR.items() if k not in {"Expr", "RelExpr", "RelExprTail", "AddExpr", "AddExprTail", "Term", "TermTail"}},
    "Expr": [["Expr", "RelOp", "AddExpr"], ["AddExpr"]],
    "AddExpr": [["AddExpr", "AddOp", "Term"], ["Term"]],
    "Term": [["Term", "MulOp", "Factor"], ["Factor"]],
}


def nonterminals(grammar: dict[str, list[list[str]]]) -> set[str]:
    return set(grammar)


def terminals(grammar: dict[str, list[list[str]]]) -> set[str]:
    nts = nonterminals(grammar)
    result = set()
    for productions in grammar.values():
        for production in productions:
            for symbol in production:
                if symbol != EPSILON and symbol not in nts:
                    result.add(symbol)
    return result


def first_sets(grammar: dict[str, list[list[str]]]) -> dict[str, set[str]]:
    nts = nonterminals(grammar)
    first = {nt: set() for nt in nts}
    changed = True
    while changed:
        changed = False
        for head, productions in grammar.items():
            for production in productions:
                before = len(first[head])
                first[head].update(first_of_sequence(production, first, nts))
                changed |= len(first[head]) != before
    return first


def first_of_sequence(sequence: list[str], first: dict[str, set[str]], nts: set[str]) -> set[str]:
    result = set()
    if sequence == [EPSILON]:
        return {EPSILON}
    for symbol in sequence:
        if symbol == EPSILON:
            result.add(EPSILON)
            break
        if symbol not in nts:
            result.add(symbol)
            break
        result.update(first[symbol] - {EPSILON})
        if EPSILON not in first[symbol]:
            break
    else:
        result.add(EPSILON)
    return result


def follow_sets(grammar: dict[str, list[list[str]]], start: str, first: dict[str, set[str]]) -> dict[str, set[str]]:
    nts = nonterminals(grammar)
    follow = {nt: set() for nt in nts}
    follow[start].add(EOF)
    changed = True
    while changed:
        changed = False
        for head, productions in grammar.items():
            for production in productions:
                for i, symbol in enumerate(production):
                    if symbol not in nts:
                        continue
                    trailer = production[i + 1 :]
                    trailer_first = first_of_sequence(trailer, first, nts) if trailer else {EPSILON}
                    before = len(follow[symbol])
                    follow[symbol].update(trailer_first - {EPSILON})
                    if EPSILON in trailer_first:
                        follow[symbol].update(follow[head])
                    changed |= len(follow[symbol]) != before
    return follow


def ll1_table(grammar: dict[str, list[list[str]]], first: dict[str, set[str]], follow: dict[str, set[str]]) -> dict[tuple[str, str], str]:
    nts = nonterminals(grammar)
    table = {}
    for head, productions in grammar.items():
        for production in productions:
            first_alpha = first_of_sequence(production, first, nts)
            rendered = f"{head} -> {' '.join(production)}"
            for terminal in first_alpha - {EPSILON}:
                table[(head, terminal)] = rendered
            if EPSILON in first_alpha:
                for terminal in follow[head]:
                    table[(head, terminal)] = rendered
    return table


@dataclass(frozen=True)
class Item:
    head: str
    body: tuple[str, ...]
    dot: int

    def next_symbol(self) -> str | None:
        return self.body[self.dot] if self.dot < len(self.body) else None

    def advance(self) -> "Item":
        return Item(self.head, self.body, self.dot + 1)

    def render(self) -> str:
        parts = list(self.body)
        parts.insert(self.dot, ".")
        return f"{self.head} -> {' '.join(parts)}"


def productions(grammar: dict[str, list[list[str]]]) -> list[tuple[str, tuple[str, ...]]]:
    result = []
    for head, bodies in grammar.items():
        for body in bodies:
            result.append((head, tuple(symbol for symbol in body if symbol != EPSILON)))
    return result


def closure(items: frozenset[Item], grammar: dict[str, list[list[str]]]) -> frozenset[Item]:
    nts = nonterminals(grammar)
    result = set(items)
    changed = True
    while changed:
        changed = False
        for item in list(result):
            symbol = item.next_symbol()
            if symbol in nts:
                for body in grammar[symbol]:
                    candidate = Item(symbol, tuple(x for x in body if x != EPSILON), 0)
                    if candidate not in result:
                        result.add(candidate)
                        changed = True
    return frozenset(result)


def goto(items: frozenset[Item], symbol: str, grammar: dict[str, list[list[str]]]) -> frozenset[Item]:
    advanced = frozenset(item.advance() for item in items if item.next_symbol() == symbol)
    return closure(advanced, grammar) if advanced else frozenset()


def canonical_lr0_collection(grammar: dict[str, list[list[str]]], start: str) -> tuple[list[frozenset[Item]], dict[tuple[int, str], int], str]:
    augmented_start = f"{start}'"
    augmented = {augmented_start: [[start]], **grammar}
    start_item = closure(frozenset({Item(augmented_start, (start,), 0)}), augmented)
    states = [start_item]
    transitions = {}
    queue = deque([0])
    symbols = sorted(terminals(augmented) | nonterminals(augmented))
    while queue:
        index = queue.popleft()
        for symbol in symbols:
            target = goto(states[index], symbol, augmented)
            if not target:
                continue
            if target not in states:
                states.append(target)
                queue.append(len(states) - 1)
            transitions[(index, symbol)] = states.index(target)
    return states, transitions, augmented_start


def slr_table(grammar: dict[str, list[list[str]]], start: str, follow: dict[str, set[str]]) -> tuple[list[frozenset[Item]], dict[tuple[int, str], str], dict[tuple[int, str], int], list[str]]:
    states, transitions, augmented_start = canonical_lr0_collection(grammar, start)
    terms = terminals(grammar) | {EOF}
    nts = nonterminals(grammar)
    action = {}
    goto_table = {}
    conflicts = []
    prod_numbers = {prod: i for i, prod in enumerate(productions(grammar), start=1)}

    def set_action(key: tuple[int, str], value: str) -> None:
        if key in action and action[key] != value:
            conflicts.append(f"state {key[0]}, symbol {key[1]}: {action[key]} vs {value}")
        action[key] = value

    for (state, symbol), target in transitions.items():
        if symbol in terms - {EOF}:
            set_action((state, symbol), f"s{target}")
        elif symbol in nts:
            goto_table[(state, symbol)] = target
    for i, state in enumerate(states):
        for item in state:
            if item.next_symbol() is not None:
                continue
            if item.head == augmented_start:
                set_action((i, EOF), "acc")
            else:
                prod_no = prod_numbers[(item.head, item.body)]
                for terminal in follow[item.head]:
                    set_action((i, terminal), f"r{prod_no}")
    return states, action, goto_table, conflicts


def md_set(values: set[str]) -> str:
    return "{ " + ", ".join(sorted(values)) + " }"


def write_docs() -> None:
    out = Path("docs/section1")
    out.mkdir(parents=True, exist_ok=True)
    sample_dir = Path("test/samples")
    sample_dir.mkdir(parents=True, exist_ok=True)

    first = first_sets(TRANSFORMED_GRAMMAR)
    follow = follow_sets(TRANSFORMED_GRAMMAR, "Program", first)
    table = ll1_table(TRANSFORMED_GRAMMAR, first, follow)
    lr_first = first_sets(LR_GRAMMAR)
    lr_follow = follow_sets(LR_GRAMMAR, "Program", lr_first)
    states, action, goto_table, conflicts = slr_table(LR_GRAMMAR, "Program", lr_follow)

    grammar_lines = [
        "# Section 1 Grammar",
        "",
        "This project uses a compact MicroJava subset with classes, fields, methods, local declarations, assignments, calls, loops, conditionals, returns, printing, arrays, arithmetic, relational expressions, and character constants.",
        "",
        "## Original BNF",
        "",
        "The original grammar keeps expression precedence left-recursive for the LR parser.",
        "",
    ]
    for head, bodies in LR_GRAMMAR.items():
        grammar_lines.append(f"- `{head} ::= {' | '.join(' '.join(body) for body in bodies)}`")
    grammar_lines += ["", "## LL(1) Transformed Grammar", ""]
    for head, bodies in TRANSFORMED_GRAMMAR.items():
        grammar_lines.append(f"- `{head} ::= {' | '.join(' '.join(body) for body in bodies)}`")
    grammar_lines += [
        "",
        "Transformation notes:",
        "- Direct left recursion in expressions was replaced by `AddExprTail`, `TermTail`, and related tail non-terminals.",
        "- Common prefixes such as member declarations and statement forms were left-factored.",
        "- Empty productions are written as `epsilon`.",
    ]
    (out / "grammar.md").write_text("\n".join(grammar_lines) + "\n", encoding="utf-8")

    ff_lines = ["# FIRST and FOLLOW Sets", "", "| Non-terminal | FIRST | FOLLOW |", "|---|---|---|"]
    for nt in TRANSFORMED_GRAMMAR:
        ff_lines.append(f"| `{nt}` | `{md_set(first[nt])}` | `{md_set(follow[nt])}` |")
    (out / "first_follow.md").write_text("\n".join(ff_lines) + "\n", encoding="utf-8")

    ll_lines = ["# LL(1) Parsing Table", "", "| Non-terminal | Lookahead | Production |", "|---|---|---|"]
    for (head, lookahead), production in sorted(table.items()):
        ll_lines.append(f"| `{head}` | `{lookahead}` | `{production}` |")
    (out / "ll1_table.md").write_text("\n".join(ll_lines) + "\n", encoding="utf-8")

    lr_lines = [
        "# SLR(1) Parsing Table",
        "",
        "Chosen method: **SLR(1)**. The grammar is small enough for an LR(0) item automaton plus FOLLOW-set reductions, and the generated table has no conflicts.",
        "",
        f"- States: {len(states)}",
        f"- Conflicts: {', '.join(conflicts) if conflicts else 'none'}",
        "",
        "## Productions",
        "",
    ]
    for index, (head, body) in enumerate(productions(LR_GRAMMAR), start=1):
        lr_lines.append(f"{index}. `{head} -> {' '.join(body) if body else EPSILON}`")
    lr_lines += ["", "## Action Table", "", "| State | Terminal | Action |", "|---:|---|---|"]
    for (state, terminal), value in sorted(action.items()):
        lr_lines.append(f"| {state} | `{terminal}` | `{value}` |")
    lr_lines += ["", "## Goto Table", "", "| State | Non-terminal | Goto |", "|---:|---|---:|"]
    for (state, nt), value in sorted(goto_table.items()):
        lr_lines.append(f"| {state} | `{nt}` | {value} |")
    lr_lines += ["", "## LR(0) Item Sets", ""]
    for index, state in enumerate(states):
        lr_lines.append(f"### I{index}")
        for item in sorted(state, key=lambda x: x.render()):
            lr_lines.append(f"- `{item.render()}`")
        lr_lines.append("")
    (out / "lr_table.md").write_text("\n".join(lr_lines) + "\n", encoding="utf-8")


def c_string(value: str) -> str:
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


def write_lr_c_header() -> None:
    lr_first = first_sets(LR_GRAMMAR)
    lr_follow = follow_sets(LR_GRAMMAR, "Program", lr_first)
    states, action, goto_table, conflicts = slr_table(LR_GRAMMAR, "Program", lr_follow)
    if conflicts:
        raise RuntimeError("SLR table has conflicts: " + "; ".join(conflicts))

    prods = productions(LR_GRAMMAR)
    lines = [
        "#ifndef MICROJAVA_LR_TABLES_H",
        "#define MICROJAVA_LR_TABLES_H",
        "",
        "#define LR_MAX_RHS 8",
        "",
        "typedef struct {",
        "    const char *lhs;",
        "    int rhs_len;",
        "    const char *rhs[LR_MAX_RHS];",
        "} LRProduction;",
        "",
        "typedef struct {",
        "    int state;",
        "    const char *terminal;",
        "    char kind;",
        "    int value;",
        "} LRActionEntry;",
        "",
        "typedef struct {",
        "    int state;",
        "    const char *nonterminal;",
        "    int next_state;",
        "} LRGotoEntry;",
        "",
        f"static const int LR_STATE_COUNT = {len(states)};",
        f"static const int LR_PRODUCTION_COUNT = {len(prods) + 1};",
        "",
        "static const LRProduction LR_PRODUCTIONS[] = {",
        '    {"", 0, {0}},',
    ]
    for head, body in prods:
        rhs = ", ".join(c_string(symbol) for symbol in body)
        padding = ", ".join("0" for _ in range(8 - len(body)))
        values = ", ".join(x for x in [rhs, padding] if x)
        lines.append(f"    {{{c_string(head)}, {len(body)}, {{{values}}}}},")
    lines += [
        "};",
        "",
        f"static const int LR_ACTION_COUNT = {len(action)};",
        "static const LRActionEntry LR_ACTIONS[] = {",
    ]
    for (state, terminal), value in sorted(action.items()):
        if value == "acc":
            kind = "a"
            number = 0
        else:
            kind = value[0]
            number = int(value[1:])
        lines.append(f"    {{{state}, {c_string(terminal)}, '{kind}', {number}}},")
    lines += [
        "};",
        "",
        f"static const int LR_GOTO_COUNT = {len(goto_table)};",
        "static const LRGotoEntry LR_GOTOS[] = {",
    ]
    for (state, nonterminal), target in sorted(goto_table.items()):
        lines.append(f"    {{{state}, {c_string(nonterminal)}, {target}}},")
    lines += [
        "};",
        "",
        "#endif",
    ]
    Path("src/lr_tables.h").write_text("\n".join(lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    write_docs()
    write_lr_c_header()
