%{ 
#include "ParsingTree.h"
#include "SymbolTable.h"

extern int yylineno;
extern int yylex();
extern int yychar;
extern ParsingTree pt;
extern SymbolTable symbolTable;

void yyerror(char* out,const char *s) 
{  
	ofstream fout(out);
	fout<<"Error.";
	std::cerr<<"\nline "<<yylineno<<": "<<s<<".\n";  
	fout.close();
	exit(1); 
}

%}

%union{ 
	TreeNode* node;
}

%define parse.error verbose
%define parse.lac full

%parse-param { char* out }

%token <node> CONST
%token <node> ID
%token IF FOR WHILE DO BREAK CONTINUE RETURN
%nonassoc IFX
%nonassoc ELSE
%left COMMA
%right ASSIGN DIVASSIGN MULASSIGN MODASSIGN PLUSASSIGN MINUSASSIGN SLASSIGN SRASSIGN ANDASSIGN ORASSIGN XORASSIGN
%left LOR
%left LAND
%left BOR
%left XOR
%left BAND
%left EQ NE
%left GT GE LT LE
%left SL SR
%left PLUS MINUS
%left DIV MUL MOD
%right LNOT BINVERSION
%nonassoc UMINUS
%right TYPE INC DEC
%left LP RP LB RB LC RC DOT
%token SEMI STRUCT TYPE_INT TYPE_CHAR TYPE_FLOAT TYPE_DOUBLE STRING_LITERAL

%type <node> program extdefs extvars extdef spec stspec opttag
%type <node> var paras func para defs def init decs
%type <node> stmt stmts stmtblock arrs args
%type <node> dec
%type <node> exp fexp

%start program

%%
program:	extdefs				{ $$ = pt.link("program",1,$1);pt.root=$$; }
			;

extdefs:	extdef extdefs		{ $$ = pt.link("extdefs",2,$1,$2); }
			|					{ $$ = pt.link("no_def",0); }
			;

extvars:	dec					{ $$ = pt.link("dec",1,$1); }
			| dec COMMA extvars	{ $$ = pt.link("decs",2,$1,$3); }
			|					{ $$ = pt.link("no_dec",0); }
			;

extdef:		spec extvars SEMI	{ $$ = pt.link("def_var",2,$1,$2); }
			| spec func stmtblock	{ $$ = pt.link("def_func",3,$1,$2,$3); }
			;

spec:		TYPE_INT			{ $$ = pt.link("type_int",0);$$->type = "int"; }
			| stspec			{ $$ = pt.link("struct",1,$1);$$->type = "struct"; }
			;

stspec:		STRUCT opttag LC defs RC	{ $$ = pt.link("struct_spec",2,$2,$4); }
			| STRUCT ID			{ $$ = pt.link("struct",1,$2); }
			;

opttag:		ID					{ $$ = pt.link("structID",1,$1);$1->type = "struct"; }
			|					{ $$ = pt.link("no_tag",0); }
			;

var:		ID					{ $$ = pt.link("ID",0);$$->name = $1->name;$$->type = $1->type; }
			| var LB CONST RB	{ $$ = pt.link("array",2,$1,$3);$$->name = $1->name;$$->type = $1->type; }
			;

func:		ID LP paras RP		{ $$ = pt.link("func",2,$1,$3); }
			;

paras:		para COMMA paras	{ $$ = pt.link("paras",2,$1,$3); }
			| para				{ $$ = pt.link("para",1,$1); }
			|					{ $$ = pt.link("no_para",0); }
			;

para:		spec var			{ $$ = pt.link("spec_var",2,$1,$2); }
			;

stmtblock:	LC defs stmts RC	{ $$ = pt.link("stmtblock",2,$2,$3); }			
			;

stmts:		stmt stmts			{ $$ = pt.link("stmts",2,$1,$2); }
			|					{ $$ = pt.link("no_stmt",0); }
			;

stmt:		 IF LP exp RP stmt %prec IFX		
								{ $$ = pt.link("if",2,$3,$5); }
			| IF LP exp RP stmt ELSE stmt	
								{ $$ = pt.link("if_else",3,$3,$5,$7); }
			| fexp SEMI			{ $$ = pt.link("fexp",1,$1); }
			| stmtblock			{ $$ = pt.link("stmtblock",1,$1); }
			| RETURN exp SEMI	{ $$ = pt.link("return",1,$2); }
			| FOR LP fexp SEMI fexp SEMI fexp RP stmt 	{ $$ = pt.link("for",4,$3,$5,$7,$9); }
			| CONTINUE SEMI		{ $$ = pt.link("continue",0); }
			| BREAK SEMI		{ $$ = pt.link("break",0); }
			;

defs:		def defs			{ $$ = pt.link("defs",2,$1,$2); }
			|					{ $$ = pt.link("no_def",0); }
			;

def:		spec decs SEMI		{ $$ = pt.link("def",2,$1,$2); }
			;

decs:		dec COMMA decs		{ $$ = pt.link("decs",2,$1,$3); }
			| dec				{ $$ = pt.link("dec",1,$1); }
			;

dec:		var					{ $$ = pt.link("dec",1,$1); }
			| var ASSIGN init	{ $$ = pt.link("dec_init",2,$1,$3); }
			;

init:		exp					{ $$ = pt.link("exp",1,$1); }
			| LC args RC		{ $$ = pt.link("set_init",1,$2); }
			;

exp:		 CONST				{ 	
									$$ = pt.link("CONST",0);
									$$->type = $1->type;
									$$->iVal = $1->iVal;
								}
			| LP exp RP			{ 
									$$ = $2;
									/*
									if($2->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $2->iVal;
									}
									else
										$$ = pt.link("exp",1,$2);
									*/
								}
			| ID LP args RP		{ $$ = pt.link("func_call",2,$1,$3); }
			| ID 				{ $$ = pt.link("ID",0); $$->name = $1->name;}
			| ID LB arrs		{ $$ = pt.link("array",2,$1,$3);$$->name = $1->name; }
			| exp DOT ID		{ $$ = pt.link("member",2,$1,$3); }
			| exp ASSIGN exp	{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal = $3->iVal;
									}
									else 
										$$ = pt.link("assign",2,$1,$3);
								}
			| exp PLUS exp		{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal + $3->iVal;
									}
									else
										$$ = pt.link("plus",2,$1,$3);
								}
			| MINUS exp %prec UMINUS
								{ 
									if($2->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = -$2->iVal;
									}
									else
										$$ = pt.link("negetive",1,$2);
								}
			| exp MINUS exp		{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal - $3->iVal;
									}
									else
										$$ = pt.link("minus",2,$1,$3);
								}
			| exp MUL exp		{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal * $3->iVal;
									}
									else
										$$ = pt.link("mul",2,$1,$3);
								}
			| exp DIV exp		{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										if($3->iVal == 0)
											cerr<<"line "<<$3->lineno<<": Divided by 0!\n";
										else
										{
											$$ = pt.link("CONST",0);
											$$->iVal = $1->iVal / $3->iVal;
										}
									}
									else
										$$ = pt.link("div",2,$1,$3);
								}
			| exp MOD exp		{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										if($3->iVal == 0)
										{
											$$ = pt.link("modulo",2,$1,$3);
											cerr<<"line "<<$3->lineno<<": Divided by 0!\n";
										}	
										else
										{
											$$ = pt.link("CONST",0);
											$$->iVal = $1->iVal % $3->iVal;
										}
									}
									else 
										$$ = pt.link("mod",2,$1,$3);
								}
			| exp PLUSASSIGN exp
								{ 
									$$ = pt.link("plusassign",2,$1,$3);
								}
			| exp MINUSASSIGN exp 
								{ 
									$$ = pt.link("minusassign",2,$1,$3);
								}
			| exp MULASSIGN exp	{ 
									$$ = pt.link("mulassign",2,$1,$3);
								}
			| exp DIVASSIGN exp {
									if($3->node_type == "CONST" && $3->iVal == 0) 
										cerr<<"Line "<<$3->lineno<<": Divided by 0!\n";
									$$ = pt.link("divassign",2,$1,$3);
								}
			| exp MODASSIGN exp	{ 	
									if($3->node_type == "CONST" && $3->iVal == 0) 
										cerr<<"Line "<<$3->lineno<<": Divided by 0!\n";
									$$ = pt.link("modassign",2,$1,$3);
								}
			| BINVERSION exp 	{ 
									if($2->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = ~$2->iVal;
									}
									else
										$$ = pt.link("inversion",2,$2);
								}
			| LNOT exp			{ 
									if($2->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = !$2->iVal;
									}
									else
										$$ = pt.link("logic_not",1,$2);
								}
			| INC exp			{ 
									if($2->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $2->iVal + 1;
									}
									else
										$$ = pt.link("inc",1,$2);
								}
			| DEC exp			{ 
									if($2->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $2->iVal - 1;
									}
									else
										$$ = pt.link("dec",1,$2);
								 }
			| exp SL exp		{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal << $3->iVal;
									}
									else
										$$ = pt.link("shift_left",2,$1,$3);
								}
			| exp SR exp		{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal >> $3->iVal;
									}
									else
									$$ = pt.link("shift_right",2,$1,$3);
								}
			| exp LT exp 		{
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = ($1->iVal < $3->iVal);
									}
									else 
										$$ = pt.link("less_than",2,$1,$3);
								}
			| exp GT exp		{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = ($1->iVal > $3->iVal);
									}
									else
										$$ = pt.link("greater_than",2,$1,$3);
								} 
			| exp EQ exp		{

									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = ($1->iVal == $3->iVal);
									}
									else 	
										$$ = pt.link("equal",2,$1,$3);
								 }
			| exp NE exp		{ 
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = ($1->iVal != $3->iVal);
									}
									else
										$$ = pt.link("not_equal",2,$1,$3);
								}
			| exp LE exp		{ 	
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = ($1->iVal <= $3->iVal);
									}
									else
										$$ = pt.link("less_equal",2,$1,$3);
								}
			| exp GE exp		{ 	
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = ($1->iVal >= $3->iVal);
									}
									else
										$$ = pt.link("greater_equal",2,$1,$3);
								}
			| exp BOR exp		{ 	
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal | $3->iVal;
									}
									else
										$$ = pt.link("bit_or",2,$1,$3);
								 }
			| exp BAND exp		{ 	
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal & $3->iVal;
									}
									else
										$$ = pt.link("bit_and",2,$1,$3);
								}
			| exp XOR exp		{ 	
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal ^ $3->iVal;
									}
									else
										$$ = pt.link("xor",2,$1,$3);
								}
			| exp LAND exp		{ 	
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal && $3->iVal;
									}
									else
										$$ = pt.link("logic_and",2,$1,$3);
								 }
			| exp LOR exp		{ 	
									if($1->node_type == "CONST" && $3->node_type == "CONST")
									{
										$$ = pt.link("CONST",0);
										$$->iVal = $1->iVal || $3->iVal;
									}
									else
									$$ = pt.link("logic_or",2,$1,$3);
								}
			| exp ANDASSIGN exp	{
								 	if($1->node_type != "ID")
										cerr<<"Line "<<$1->lineno<<": Cannot assign to an expression!\n";
									$$ = pt.link("and_assign",2,$1,$3);
								}
			| exp ORASSIGN exp	{	
									if($1->node_type != "ID")
										cerr<<"Line "<<$1->lineno<<": Cannot assign to an expression!\n";
									$$ = pt.link("or_assign",2,$1,$3);
								}
			| exp XORASSIGN exp	{ 	
									if($1->node_type != "ID")
										cerr<<"Line "<<$1->lineno<<": Cannot assign to an expression!\n";
									$$ = pt.link("xor_assign",2,$1,$3);
								}
			| exp SLASSIGN exp	{
									if($1->node_type != "ID")
										cerr<<"Line "<<$1->lineno<<": Cannot assign to an expression!\n";
									$$ = pt.link("shift_left_assign",2,$1,$3);
								}
			| exp SRASSIGN exp	{
									if($1->node_type != "ID")
										cerr<<"Line "<<$1->lineno<<": Cannot assign to an expression!\n";
									$$ = pt.link("shift_right_assign",2,$1,$3);
								}
			;
	
fexp:		exp					{ $$ = pt.link("exp",1,$1); }
			|					{ $$ = pt.link("no_expr",0); }
			;
			
arrs:		 exp RB LB arrs		{ $$ = pt.link("arrs",2,$1,$4); }
			| exp RB			{ $$ = pt.link("arr",1,$1); }
			;

args:		exp COMMA args		{ $$ = pt.link("args",2,$1,$3); }
			| exp				{ $$ = pt.link("arg",1,$1); }
			| 					{ $$ = pt.link("no_arg",0); }
			;

%%
