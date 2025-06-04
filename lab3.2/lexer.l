%option reentrant noyywrap bison-bridge bison-locations noinput nounput
%option extra-type="struct Extra *"

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.tab.h"

#define YY_USER_ACTION \
  { \
    int i; \
    struct Extra *extra = yyextra; \
    if (! extra->continued ) { \
      yylloc->first_line = extra->cur_line; \
      yylloc->first_column = extra->cur_column; \
    } \
    extra->continued = false; \
    for (i = 0; i < yyleng; ++i) { \
      if (yytext[i] == '\n') { \
        extra->cur_line += 1; \
        extra->cur_column = 1; \
      } else { \
        extra->cur_column += 1; \
      } \
    } \
    yylloc->last_line = extra->cur_line; \
    yylloc->last_column = extra->cur_column; \
  }

  void yyerror(YYLTYPE *loc, yyscan_t scanner, int space, bool first_elem_in_rhs, bool has_rep, bool stitching, const char *message) {
      printf("Error (%d,%d): %s\n", loc->first_line, loc->first_column, message);
  }

%}

DIGIT    [0-9]
LETTER   [a-zA-Z_]
IDENT    {LETTER}({LETTER}|{DIGIT})*
NUMBER   {DIGIT}+(\.[0-9]+)?

%%

[\n\t ]+

"$"[^\n]*

\%class    return CLASS;
\%tokens   return TOKENS;
\%types    return TYPES;
\%methods  return METHODS;
\%grammar  return GRAMMAR;
\%axiom    return AXIOM;
\%end      return END_SPEC;
\%rep      return REP;

\=         return EQUALS;
\;         return SEMICOLON;
\:         return COLON;
\,         return COMMA;
\(         return LPAREN;
\)         return RPAREN;
\[]        return INDEX;
\/         return SLASH;
\|         return PIPE;

{IDENT} {
yylval->ident = strdup(yytext);
    return IDENT;
}

%%

void init_scanner(FILE *input, yyscan_t *scanner, struct Extra *extra) {
    extra->continued = false;
    extra->cur_line = 1;
    extra->cur_column = 1;

    yylex_init(scanner);
    yylex_init_extra(extra, scanner);
    yyset_in(input, *scanner);
}

void destroy_scanner(yyscan_t scanner) {
    yylex_destroy(scanner);
}