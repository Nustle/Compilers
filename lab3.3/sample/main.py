import sys
import parser_edsl as pe
from dataclasses import dataclass


class SemanticError(pe.Error):
    pass


class TypeRedefinition(SemanticError):
    def __init__(self, pos, typename):
        self.pos = pos
        self.typename = typename

    @property
    def message(self):
        return f'Переопределение типа {self.typename}'


class ConstructorRedefinition(SemanticError):
    def __init__(self, pos, constructor):
        self.pos = pos
        self.constructor = constructor

    @property
    def message(self):
        return f'Переопределение конструктора {self.constructor}'


class FunctionRedefinition(SemanticError):
    def __init__(self, pos, funcname):
        self.pos = pos
        self.funcname = funcname

    @property
    def message(self):
        return f'Переопределение функции {self.funcname}'


class FunctionNameMismatch(SemanticError):
    def __init__(self, pos, expected, actual):
        self.pos = pos
        self.expected = expected
        self.actual = actual

    @property
    def message(self):
        return f'Имя функции в образце {self.actual} не совпадает с именем в сигнатуре {self.expected}'


class ArgumentCountMismatch(SemanticError):
    def __init__(self, pos, expected, actual):
        self.pos = pos
        self.expected = expected
        self.actual = actual

    @property
    def message(self):
        return f'Количество аргументов ({self.actual}) не совпадает с сигнатурой ({self.expected})'


class RepeatedVariable(SemanticError):
    def __init__(self, pos, varname):
        self.pos = pos
        self.varname = varname

    @property
    def message(self):
        return f'Повторная переменная {self.varname} в образце'


class UnknownVariable(SemanticError):
    def __init__(self, pos, varname):
        self.pos = pos
        self.varname = varname

    @property
    def message(self):
        return f'Неизвестная переменная {self.varname}'


class UnknownType(SemanticError):
    def __init__(self, pos, typename):
        self.pos = pos
        self.typename = typename

    @property
    def message(self):
        return f'Неизвестный тип {self.typename}'


class UnknownConstructor(SemanticError):
    def __init__(self, pos, constructor):
        self.pos = pos
        self.constructor = constructor

    @property
    def message(self):
        return f'Неизвестный конструктор {self.constructor}'


class UnknownFunction(SemanticError):
    def __init__(self, pos, funcname):
        self.pos = pos
        self.funcname = funcname

    @property
    def message(self):
        return f'Неизвестная функция {self.funcname}'


class TypeMismatch(SemanticError):
    def __init__(self, pos, expected, actual):
        self.pos = pos
        self.expected = expected
        self.actual = actual

    @property
    def message(self):
        return f'Несоответствие типов: ожидался {self.expected}, получен {self.actual}'


# Type → ElementaryType | IDENT
@dataclass
class TypeNode:
    name: str
    pos: pe.Position = None

    @pe.ExAction
    def create(attrs, coords, res_coord):
        name = attrs[0]
        name_coord = coords[0] if len(coords) > 0 else res_coord
        return [TypeNode(name, name_coord)]

    def resolve(self, type_table):
        if self.name == 'int' or self.name == 'Int':
            return 'int'
        if self.name not in type_table:
            raise UnknownType(self.pos, self.name)
        return self.name


# AlgType → IDENT Types
@dataclass
class AlgType:
    construct: str
    construct_pos: pe.Position
    types: list[TypeNode]

    @pe.ExAction
    def create(attrs, coords, res_coord):
        construct, types = attrs[0], attrs[1]
        construct_coord = coords[0] if len(coords) > 0 else res_coord
        return AlgType(construct, construct_coord, types)

    def check(self, type_table):
        for t in self.types:
            t.resolve(type_table)


# Def → type IDENT : Alternatives .
@dataclass
class TypeDef:
    name: str
    name_pos: pe.Position
    alternatives: list[AlgType]

    @pe.ExAction
    def create(attrs, coords, res_coord):
        name, alternatives = attrs[0], attrs[1]
        name_coord = coords[1] if len(coords) > 1 else res_coord
        return TypeDef(name, name_coord, alternatives)

    def check(self, type_table):
        for alt in self.alternatives:
            alt.check(type_table)


# FunSignature → ( IDENT Types ) -> Type
@dataclass
class FunSignature:
    func_name: str
    func_pos: pe.Position
    arg_types: list[TypeNode]
    ret_type: TypeNode

    @pe.ExAction
    def create(attrs, coords, res_coord):
        name, arg_types, ret_type = attrs[0], attrs[1], attrs[2]
        name_coord = coords[1] if len(coords) > 1 else res_coord
        return FunSignature(name, name_coord, arg_types, ret_type[0] if isinstance(ret_type, list) else ret_type)

    def check(self, type_table):
        for t in self.arg_types:
            t.resolve(type_table)
        self.ret_type.resolve(type_table)


# Clause → FunctionPattern -> Expression
@dataclass
class Clause:
    pattern: "FunctionPattern"
    expr: "Expression"
    var_types: dict = None

    def check(self, signature, type_table, constructor_table, func_table):
        if self.pattern.func_name != signature.func_name:
            raise FunctionNameMismatch(self.pattern.func_pos, signature.func_name, self.pattern.func_name)

        if len(self.pattern.parameters) != len(signature.arg_types):
            raise ArgumentCountMismatch(self.pattern.func_pos, len(signature.arg_types), len(self.pattern.parameters))

        self.var_types = {}

        for param, expected_type in zip(self.pattern.parameters, signature.arg_types):
            param.collect_vars(expected_type.resolve(type_table), self.var_types, type_table, constructor_table)

        expr_type = self.expr.check_type(self.var_types, type_table, constructor_table, func_table)
        expected_ret_type = signature.ret_type.resolve(type_table)

        if expr_type != 'unknown' and expr_type != expected_ret_type:
            raise TypeMismatch(self.expr.get_pos(), expected_ret_type, expr_type)


# FunDef → fun FunSignature : Clauses .
@dataclass
class FunDef:
    signature: FunSignature
    clauses: list[Clause]

    @pe.ExAction
    def create(attrs, coords, res_coord):
        signature, clauses = attrs[0], attrs[1]
        return FunDef(signature, clauses)

    def check(self, type_table, constructor_table, func_table):
        self.signature.check(type_table)
        for clause in self.clauses:
            clause.check(self.signature, type_table, constructor_table, func_table)


# Program → Defs Funcs.
@dataclass
class Program:
    defs: list[TypeDef]
    funcs: list[FunDef]

    def check(self):
        type_table = {'int': 'int', 'Int': 'int'}
        constructor_table = {}
        func_table = {
            'add': (['int', 'int'], 'int'),
            'mul': (['int', 'int'], 'int')
        }

        for typedef in self.defs:
            if typedef.name in type_table:
                raise TypeRedefinition(typedef.name_pos, typedef.name)
            type_table[typedef.name] = typedef.name

        for typedef in self.defs:
            for alt in typedef.alternatives:
                if alt.construct in constructor_table:
                    raise ConstructorRedefinition(alt.construct_pos, alt.construct)
                constructor_table[alt.construct] = (typedef.name, alt.types)

        for typedef in self.defs:
            typedef.check(type_table)

        for func in self.funcs:
            if func.signature.func_name in func_table:
                raise FunctionRedefinition(func.signature.func_pos, func.signature.func_name)
            arg_types = [t.resolve(type_table) for t in func.signature.arg_types]
            ret_type = func.signature.ret_type.resolve(type_table)
            func_table[func.signature.func_name] = (arg_types, ret_type)

        for func in self.funcs:
            func.check(type_table, constructor_table, func_table)


# FuncParameter → IDENT | ListParam
class FuncParameter:
    def collect_vars(self, expected_type, var_types, type_table, constructor_table):
        raise NotImplementedError


@dataclass
class VarParam(FuncParameter):
    name: str
    pos: pe.Position = None

    @pe.ExAction
    def create(attrs, coords, res_coord):
        name = attrs[0]
        name_coord = coords[0] if len(coords) > 0 else res_coord
        return VarParam(name, name_coord)

    def collect_vars(self, expected_type, var_types, type_table, constructor_table):
        if self.name in var_types:
            raise RepeatedVariable(self.pos, self.name)
        var_types[self.name] = expected_type


@dataclass
class ListParam(FuncParameter):
    construct: str
    construct_pos: pe.Position
    parameters: list[FuncParameter]

    @pe.ExAction
    def create(attrs, coords, res_coord):
        construct, params = attrs[0], attrs[1]
        construct_coord = coords[1] if len(coords) > 1 else res_coord
        return ListParam(construct, construct_coord, params)

    def collect_vars(self, expected_type, var_types, type_table, constructor_table):
        if self.construct not in constructor_table:
            raise UnknownConstructor(self.construct_pos, self.construct)
        constr_type, arg_types = constructor_table[self.construct]
        if constr_type != expected_type:
            raise TypeMismatch(self.construct_pos, expected_type, constr_type)
        if len(self.parameters) != len(arg_types):
            raise ArgumentCountMismatch(self.construct_pos, len(arg_types), len(self.parameters))
        for param, arg_type in zip(self.parameters, arg_types):
            param.collect_vars(arg_type.resolve(type_table), var_types, type_table, constructor_table)


# FunctionPattern → ( IDENT FuncParameters )
@dataclass
class FunctionPattern:
    func_name: str
    func_pos: pe.Position
    parameters: list[FuncParameter]

    @pe.ExAction
    def create(attrs, coords, res_coord):
        name, params = attrs[0], attrs[1]
        name_coord = coords[1] if len(coords) > 1 else res_coord
        return FunctionPattern(name, name_coord, params)


# Expression → IDENT | Const | FuncCall | ListCall
class Expression:
    def check_type(self, var_types, type_table, constructor_table, func_table):
        raise NotImplementedError

    def get_pos(self):
        raise NotImplementedError


@dataclass
class VarExpr(Expression):
    name: str
    pos: pe.Position = None

    @pe.ExAction
    def create(attrs, coords, res_coord):
        name = attrs[0]
        name_coord = coords[0] if len(coords) > 0 else res_coord
        return VarExpr(name, name_coord)

    def check_type(self, var_types, type_table, constructor_table, func_table):
        if self.name in var_types:
            return var_types[self.name]
        else:
            return 'unknown'

    def get_pos(self):
        return self.pos


@dataclass
class ConstExpr(Expression):
    value: int
    pos: pe.Position = None

    @pe.ExAction
    def create(attrs, coords, res_coord):
        value = attrs[0]
        value_coord = coords[0] if len(coords) > 0 else res_coord
        return ConstExpr(value, value_coord)

    def check_type(self, var_types, type_table, constructor_table, func_table):
        return 'int'

    def get_pos(self):
        return self.pos


# FuncCall → ( Func Expressions )
@dataclass
class FuncCallExpr(Expression):
    func: str
    func_pos: pe.Position
    args: list[Expression]

    @pe.ExAction
    def create(attrs, coords, res_coord):
        func, args = attrs[0], attrs[1]
        func_coord = coords[1] if len(coords) > 1 else res_coord
        return FuncCallExpr(func, func_coord, args)

    def check_type(self, var_types, type_table, constructor_table, func_table):
        if self.func not in func_table:
            raise UnknownFunction(self.func_pos, self.func)

        arg_types, ret_type = func_table[self.func]

        if len(self.args) != len(arg_types):
            raise ArgumentCountMismatch(self.func_pos, len(arg_types), len(self.args))

        for arg, expected_type in zip(self.args, arg_types):
            actual_type = arg.check_type(var_types, type_table, constructor_table, func_table)
            if actual_type != 'unknown' and actual_type != expected_type:
                raise TypeMismatch(arg.get_pos(), expected_type, actual_type)

        return ret_type

    def get_pos(self):
        return self.func_pos


# ListCall → [ IDENT Expressions ]
@dataclass
class ListCallExpr(Expression):
    construct: str
    construct_pos: pe.Position
    args: list[Expression]

    @pe.ExAction
    def create(attrs, coords, res_coord):
        construct, args = attrs[0], attrs[1]
        construct_coord = coords[1] if len(coords) > 1 else res_coord
        return ListCallExpr(construct, construct_coord, args)

    def check_type(self, var_types, type_table, constructor_table, func_table):
        if self.construct not in constructor_table:
            raise UnknownConstructor(self.construct_pos, self.construct)

        constr_type, arg_types = constructor_table[self.construct]

        if len(self.args) != len(arg_types):
            raise ArgumentCountMismatch(self.construct_pos, len(arg_types), len(self.args))

        for arg, arg_type in zip(self.args, arg_types):
            expected_type = arg_type.resolve(type_table)
            actual_type = arg.check_type(var_types, type_table, constructor_table, func_table)
            if actual_type != 'unknown' and actual_type != expected_type:
                raise TypeMismatch(arg.get_pos(), expected_type, actual_type)

        return constr_type

    def get_pos(self):
        return self.construct_pos


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
NDef |= KW_type, IDENT, COLON, NAlternatives, DOT, TypeDef.create

# Alternatives → AlgType | Alternatives "|" AlgType
NAlternatives |= NAlgType, lambda alt: [alt]
NAlternatives |= NAlternatives, PIPE, NAlgType, lambda alts, alt: alts + [alt]

# AlgType → Construct Types
NAlgType |= IDENT, NTypes, AlgType.create

# Types → ε | Types Type
NTypes |= lambda: []
NTypes |= NTypes, NType, lambda ts, t: ts + t

# Type → ElementaryType | IDENT
NType |= NElemType, lambda elt: elt
NType |= IDENT, TypeNode.create

# ElementaryType → INT
NElemType |= KW_int, lambda _: [TypeNode('int')]

# Funcs → ε | Funcs FunDef
NFuncs |= lambda: []
NFuncs |= NFuncs, NFunDef, lambda fs, f: fs + [f]

# FunDef → fun FunSignature : Clauses .
NFunDef |= KW_fun, NFunSignature, COLON, NClauses, DOT, FunDef.create

# FunSignature → ( IDENT Types ) -> Type
NFunSignature |= LPAREN, IDENT, NTypes, RPAREN, ARROW, NType, FunSignature.create

# Clauses → Clause | Clauses "|" Clause
NClauses |= NClause, lambda c: [c]
NClauses |= NClauses, PIPE, NClause, lambda cs, c: cs + [c]

# Clause → FunctionPattern -> Expression
NClause |= NFunctionPattern, ARROW, NExpression, lambda p, e: Clause(p, e)

# FunctionPattern → ( IDENT FuncParameters )
NFunctionPattern |= LPAREN, IDENT, NFuncParameters, RPAREN, FunctionPattern.create

# FuncParameters → ε | FuncParameters FuncParameter
NFuncParameters |= lambda: []
NFuncParameters |= NFuncParameters, NFuncParameter, lambda ps, p: ps + [p]

# FuncParameter → IDENT | ListParam
NFuncParameter |= IDENT, VarParam.create
NFuncParameter |= NListParam, lambda lp: lp

# ListParam → [ Construct FuncParameters ]
NListParam |= LBRACK, IDENT, NFuncParameters, RBRACK, ListParam.create

# Expression → IDENT | Const | FuncCall | ListCall
NExpression |= IDENT, VarExpr.create
NExpression |= NConst, lambda const: const
NExpression |= NFuncCall, lambda fc: fc
NExpression |= NListCall, lambda lc: lc

# Const → INT_CONST
NConst |= INT_CONST, ConstExpr.create

# FuncCall → ( Func Expressions )
NFuncCall |= LPAREN, NFunc, NExpressions, RPAREN, FuncCallExpr.create

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
NListCall |= LBRACK, IDENT, NExpressions, RBRACK, ListCallExpr.create

if __name__ == "__main__":
    p = pe.Parser(NProgram)
    assert p.is_lalr_one()

    p.add_skipped_domain(r'\s+')
    p.add_skipped_domain(r'<<[^\n]*>>')

    for filename in sys.argv[1:]:
        try:
            with open(filename, encoding="utf8") as f:
                tree = p.parse(f.read())
                tree.check()
                print('Семантических ошибок не найдено')
        except pe.Error as e:
            print(f'Ошибка {e.pos}: {e.message}')
        except Exception as e:
            print(e)

