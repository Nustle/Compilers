package main

import (
	"fmt"
	"os"
)

const (
	Space = iota
	Ident
	Number
	Keyword
	Operation
	Comment
	EOF
)

var domainTags = [7]string{"SPACE", "IDENT", "NUMBER", "KEYWORD", "OPERATION", "COMMENT", "EOF"}

type DomainTag struct {
	index int
}

func (d DomainTag) Val() string {
	return domainTags[d.index]
}

type Position struct {
	program string
	line    int
	pos     int
	index   int
}

func (p *Position) String() string {
	return fmt.Sprintf("(%d, %d)", p.line, p.pos)
}

func (p *Position) Cp() string {
	if p.index == len(p.program) {
		return "EOF"
	}
	return string(p.program[p.index])
}

func (p *Position) peek() string {
	if p.index+1 == len(p.program) {
		return "EOF"
	}
	return string(p.program[p.index+1])
}

func (p *Position) IsNewLine() bool {
	if p.index == len(p.program) {
		return true
	}
	if p.Cp() == "\r" && p.index+1 < len(p.program) {
		return p.peek() == "\n"
	}
	return p.Cp() == "\n"
}

func (p *Position) advance() {
	if p.index < len(p.program) {
		if p.IsNewLine() {
			if p.Cp() == "\r" {
				p.index++
			}
			p.line++
			p.pos = 1
		} else {
			p.pos++
		}
		p.index++
	}
}

func (p *Position) deleteSpaces() {
	for p.IsWhiteSpace() {
		p.advance()
	}
}

func (p *Position) next() string {
	isPrevNewLine := p.IsNewLine()
	p.advance()
	if isPrevNewLine {
		p.deleteSpaces()
	}
	return p.Cp()
}

func (p *Position) IsDigit() bool {
	if p.Cp() == "EOF" {
		return false
	}
	return p.Cp()[0] >= '0' && p.Cp()[0] <= '9'
}

func (p *Position) IsLetter() bool {
	if p.Cp() == "EOF" {
		return false
	}
	c := p.Cp()[0]
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
}

func (p *Position) IsLetterOrDigit() bool {
	if p.Cp() == "EOF" {
		return false
	}
	return p.IsLetter() || p.IsDigit()
}

func (p *Position) IsWhiteSpace() bool {
	if p.Cp() == "EOF" {
		return false
	}
	return p.Cp() == " "
}

type Fragment struct {
	starting  Position
	following Position
}

func (f Fragment) String() string {
	return fmt.Sprintf("%s-%s", f.starting.String(), f.following.String())
}

type Token struct {
	tag      DomainTag
	fragment Fragment
	attr     string
}

func (t Token) String() string {
	return fmt.Sprintf("%s %s: %s", t.tag.Val(), t.fragment.String(), t.attr)
}

type LexRecognizer struct {
	curr  int
	table [18][15]int
	final map[int]DomainTag
}

func NewLexRecognizer() *LexRecognizer {
	return &LexRecognizer{
		curr: 0,
		table: [18][15]int{
			// Digit, w, e, n, d, h, i, l, Letter\{w, e, n, d, h, i, l}, Space, \n, -, <, =, Any\{prev}
			/** 0 **/ {11, 2, 1, 1, 1, 1, 1, 1, 1, 10, 17, 14, 12, -1, -1},
			/** 1 **/ {1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1},
			/** 2 **/ {1, 1, 3, 1, 1, 4, 1, 1, 1, -1, -1, -1, -1, -1, -1},
			/** 3 **/ {1, 1, 1, 5, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1},
			/** 4 **/ {1, 1, 1, 1, 1, 1, 6, 1, 1, -1, -1, -1, -1, -1, -1},
			/** 5 **/ {1, 1, 1, 1, 7, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1},
			/** 6 **/ {1, 1, 1, 1, 1, 1, 1, 8, 1, -1, -1, -1, -1, -1, -1},
			/** 7 **/ {1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1},
			/** 8 **/ {1, 1, 9, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1},
			/** 9 **/ {1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1},
			/** 10 **/ {-1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 17, -1, -1, -1, -1},
			/** 11 **/ {11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			/** 12 **/ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, -1},
			/** 13 **/ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			/** 14 **/ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, -1, -1, -1},
			/** 15 **/ {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 15, 15, 15, 15},
			/** 16 **/ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			/** 17 **/ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		},
		final: map[int]DomainTag{
			1:  DomainTag{Ident},
			2:  DomainTag{Ident},
			3:  DomainTag{Ident},
			4:  DomainTag{Ident},
			5:  DomainTag{Ident},
			6:  DomainTag{Ident},
			7:  DomainTag{Keyword},
			8:  DomainTag{Ident},
			9:  DomainTag{Keyword},
			10: DomainTag{Space},
			11: DomainTag{Number},
			12: DomainTag{Operation},
			13: DomainTag{Operation},
			16: DomainTag{Comment},
			17: DomainTag{Space},
		},
	}
}

func (r *LexRecognizer) nextState(l *Lexer) {
	switch {
	case l.curr.IsDigit():
		r.curr = r.table[r.curr][0]
	case l.curr.Cp() == "w":
		r.curr = r.table[r.curr][1]
	case l.curr.Cp() == "e":
		r.curr = r.table[r.curr][2]
	case l.curr.Cp() == "n":
		r.curr = r.table[r.curr][3]
	case l.curr.Cp() == "d":
		r.curr = r.table[r.curr][4]
	case l.curr.Cp() == "h":
		r.curr = r.table[r.curr][5]
	case l.curr.Cp() == "i":
		r.curr = r.table[r.curr][6]
	case l.curr.Cp() == "l":
		r.curr = r.table[r.curr][7]
	case l.curr.IsLetter():
		r.curr = r.table[r.curr][8]
	case l.curr.IsWhiteSpace():
		r.curr = r.table[r.curr][9]
	case l.curr.IsNewLine():
		r.curr = r.table[r.curr][10]
	case l.curr.Cp() == "-":
		r.curr = r.table[r.curr][11]
	case l.curr.Cp() == "<":
		r.curr = r.table[r.curr][12]
	case l.curr.Cp() == "=":
		r.curr = r.table[r.curr][13]
	default:
		r.curr = r.table[r.curr][14]
	}
}

type Lexer struct {
	program    string
	curr       Position
	messages   map[Position]string
	recognizer *LexRecognizer
}

func NewLexer(program string, recognizer *LexRecognizer) *Lexer {
	return &Lexer{
		program:    program,
		curr:       Position{program, 1, 1, 0},
		messages:   make(map[Position]string),
		recognizer: recognizer,
	}
}

func (l *Lexer) nextToken() Token {
	for l.curr.Cp() != "EOF" {
		var finalPos Position
		var finalTag DomainTag
		wasFinal := false
		starting := l.curr
		l.recognizer.curr = 0
		l.recognizer.nextState(l)
		for l.recognizer.curr != -1 {
			if tag, isFinal := l.recognizer.final[l.recognizer.curr]; isFinal {
				finalPos = l.curr
				finalTag = tag
				wasFinal = true
			}
			isNewLine := l.curr.IsNewLine()
			if isNewLine {
				l.recognizer.nextState(l)
			}
			l.curr.next()
			if !isNewLine {
				l.recognizer.nextState(l)
			}
		}
		if !wasFinal {
			l.messages[l.curr] = "unexpected char"
			// если встретили перенос строки, deleteSpaces. next уже учитывает это
			l.curr.next()
		} else {
			finalPos.advance()
			return Token{
				tag:      finalTag,
				fragment: Fragment{starting: starting, following: finalPos},
				attr:     l.program[starting.index:finalPos.index],
			}
		}
	}
	return Token{
		tag:      DomainTag{EOF},
		fragment: Fragment{l.curr, l.curr},
		attr:     "",
	}
}

func main() {
	data, _ := os.ReadFile("program.txt")
	program := string(data)
	recognizer := NewLexRecognizer()
	lexer := NewLexer(program, recognizer)
	lexer.curr.deleteSpaces()

	token := lexer.nextToken()
	for token.tag.index != EOF {
		fmt.Println(token)
		token = lexer.nextToken()
	}
	fmt.Println(token)
	fmt.Println("Messages")
	for pos, text := range lexer.messages {
		fmt.Printf("%s: %s\n", pos.String(), text)
	}
}
