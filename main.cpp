#include <iostream>
#include <cstdlib>
#include "codegen/codegen.hpp"
using namespace std;

ParsingTree pt;
SymbolTable symbolTable;
extern int yyparse(char*);
extern FILE* yyin;


int main(int argc,char* argv[])
{
	if(!(yyin=fopen(argv[1],"r")))
	{
		cerr<<"File not exist.\n";
		exit(-1);
	}
	
	if(yyparse(argv[2])) exit(1);
	//else cout<<"successed.\n";

	pt.printTree(argv[2]);

	CodeGenContext context(argv[3]);
	context.generateCode(pt);

	return 0;
}
