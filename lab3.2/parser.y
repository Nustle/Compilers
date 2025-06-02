%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"
%}

%define api.pure
%locations

%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {long env[26]}
%parse-param {int space}
%parse-param {bool first_elem_in_rhs}
%parse-param {bool has_rep}
%parse-param {bool stitching}

%union {
    char* comment;
    char* ident;
}

%token CLASS TOKENS TYPES METHODS GRAMMAR AXIOM END_SPEC REP
%token EQUALS SEMICOLON COLON COMMA LPAREN RPAREN INDEX SLASH PIPE
%token <ident> IDENT

%{
int yylex(YYSTYPE *yylval_param, YYLTYPE *yylloc_param, yyscan_t scanner);
void yyerror(YYLTYPE *loc, yyscan_t scanner, long env[26], int space, bool first_elem_in_rhs, bool has_rep, bool stitching, const char *message);

void print_spaces(int space) {
    for(int i = 0; i < space; i++) {
        printf(" ");
    }
}
%}

%%

Spec:
    CLASS IDENT {
        printf("%%class\n");
        print_spaces(2);
        printf("%s\n\n", $2);
    }
    TOKENS {
        printf("%%tokens\n");
        print_spaces(2);
    } TokDef {
        printf("\n\n");
    }
    TYPES {
        printf("%%types\n");
        print_spaces(2);
    } TypesDef
    METHODS {
        printf("\n%%methods\n");
    } MethodsDef
    GRAMMAR {
        printf("\n%%grammar\n");
        space = 2;
    } GrammarDef
    AXIOM {
        printf("%%axiom\n");
    } AxiomPart
    END_SPEC {
        printf("%%end\n");
    }
    ;

TokDef:
    IDENT {
        printf("%s", $1);
    }
    | TokDef IDENT {
        printf(" %s", $2);
    }
    ;

TypesDef:
    TypeDecls COLON IDENT SEMICOLON {
        printf(": %s;\n", $3);
        print_spaces(2);
    }
    | TypesDef TypeDecls COLON IDENT SEMICOLON {
        printf(": %s;\n", $4);
        print_spaces(2);
    }
    ;

TypeDecls:
    IDENT {
        printf("%s", $1);
    }
    | TypeDecls COMMA IDENT {
        printf(", %s", $3);
    }
    ;

MethodsDef:
    Method
    | MethodsDef Method
    ;

Method:
    IDENT IDENT LPAREN {
        print_spaces(2);
        printf("%s %s(", $1, $2);
    } ParamsList RPAREN SEMICOLON {
        printf(");\n");
    }
    ;

ParamsList:
    /* ε */
    | Params
    ;

Params:
    TypeSpec
    | Params COMMA {
        printf(", ");
    } TypeSpec
    ;

TypeSpec:
    IDENT {
        printf("%s", $1);
    }
    | IDENT INDEX {
        printf("%s[]", $1);
    }
    ;

GrammarDef:
    /* ε */
    | GrammarDef GrammarRule
    ;

GrammarRule:
    IDENT EQUALS {
        print_spaces(space);
        printf("%s =\n", $1);
        space += 2;
        first_elem_in_rhs = true;
        stitching = false;
    } RHSList SEMICOLON {
        print_spaces(space);
        printf(";\n\n");
        space -= 2;
    }
    ;

RHSList:
    RHS {
        first_elem_in_rhs = false;
        space -= 2;
        printf("\n");
        stitching = true;
    }
    | RHSList PIPE RHS {
        first_elem_in_rhs = false;
        space -= 2;
        printf("\n");
        stitching = true;
    }
    ;

RHS:
    {
        print_spaces(space);
        if (first_elem_in_rhs) {
            printf("  ");
        } else {
            printf("| ");
        }
        space += 2;
    } Sequence ActionOpt
    ;

Sequence:
    /* ε */
    | Sequence Element
    ;

Element:
    RepOpt ElementCore
    ;

ElementCore:
    LPAREN {
        if (has_rep) {
            printf("(");
            has_rep = false;
        } else {
            if (!first_elem_in_rhs && stitching) {
                printf("\n");
                print_spaces(space);
            }
            printf("(");
        }
        printf("\n");
        space += 2;
        first_elem_in_rhs = true;
    } RHSList RPAREN {
        space -= 2;
        print_spaces(space);
        printf(")");
        printf("\n");

        stitching = false;
        print_spaces(space);
    }
    | IDENT {
        printf("%s ", $1);
        stitching = true;
    }
    ;

RepOpt:
    /* ε */
    | REP {
        if (stitching) {
            printf("\n");
            print_spaces(space);
        }
        printf("%%rep ");
        has_rep = true;
    }
    ;

ActionOpt:
    /* ε */
    | SLASH IDENT {
        printf("/ %s", $2);
    }
    ;

AxiomPart:
    IDENT {
        print_spaces(2);
        printf("%s\n\n", $1);
    }
%%

int main(int argc, char *argv[]) {
    FILE *input = 0;
    long env[26] = { 0 };
    int space = 0;
    bool first_elem_in_rhs = false;
    bool has_rep = false;
    bool stitching = false;
    yyscan_t scanner;
    struct Extra extra;

    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "Cannot open file %s\n", argv[1]);
            return 1;
        }
    } else {
        printf("No file in command line, use stdin\n");
        input = stdin;
    }

    init_scanner(input, &scanner, &extra);
    yyparse(scanner, env, space, first_elem_in_rhs, has_rep, stitching);
    destroy_scanner(scanner);

    if (input != stdin) {
        fclose(input);
    }

    return 0;
}