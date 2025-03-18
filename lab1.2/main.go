package main

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"regexp"
	"strings"
)

// IDENT ::= ([a-z] | [A-Z])([a-z] | [A-Z] | [0-9])*
// STRING_CONST ::= (STRING_SECTION)+
// STRING_SECTION ::= '([a-z] | [A-Z] | [0-9] | '' | space)+' | #([0-9])+
// Лексические домены: IDENT и STRING_CONST - не пересекаются

var (
	identRegex       = regexp.MustCompile(`^([A-Za-z])(\w)*`)
	stringConstRegex = regexp.MustCompile(`^('([^\n']|'')+'|#(\d)+)+`)
)

const (
	LexIdent  = "IDENT"
	LexString = "STRING"
	LexError  = "syntax error"
)

type Coordinate struct {
	line int
	pos  int
}

type Token struct {
	lex   string
	point Coordinate
	val   string
}

func (t Token) String() string {
	return fmt.Sprintf("%s (%d, %d): %s", t.lex, t.point.line, t.point.pos, t.val)
}

type lexer struct {
	program []string
	point   Coordinate
}

func (l *lexer) makeLexer(program []string) {
	l.program = program
	l.point = Coordinate{1, 1}
}

func (l *lexer) nextPos(step int) {
	newPos := l.point.pos + step
	if newPos > len(l.program[l.point.line-1]) {
		l.point.line++
		l.point.pos = 1
	} else {
		l.point.pos = newPos
	}
}

func (l *lexer) nextToken() (Token, error) {
	currentLine := l.program[l.point.line-1][l.point.pos-1:]
	line := strings.TrimLeft(currentLine, " ")
	countSpaces := len(currentLine) - len(line)
	l.point.pos += countSpaces
	stringConstPos := stringConstRegex.FindStringIndex(line)
	identPos := identRegex.FindStringIndex(line)

	var token Token
	var lexPos []int
	var lexType string

	if stringConstPos == nil && identPos == nil {
		return Token{}, errors.New("interrupted next token")
	} else if stringConstPos == nil {
		lexPos, lexType = identPos, LexIdent
	} else if identPos == nil {
		lexPos, lexType = stringConstPos, LexString
	}

	token = Token{lexType, Coordinate{l.point.line, l.point.pos}, line[:lexPos[1]]}
	l.nextPos(lexPos[1])
	return token, nil
}

func (l *lexer) recovery(endProgram Coordinate) (Token, error) {
	token, err := l.nextToken()
	for err != nil && l.point != endProgram {
		l.nextPos(1)
		if l.point != endProgram {
			token, err = l.nextToken()
		}
	}
	return token, err
}

func main() {
	var fileName string
	fmt.Scan(&fileName)
	file, _ := os.Open(fileName)
	defer file.Close()
	scanner := bufio.NewScanner(file)
	program := make([]string, 0)
	for scanner.Scan() {
		program = append(program, scanner.Text())
	}
	var l lexer
	l.makeLexer(program)
	endProgram := Coordinate{len(l.program) + 1, 1}
	for l.point != endProgram {
		token, err := l.nextToken()
		if err != nil {
			fmt.Printf("%s (%d, %d)\n", LexError, l.point.line, l.point.pos)
			token, err = l.recovery(endProgram)
		}
		if err == nil {
			fmt.Println(token)
		}
	}
}
