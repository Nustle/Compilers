%{
   #include <iostream>
   #include <fstream>
   #include <string>
   #include <map>

   #define TAG_EOF 0
   #define TAG_IDENT 1
   #define TAG_NUMBER 2
   #define TAG_KEYWORD 3

   using namespace std;

   string tag_names[] = {
           "END_OF_PROGRAM", "IDENT", "NUMBER", "KEYWORD"
   };

   typedef struct Position Position;
   struct Position {
       int line, pos, index;
   };

   map<Position, string> errors;
   vector<string> idents;
   map<string, int> ident_codes;

   int AddIdent (const string& ident) {
       auto ident_code = ident_codes.find(ident);
       if (ident_code != ident_codes.end()) {
           return ident_code->second;
       } else {
           int code = (int)idents.size();
           idents.push_back(ident);
           ident_codes[ident] = code;
           return code;
       }
   }

   bool operator<(const Position &first, const Position &second) {
       return first.index < second.index;
   }

   void AddMessage(const Position &c, const string& text) {
       errors[c] = text;
   }

   void print_pos(Position *p) {
       cout << "(" << p->line<< "," << p->pos << ")";
   }

   struct Fragment {
       Position starting, following;
   };

   typedef struct Fragment YYLTYPE;

   void print_frag(struct Fragment* f) {
       print_pos(&(f->starting));
       cout << "-";
       print_pos(&(f->following));
   }

   struct Token {
       long num;
       bool is_hex;
       string ch;

       Token() {
           num=0;
           ch="";
           is_hex=false;
       }
   };

   typedef struct Token YYSTYPE;

   int continued;
   struct Position cur;
   struct Position start;

   #define YY_USER_ACTION {                 \
           int i;                           \
           if (!continued)                  \
               yylloc->starting = cur;      \
               start = cur;                 \
           continued = 0;                   \
                                            \
           for (i = 0; i < yyleng; i++) {   \
               if (yytext[i] == '\n') {     \
                   cur.line++;              \
                   cur.pos = 1;             \
               }                            \
               else                         \
                   cur.pos++;               \
               cur.index++;                 \
           }                                \
           yylloc->following = cur;         \
       }

   void init_scanner (const char * program){
       continued = 0;
       cur.line = 1;
       cur.pos = 1;
       cur.index = 0;
       yy_scan_string(program);
   }

   void err (Position p, const string& msg){
       cout << "Error ";
       print_pos(&p);
       cout << ": " << msg << endl;
   }
%}

%option noyywrap bison-bridge bison-locations

DECIMAL     [0-9]+
HEX         [0-9][0-9A-Fa-f]*h
IDENT       [A-Za-z][A-Za-z0-9]*

%%

[\n\t ]+


{HEX} {
    string lex(yytext);
    lex.pop_back(); 
    yylval->num = strtol(lex.c_str(), NULL, 16);
    yylval->is_hex = true;
    yylval->ch = lex;
    return TAG_NUMBER;
}

{DECIMAL} {
    yylval->num = atoi(yytext);
    yylval->is_hex = false;
    return TAG_NUMBER;
}

{IDENT} {
    string lex(yytext);
    string lower;
    for (int i = 0; i < yyleng; i++) {
        lower.push_back(tolower(yytext[i]));
    }
    if (lower == "mov" || lower == "eax") {
        yylval->ch = lex;
        return TAG_KEYWORD;
    }
    int code = AddIdent(lex);
    yylval->ch = lex;
    yylval->num = code;
    return TAG_IDENT;
}

. {
    string msg = string("unknown symbol ") + string(1, yytext[0]);
    AddMessage(start, msg);
}

<<EOF>> {
    yylloc->starting = cur;
    yylloc->following = cur;
    return TAG_EOF;
}    

%%

int main(){
    ifstream file("/Users/adilismailov/Desktop/ИУ9-62Б/Компиляторы/lab1.5/program.txt");
    string programText((istreambuf_iterator<char>(file)),
                      (istreambuf_iterator<char>()));
    file.close();

    init_scanner(programText.c_str());

    YYSTYPE value;
    YYLTYPE coords;
    int tag = yylex(&value, &coords);

    while (tag != TAG_EOF){
        print_frag(&coords);
        cout << " " << tag_names[tag] << " ";

        if (tag == TAG_NUMBER) {
            if (value.is_hex) {
                cout << hex << value.num << dec << endl;
            } else {
                cout << value.num << endl;
            }
        }

        if (tag == TAG_IDENT) {
            cout << value.num << endl;
        }

        if (tag == TAG_KEYWORD) {
            cout << value.ch << endl;
        }

       tag = yylex(&value,&coords);
    }
    print_frag(&coords);
    cout << " " << tag_names[tag] << " " << endl;
    
    cout << "----" << endl;
    for (const auto& error : errors) {
        err(error.first, error.second);
    }
    return 0;
}