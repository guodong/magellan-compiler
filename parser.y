%{
  #include "node.h"
  NBlock* programBlock;
  extern int yylex();
  void yyerror(const char* s) {
    printf("yyerror: %s\n", s);
  }
%}

%union {
  NBlock* block;
  NStatement* stmt;
  NExpression* expr;
  NIdentifier* ident;
  NVariableDeclaration* var_decl;
  std::vector<NVariableDeclaration*>* varvec;
  std::vector<NExpression*>* exprvec;
  std::string* string;
  int token;
}

%token <string> TIDENTIFIER TINTEGER
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TEQUAL TIF TELSE TCEQ TCNE TCLT TCLE TCGT TCGE TPLUS TMINUS TMUL TDIV TRETURN TAND TOR TXOR TMOD TSHIFTR TSHIFTL TEXTERN

%type <ident> ident
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl if_stmt extern_decl
%type <expr> expr numeric
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <token> binary

%start program

%%

program : stmts {programBlock = $1; }
;

stmts : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
| stmts stmt { $1->statements.push_back($<stmt>2); }
;

stmt : var_decl
| func_decl
| extern_decl
| if_stmt
| expr
| TRETURN expr { $$ = new NReturnStatement(*$2); }
;

block : TLBRACE stmts TRBRACE { $$ = $2; }
| TLBRACE TRBRACE { $$ = new NBlock(); }
;

var_decl : ident ident { $$ = new NVariableDeclaration(*$1, *$2); }
| ident ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
;

func_decl : ident ident TLPAREN func_decl_args TRPAREN block { $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); }
;

func_decl_args : /*blank*/ { $$ = new VariableList(); }
| var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
| func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }

expr : numeric
| ident { $<ident>$ = $1; }
| TLPAREN expr TRPAREN { $$ = $2; }
| expr binary expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
| ident TEQUAL expr { $$ = new NAssignment(*$<ident>1, *$3); }
| ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
;

numeric : TINTEGER { $$ = new NInteger(atol($1->c_str())); }
;

if_stmt : TIF expr block { $$ = new NIfStatement(*$2, *$3); }
| TIF expr block TELSE block { $$ = new NIfStatement(*$2, *$3, *$5); }
;

binary : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE | TPLUS | TMINUS | TMUL | TDIV | TAND | TOR | TXOR | TSHIFTL | TSHIFTR
;

extern_decl : TEXTERN ident ident TLPAREN func_decl_args TRPAREN { $$ = new NExternDeclaration(*$2, *$3, *$5); delete $5; }
;

call_args : /*blank*/  { $$ = new ExpressionList(); }
| expr { $$ = new ExpressionList(); $$->push_back($1); }
| call_args TCOMMA expr  { $1->push_back($3); }
;

%%
