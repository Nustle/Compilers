import sys
import parser_edsl as pe
from dataclasses import dataclass
from pprint import pprint


# Type → ElementaryType | IDENT
@dataclass
class TypeNode:
    name: str


# AlgType → IDENT Types
@dataclass
class AlgType:
    construct: str
    types: list[TypeNode]


# Def → type IDENT : Alternatives .
@dataclass
class TypeDef:
    name: str
    alternatives: list[AlgType]


# FunSignature → ( IDENT Types ) -> Type
@dataclass
class FunSignature:
    func_name: str
    arg_types: list[TypeNode]
    ret_type: TypeNode


# Clause → FunctionPattern -> Expression
@dataclass
class Clause:
    pattern: "FunctionPattern"
    expr: "Expression"


# FunDef → fun FunSignature : Clauses .
@dataclass
class FunDef:
    signature: FunSignature
    clauses: list[Clause]


# Program → Defs Funcs
@dataclass
class Program:
    defs: list[TypeDef]
    funcs: list[FunDef]


# FuncParameter → IDENT | ListParam
class FuncParameter:
    pass


@dataclass
class VarParam(FuncParameter):
    name: str


@dataclass
class ListParam(FuncParameter):
    construct: str
    parameters: list[FuncParameter]


# FunctionPattern → ( IDENT FuncParameters )
@dataclass
class FunctionPattern:
    func_name: str
    parameters: list[FuncParameter]


# Expression → IDENT | Const | FuncCall | ListCall
class Expression:
    pass


@dataclass
class VarExpr(Expression):
    name: str


@dataclass
class ConstExpr(Expression):
    value: int


# FuncCall → ( Func Expressions )
@dataclass
class FuncCallExpr(Expression):
    func: str
    args: list[Expression]


# ListCall → [ IDENT Expressions ]
@dataclass
class ListCallExpr(Expression):
    construct: str
    args: list[Expression]


IDENT = pe.Terminal('IDENT', r'[A-Za-z_][A-Za-z_0-9]*', str)
INT_CONST = pe.Terminal('INT_CONST', r'[0-9]+', int)
KW_int = pe.Terminal('KW_int', 'int', lambda *args: 'int')


def make_keyword(image):
    return pe.Terminal(image, image, lambda *args: None, priority=10)


KW_type = make_keyword('type')
KW_fun = make_keyword('fun')
KW_add = make_keyword('add')
KW_mul = make_keyword('mul')

DOT = pe.Terminal('DOT', r'\.', lambda *args: None)
COLON = pe.Terminal('COLON', r':', lambda *args: None)
PIPE = pe.Terminal('PIPE', r'\|', lambda *args: None)
ARROW = pe.Terminal('ARROW', r'->', lambda *args: None)
LPAREN = pe.Terminal('LPAREN', r'\(', lambda *args: None)
RPAREN = pe.Terminal('RPAREN', r'\)', lambda *args: None)
LBRACK = pe.Terminal('LBRACK', r'\[', lambda *args: None)
RBRACK = pe.Terminal('RBRACK', r'\]', lambda *args: None)


def safe_lambda(expected, func):
    def wrapper(*args):
        if len(args) < expected:
            raise Exception(f"Not enough arguments in production (expected {expected}, got {len(args)}): {args}")
        return func(*args)

    return wrapper


NProgram = pe.NonTerminal('Program')
NDefs = pe.NonTerminal('Defs')
NDef = pe.NonTerminal('Def')
NAlternatives = pe.NonTerminal('Alternatives')

NAlgType = pe.NonTerminal('AlgType')
NTypes = pe.NonTerminal('Types')
NType = pe.NonTerminal('Type')
NElemType = pe.NonTerminal('ElementaryType')

NFuncs = pe.NonTerminal('Funcs')
NFunDef = pe.NonTerminal('FunDef')
NFunSignature = pe.NonTerminal('FunSignature')
NClauses = pe.NonTerminal('Clauses')
NClause = pe.NonTerminal('Clause')

NFunctionPattern = pe.NonTerminal('FunctionPattern')
NFuncParameters = pe.NonTerminal('FuncParameters')
NFuncParameter = pe.NonTerminal('FuncParameter')
NListParam = pe.NonTerminal('ListParam')

NExpression = pe.NonTerminal('Expression')
NConst = pe.NonTerminal('Const')
NFuncCall = pe.NonTerminal('FuncCall')
NExpressions = pe.NonTerminal('Expressions')
NListCall = pe.NonTerminal('ListCall')

NFunc = pe.NonTerminal('Func')
NAddOp = pe.NonTerminal('AddOp')
NMulOp = pe.NonTerminal('MulOp')

# Program → Defs Funcs
NProgram |= NDefs, NFuncs, lambda defs, funcs: Program(defs, funcs)

# Defs → ε | Defs Def
NDefs |= lambda: []
NDefs |= NDefs, NDef, lambda ds, d: ds + [d]

# Def → type IDENT : Alternatives .
NDef |= KW_type, IDENT, COLON, NAlternatives, DOT, safe_lambda(2, lambda *args: TypeDef(args[0], args[1]))

# Alternatives → AlgType | Alternatives "|" AlgType
NAlternatives |= NAlgType, safe_lambda(1, lambda *args: [args[0]])
NAlternatives |= NAlternatives, PIPE, NAlgType, safe_lambda(2, lambda *args: args[0] + [args[1]])

# AlgType → Construct Types
NAlgType |= IDENT, NTypes, lambda constr, types: AlgType(constr, types)

# Types → ε | Types Type
NTypes |= lambda: []
NTypes |= NTypes, NType, lambda ts, t: ts + t

# Type → ElementaryType | IDENT
NType |= NElemType, lambda elt: elt
NType |= IDENT, lambda ident: [TypeNode(ident)]

# ElementaryType → INT
NElemType |= KW_int, lambda *args: [TypeNode('int')]

# Funcs → ε | Funcs FunDef
NFuncs |= lambda: []
NFuncs |= NFuncs, NFunDef, lambda fs, f: fs + [f]

# FunDef → fun FunSignature : Clauses .
NFunDef |= KW_fun, NFunSignature, COLON, NClauses, DOT, safe_lambda(2, lambda *args: FunDef(args[0], args[1]))

# FunSignature → ( IDENT Types ) -> Type
NFunSignature |= LPAREN, IDENT, NTypes, RPAREN, ARROW, NType, safe_lambda(3, lambda *args: FunSignature(args[0],
                                                                                                           args[1],
                                                                                                           args[2][0]))

# Clauses → Clause | Clauses "|" Clause
NClauses |= NClause, lambda c: [c]
NClauses |= NClauses, PIPE, NClause, safe_lambda(2, lambda *args: args[0] + [args[1]])

# Clause → FunctionPattern -> Expression
NClause |= NFunctionPattern, ARROW, NExpression, safe_lambda(2, lambda *args: Clause(args[0], args[1]))

# FunctionPattern → ( IDENT FuncParameters )
NFunctionPattern |= LPAREN, IDENT, NFuncParameters, RPAREN, safe_lambda(2,
                                                                        lambda *args: FunctionPattern(args[0], args[1]))
# FuncParameters → ε | FuncParameters FuncParameter
NFuncParameters |= lambda: []
NFuncParameters |= NFuncParameters, NFuncParameter, lambda ps, p: ps + [p]

# FuncParameter → IDENT | ListParam
NFuncParameter |= IDENT, lambda ident: VarParam(ident)
NFuncParameter |= NListParam, lambda lp: lp

# ListParam → [ Construct FuncParameters ]
NListParam |= LBRACK, IDENT, NFuncParameters, RBRACK, safe_lambda(2, lambda *args: ListParam(args[0], args[1]))

# Expression → IDENT | Const | FuncCall | ListCall
NExpression |= IDENT, lambda ident: VarExpr(ident)
NExpression |= NConst, lambda const: const
NExpression |= NFuncCall, lambda fc: fc
NExpression |= NListCall, lambda lc: lc

# Const → INT_CONST
NConst |= INT_CONST, lambda i: ConstExpr(i)

# FuncCall → ( Func Expressions )
NFuncCall |= LPAREN, NFunc, NExpressions, RPAREN, safe_lambda(2, lambda *args: FuncCallExpr(args[0], args[1]))

# Func → AddOp | MulOp | IDENT
NFunc |= NAddOp, lambda op: op
NFunc |= NMulOp, lambda op: op
NFunc |= IDENT, lambda ident: ident

# AddOp → add
NAddOp |= KW_add, lambda *args: "add"

# MulOp → mul
NMulOp |= KW_mul, lambda *args: "mul"

# Expressions → ε | Expressions Expression
NExpressions |= lambda: []
NExpressions |= NExpressions, NExpression, lambda es, e: es + [e]

# ListCall → [ Construct Expressions ]
NListCall |= LBRACK, IDENT, NExpressions, RBRACK, safe_lambda(2, lambda *args: ListCallExpr(args[0], args[1]))

if __name__ == "__main__":
    p = pe.Parser(NProgram)
    assert p.is_lalr_one()

    p.add_skipped_domain(r'\s+')
    p.add_skipped_domain(r'<<[^\n]*>>')

    for filename in sys.argv[1:]:
        try:
            with open(filename, encoding="utf8") as f:
                tree = p.parse(f.read())
                pprint(tree)
        except pe.Error as e:
            print(f'Ошибка {e.pos}: {e.message}')
        except Exception as e:
            print(e)
