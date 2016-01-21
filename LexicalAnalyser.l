%{
#include "ParsingTree.h"
#include "SymbolTable.h"
#include "y.tab.hpp"
#include <memory>
#include <cstdlib>

#ifdef DEBUG
#define MYECHO ECHO
#else
#define MYECHO
#endif

extern SymbolTable symbolTable;
void check_dec();
void check_hex();
void check_oct();
void check_id();
void int_error();

%}

DIGIT 	[0-9]
HEX 	[a-fA-F0-9]
LETTER	[a-zA-Z]
OCT 	[0-7]

%option yylineno

%%
"int"		{MYECHO;return(TYPE_INT);}
"struct"	{MYECHO;return(STRUCT);}
"return" 	{MYECHO;return(RETURN);}
"if"		{MYECHO;return(IF);}
"else"		{MYECHO;return(ELSE);}
"for"		{MYECHO;return(FOR);}
"break"		{MYECHO;return(BREAK);}
"continue"	{MYECHO;return(CONTINUE);}
[ \t\n]		{MYECHO;}
[1-9][0-9]*	{MYECHO;check_dec();return(CONST);}
0[xX]{HEX}+	{MYECHO;check_hex();return(CONST);}
0[xX]{HEX}*([g-zG-Z]{HEX}?)+{HEX}* {MYECHO;int_error();return(CONST);}
0{OCT}*		{MYECHO;check_oct();return(CONST);}
0{OCT}*([89]{OCT}?)+{OCT}*			{MYECHO;int_error();return(CONST);}
({LETTER}|"_")+({LETTER}|{DIGIT}|"_")* 	{MYECHO;check_id();return(ID);}
"<<="		{MYECHO;return(SLASSIGN);}
">>="		{MYECHO;return(SRASSIGN);}
"|="		{MYECHO;return(ORASSIGN);}
"&="		{MYECHO;return(ANDASSIGN);}
"^="		{MYECHO;return(XORASSIGN);}
"&&"		{MYECHO;return(LAND);}
"||"		{MYECHO;return(LOR);}
"^"			{MYECHO;return(XOR);}
"&"			{MYECHO;return(BAND);}
"|"			{MYECHO;return(BOR);}
"<="		{MYECHO;return(LE);}
">="		{MYECHO;return(GE);}
"!="		{MYECHO;return(NE);}
"=="		{MYECHO;return(EQ);}
"<"			{MYECHO;return(LT);}
">"			{MYECHO;return(GT);}
"<<"		{MYECHO;return(SL);}
">>"		{MYECHO;return(SR);}
"++"		{MYECHO;return(INC);}
"--"		{MYECHO;return(DEC);}
"!"			{MYECHO;return(LNOT);}
"~"			{MYECHO;return(BINVERSION);}
"*="		{MYECHO;return(MULASSIGN);}
"/="		{MYECHO;return(DIVASSIGN);}
"%="		{MYECHO;return(MODASSIGN);}
"-="		{MYECHO;return(MINUSASSIGN);}
"+="		{MYECHO;return(PLUSASSIGN);}
"*"			{MYECHO;return(MUL);}
"/"			{MYECHO;return(DIV);}
"%"			{MYECHO;return(MOD);}
"+"			{MYECHO;return(PLUS);}
"-"			{MYECHO;return(MINUS);}
"="			{MYECHO;return(ASSIGN);}
";"			{MYECHO;return(SEMI);}
","			{MYECHO;return(COMMA);}
"{"			{MYECHO;return(LC);}
"}"			{MYECHO;return(RC);}
"["			{MYECHO;return(LB);}
"]"			{MYECHO;return(RB);}
"("			{MYECHO;return(LP);}
")"			{MYECHO;return(RP);}
"."			{MYECHO;return(DOT);}
.			{MYECHO;}
%%

int yywrap()
{return 1;}

void check_dec()
{
	TreeNode* ret(new TreeNode(yylineno));
	if(*yytext == '-')
		ret->iVal = -1 * atoi(yytext+1);
	else
		ret->iVal = atoi(yytext);

	ret->node_type = "CONST";
	ret->type = "int";
	yylval.node = ret;
}

void check_hex()
{
	TreeNode* ret(new TreeNode(yylineno));
	char* p = yytext;
	int sign = 1;
	if(*p=='-')
	{
		p++;
		sign = -1;
	}

	p++;p++;//omit "0x"
	int val=0;
	int len = yyleng-2;
	for(int i=0;i<len;i++)
	{
		if('a' <= *p && *p <= 'f')
			val = val*16 + *p-'a' + 10;
		else if('A' <= *p && *p <= 'F')
			val = val*16 + *p -'A'+10;
		else 
			val = val*16 + *p - '0';
		p++;
	}
	
	ret->iVal = sign * val;
	ret->node_type = "CONST";
	ret->type = "int";
	
	yylval.node = ret;
}

void check_oct()
{
	TreeNode* ret(new TreeNode(yylineno));
	char* p = yytext;
	int sign = 1;
	if(*p == '-')
	{
		p++;
		sign = -1;
	}
	p++;
	
	int val = 0;
	int len = yyleng-1;
	for(int i=0;i<len;i++)
	{
		val = val*8 + *p - '0';
		p++;
	}
	
	ret->iVal = sign * val;
	ret->node_type = "CONST";
	ret->type = "int";
	
	yylval.node = ret;
}

void int_error()
{
	TreeNode* ret(new TreeNode(yylineno));
	
	ret->iVal = 0;
	ret->node_type = "ERROR_CONST";
	ret->type = "error_int";
	
	yylval.node = ret;
}

void check_id() 
{
	TreeNode* ret(new TreeNode(yylineno));
	ret->name = (string)yytext;
	ret->node_type = "ID";
	ret->type = "int";
	
	yylval.node = ret;
}
