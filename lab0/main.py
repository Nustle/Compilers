class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.pos = 0

    def peek(self):
        if self.pos < len(self.tokens):
            return self.tokens[self.pos]
        return None

    def consume(self, expected=None):
        token = self.peek()
        if token is None:
            raise Exception("unexpected end")
        if expected is not None and token != expected:
            raise Exception(f"expected '{expected}'; given '{token}' in pos {self.pos})")
        self.pos += 1
        return token

    def parse_program(self):
        articles = self.parse_articles()
        body = self.parse_body(stop_tokens=set())
        if self.peek() is not None:
            raise Exception("tokens after parsing")
        return [articles, body]

    def parse_articles(self):
        articles = {}
        while self.peek() == "define":
            name, body = self.parse_article()
            if name in articles:
                raise Exception(f"article '{name}' was declared")
            articles[name] = body
        return articles

    def parse_article(self):
        self.consume("define")
        token = self.peek()
        if token is None or token in {"define", "end", "if", "endif", "while", "do", "wend"}:
            raise Exception("incorrect word in article")
        name = token
        self.consume()
        body = self.parse_body(stop_tokens={"end"})
        self.consume("end")
        return name, body

    def parse_body(self, stop_tokens):
        statements = []
        while True:
            token = self.peek()
            if token is None or token in stop_tokens:
                break
            statement = self.parse_statement()
            statements.append(statement)
        return statements

    def parse_statement(self):
        token = self.peek()
        if token is None:
            raise Exception("unexpected end")
        if token == "if":
            self.consume("if")
            if_body = self.parse_body(stop_tokens={"endif"})
            self.consume("endif")
            return ["if", if_body]
        elif token == "while":
            self.consume("while")
            cond_body = self.parse_body(stop_tokens={"do"})
            self.consume("do")
            loop_body = self.parse_body(stop_tokens={"wend"})
            self.consume("wend")
            return ["while", cond_body, loop_body]
        else:
            tok = self.consume()
            if tok.isdigit():
                tok = int(tok)
            return tok


def parse(input_str):
    tokens = input_str.split()
    try:
        parser = Parser(tokens)
        ast = parser.parse_program()
        return ast
    except Exception as e:
        print(e)
        return None


if __name__ == "__main__":
    code = input()
    result = parse(code)
    print(result)
