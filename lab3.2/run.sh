#!/bin/bash
set -e

export PATH="$(brew --prefix bison)/bin:$(brew --prefix flex)/bin:$PATH"
export LDFLAGS="-L$(brew --prefix flex)/lib"

rm -f parser.tab.c parser.tab.h lex.yy.c

bison -d parser.y
flex --reentrant --bison-bridge lexer.l

gcc -o lab parser.tab.c lex.yy.c -lfl $LDFLAGS

rm -f parser.tab.c parser.tab.h lex.yy.c

./lab input.txt >> output.txt