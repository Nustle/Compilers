flex -o lex.yy.cpp main.l
g++ -std=c++14 -o lexer *.cpp
./lexer < program.txt
rm *.cpp