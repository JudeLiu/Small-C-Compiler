/**
 *  @file codegen.cpp
 *
 *  @brief This file generates LLVM intermediate representation from small-c source code.
 */
 
#include "codegen.hpp"
extern SymbolTable symbolTable;
#define CODE_LENGTH 200

void CodeGenContext::generateCode(ParsingTree& pt)
{
	fout<<";Author: Liu Junnan\n\n";
	fout<<"@.str = private unnamed_addr constant [3 x i8] c\"%d\\00\", align 1\n\n";//"%d"
	//fout<<"@.str.1 = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\", align 1\n\n";//"%d\n"

	EXTDEFS_genCode(pt.root->child);

	if(!symbolTable.exist("main"))
	{
		cerr<<"do not have the entrance function \'main\'\n";
		close();
		exit(1);
	}

	fout<<"declare i32 @__isoc99_scanf(i8*, ...)\n\n";
	fout<<"declare i32 @printf(i8*, ...)\n";
}

void CodeGenContext::EXTDEFS_genCode(TreeNode* node)
{
	if(node->node_type == "no_def")
		return;
	EXTDEF_genCode(node->child);
	EXTDEFS_genCode(node->child->sibling);
}

void CodeGenContext::EXTDEF_genCode(TreeNode* node)
{
	Configure config;
	config.scope = "global";

	if(node->node_type == "def_var")
	{
		if(node->child->node_type == "struct")
		{
			config.type = "struct";
		}
		config.structName = SPEC_genCode(node->child);
		EXTVARS_genCode(node->child->sibling,config);
	}
	else if(node->node_type == "def_func")
	{
		init();

		config.type = node->child->type;
		FUNC_genCode(node->child->sibling,config);
		STMTBLOCK_genCode(node->child->sibling->sibling);
		fout<<"}\n";
	}
}

void CodeGenContext::EXTVARS_genCode(TreeNode* node, Configure& config)
{
	if(node->node_type != "no_dec")
	{
		DEC_genCode(node->child,config);
		if(node->node_type == "decs")
		{
			EXTVARS_genCode(node->child->sibling,config);
		}
	}	
}

string CodeGenContext::SPEC_genCode(TreeNode* node)
{
	//find the type. In this project all variables and functions are i32 
	if(node->node_type == "struct")
		return STSPEC_genCode(node->child);
	else 
		return string("");
}

string CodeGenContext::STSPEC_genCode(TreeNode* node)
{
	/*
	 * deal with symbol table
	 */
	if(node->node_type == "struct_spec")
	{
		//definition of  a structure
		string structName;
		if(node->child->node_type == "no_tag")
		{
			structName = "anon" + to_string(anonymousStructNumber);
			anonymousStructNumber++;
		}
		else
		{
			structName = node->child->child->name;
		}
		Symbol sym(structName,"structType","local");
		symbolTable.insert(sym);

		//deal with variables that are in the struct
		TreeNode* tmp_node = node->child->sibling;
		int structMemberNumber = 0; 
		while(tmp_node->node_type == "defs")
		{
			Symbol tsym(tmp_node->child->child->sibling->child->child->name,"int","struct");
			tsym.structName = structName;
			tsym.structIdx = structMemberNumber;
			symbolTable.insert(tsym);

			structMemberNumber++;
			tmp_node = tmp_node->child->sibling;
		}
		fout<<"%struct."<<structName<<" = type { ";
		for(int i=0;i<structMemberNumber-1;i++)
			fout<<"i32, ";
		fout<<"i32 }\n";

		return structName;
	}
	else
	{
		//stspec -> STRUCT ID
		//struct variable declaration
		return node->child->name;
	}
}

void CodeGenContext::OPTTAG_genCode(TreeNode* node)
{

}

void CodeGenContext::VAR_genCode(TreeNode* node, Configure& config)
{
	if(node->node_type == "array")
	{
		if(symbolTable.exist(node->name))
		{
			cerr<<"Line "<<node->lineno<<":error: \'"<<node->name<<"\' is multi-defined \n";
			close();
			exit(1);
		}

		//actually no use
		if(node->child->sibling->node_type != "CONST")
		{
			cerr<<"Line "<<node->child->sibling->lineno<<":error: length of array \'"
				<<node->name<<"\' must be fixed\n";
			close();
			exit(1);
		}

		Symbol sym(node->name,"array",config.scope,node->child->sibling->iVal);
		symbolTable.insert(sym);
	}
	else if(config.type == "struct")
	{
		if(symbolTable.exist(node->name))
		{
			cerr<<"Line "<<node->lineno<<":error: \'"<<node->name<<"\' is multi-defined \n";
			close();
			exit(1);
		}

		Symbol sym(node->name,"struct",config.scope);
		sym.structName = config.structName;
		symbolTable.insert(sym);
	}
	else
	{	
		if(symbolTable.exist(node->name))
		{
			cerr<<"Line "<<node->lineno<<":error: \'"<<node->name<<"\' is multi-defined \n";
			close();
			exit(1);
		}

		//ID
		Symbol sym(node->name,"int",config.scope);
		symbolTable.insert(sym);
	}
}

void CodeGenContext::FUNC_genCode(TreeNode* node,Configure& config)
{
	fout<<"\ndefine i32 @"<<node->child->name<<"(";
	//deal with parameters
	
	int paraNum = 0;
	//PARAS_genCode(node->child->sibling);
	//deal with paras
	TreeNode* tmp_node = node->child->sibling;
	vector<string> paras;

	if(tmp_node->node_type != "no_para")
	{
		while(tmp_node->node_type == "paras")
		{
			string tmpName = tmp_node->child->child->sibling->name;
			Symbol sym(tmpName,"int","arg");
			symbolTable.insert(sym);
			paraNum++;
			paras.push_back(tmpName);
			fout<<"i32 %"<<tmpName<<", ";

			tmp_node = tmp_node->child->sibling;
		}
		if(tmp_node->node_type != "no_para")
		{
			string tmpName = tmp_node->child->child->sibling->name;
			Symbol sym(tmpName,"int","arg");
			symbolTable.insert(sym);
			paraNum++;
			paras.push_back(tmpName);
			fout<<"i32 %"<<tmpName;
		}
	}
	fout<<")\n{\n";

	for(int i=0;i<paras.size();i++)
	{
		string name = paras[i];
		fout<<"  %"<<name<<".addr = alloca i32, align 4\n"
			<<"  store i32 %"<<name<<", i32* %"<<name<<".addr\n";
	}

	//add to symbol table
	Symbol sym(node->child->name,"func","global");	
	sym.paraNum = paraNum;
	symbolTable.insert(sym);
}
/*
void CodeGenContext::PARAS_genCode(TreeNode* node)
{
	if(node->node_type == "paras")
	{
		PARA_genCode(node->child);
		fout<<", ";
		PARAS_genCode(node->child->sibling);
	}
	else if(node->node_type == "para")
	{
		PARA_genCode(node->child);
	}
	else return;
}

void CodeGenContext::PARA_genCode(TreeNode* node)
{
	paraNum++;
	//add to symbol table
	node = node->child;
	Symbol sym(node->sibling->name,node->sibling->type,"arg");
	symbolTable.insert(sym);

//	SPEC_genCode(node);

	fout<<"i32 %"<<node->sibling->name;
}
*/
void CodeGenContext::STMTBLOCK_genCode(TreeNode* node)
{
	DEFS_genCode(node->child);
	STMTS_genCode(node->child->sibling);
}

void CodeGenContext::STMTS_genCode(TreeNode* node)
{
	if(node->node_type == "no_stmt")
		return;
	STMT_genCode(node->child);
	STMTS_genCode(node->child->sibling);
}

void CodeGenContext::STMT_genCode(TreeNode* node)
{
	string nnodetype = node->node_type;
	char* code = new char [CODE_LENGTH];
	if(nnodetype == "return")
	{
		//deal with return
		if(node->child->node_type == "CONST")
		{
			if(node->child->type == "int")
			{
				sprintf(code,"  ret i32 %d",node->child->iVal);
			}
		}
		else
		{
			//return value is a expression, and cannot determined in compile-time
			char* exp = new char[10];
			exp = EXP_genCode(node->child);
			sprintf(code,"  ret i32 %%%s",exp);
		}

		fout<<code<<endl;
	}
	else if(nnodetype == "if")
	{
		int tmpNumber=ifNumber;
		ifNumber++;

		if(node->child->node_type == "CONST")
		{
			fout<<"  br i1 "<<node->child->iVal<<", label %if.then"<<tmpNumber<<", label %if.end"<<tmpNumber<<endl;
		}
		else
		{
			char* cond = new char[10];
			cond = CondEXP_genCode(node->child);

			fout<<"  br i1 %"<<cond<<", label %if.then"<<tmpNumber<<", label %if.end"<<tmpNumber<<endl;
		}
		fout<<"\nif.then"<<tmpNumber<<":\n";
		STMT_genCode(node->child->sibling);
		fout<<"  br label %if.end"<<tmpNumber
			<<"\n\nif.end"<<tmpNumber<<":\n";
	}
	else if(nnodetype == "if_else")
	{
		int tmpNumber = ifelseNumber;
		ifelseNumber++;

		if(node->child->node_type == "CONST")
			fout<<"  br i1 "<<node->child->iVal<<", label %if.then"<<tmpNumber<<", label %if.else"<<tmpNumber<<endl
				<<"\nif.then"<<tmpNumber<<":\n";
		else
		{			
			char* cond = new char[10];
			cond = CondEXP_genCode(node->child);

			fout<<"  br i1 %"<<cond<<", label %if.then"<<tmpNumber<<", label %if.else"<<tmpNumber<<endl
				<<"\nif.then"<<tmpNumber<<":\n";	
		}

		STMT_genCode(node->child->sibling);

		fout<<"  br label %if.end"<<tmpNumber
			<<"\n\nif.else"<<tmpNumber<<":\n";

		STMT_genCode(node->child->sibling->sibling);

		fout<<"  br label %if.end"<<tmpNumber<<
			"\n\nif.end"<<tmpNumber<<":\n";
	}
	else if(nnodetype == "for")
	{
		int tmpNumber = forNumber;
		forNumber++;

		FEXP_genCode(node->child);
		fout<<"  br label %for.cond"<<tmpNumber<<endl
			<<"\nfor.cond"<<tmpNumber<<":\n";

		if(node->child->sibling->node_type != "no_expr")
		{
			char* cond = new char[10];
			cond = CondFEXP_genCode(node->child->sibling);
			fout<<"  br i1 %"<<cond<<", label %for.body"<<tmpNumber<<", label %for.next"<<tmpNumber<<endl;
		}
		else
		{
			fout<<"  br label %for.body"<<tmpNumber<<endl;
		}		

		fout<<"\nfor.body"<<tmpNumber<<":\n";
		STMT_genCode(node->child->sibling->sibling->sibling);
		FEXP_genCode(node->child->sibling->sibling);
		fout<<"  br label %for.cond"<<tmpNumber<<endl;
		fout<<"\nfor.next"<<tmpNumber<<":\n";
	}
	else if(nnodetype == "fexp")
	{
		if(node->child->child->node_type == "no_expr")
			return ;
		else
			FEXP_genCode(node->child);
	}
	else if(nnodetype == "break")
	{
		fout<<"  br label %for.next"<<forNumber-1<<endl;
	}
	else if(nnodetype == "continue")
	{
		fout<<"  br label %for.body"<<forNumber-1<<endl;
	}
	else if(nnodetype == "stmtblock")
	{
		STMTBLOCK_genCode(node->child);
	}
}

void CodeGenContext::ESTMT_genCode(TreeNode* node)
{
}

void CodeGenContext::DEFS_genCode(TreeNode* node)
{
	if(node->node_type == "no_def")
		return;
	DEF_genCode(node->child);
	DEFS_genCode(node->child->sibling);
}

void CodeGenContext::DEF_genCode(TreeNode* node)
{
	Configure config;
	config.scope = "local";
	config.type = node->child->type;

	SPEC_genCode(node->child);
	DECS_genCode(node->child->sibling,config);
}

void CodeGenContext::DECS_genCode(TreeNode* node, Configure& config)
{
	DEC_genCode(node->child,config);
	if(node->node_type == "decs")
	{
		DECS_genCode(node->child->sibling,config);
	}
}

void CodeGenContext::DEC_genCode(TreeNode* node, Configure& config)
{
	VAR_genCode(node->child,config);
	string ncName = node->child->name;//node->child->name;
	Symbol sym = symbolTable.find(ncName)->second;

	if(sym.type == "struct")
	{	
		if(config.scope == "global")
		{
			fout<<"@"<<ncName<<" = common global %struct."<<sym.structName<<" zeroinitializer, align 4\n";
		}
	}
	else if(config.scope == "global")
	{
		fout<<'@'<<ncName<<" = global ";
		if (sym.type == "int")
		{
			fout<<"i32 ";
			if (node->node_type == "dec_init")
			{
				INIT_genCode(node->child->sibling,config,ncName);
			}
			else
				fout<<"0";
			fout<<", align 4\n";

		}
		else if(sym.type == "array")
		{
			fout<<'['<<sym.arrSize<<" x i32] ";
			if(node->node_type == "dec_init")
				INIT_genCode(node->child->sibling,config,ncName);
			else
				fout<<"zeroinitializer";
			if(sym.arrSize >=4 )
				fout<<", align 16\n";
			else
				fout<<", align 4\n";
		}
	}
	else
	{
		//local variable
		fout<<"  %"<<ncName<<" = alloca ";
		if(sym.type == "int")
		{
			fout<<"i32, align 4\n";
		}
		else if(sym.type == "array") 
		{
			/* different from clang, e.g. given int a[3]={1,2}
			 * the IR will be 
			 * %a = alloca [3 x i32], align 12
			 * %arrIdx0 = getelementptr [3 x i32], [3 x i32]* %a, i32 0, i32 0
			 * %arrIdx1 = getelementptr [3 x i32], [3 x i32]* %a, i32 0, i32 1
			 * %arrIdx2 = getelementptr [3 x i32], [3 x i32]* %a, i32 0, i32 2
			 * store i32 1, i32* %arrIdx0, align 4
			 * store i32 2, i32* %arrIdx1, align 4
			 * store i32 0, i32* %arrIdx2, align 4
			 *
			 */
			 int arrSize = sym.arrSize;
			 fout<<"["<<arrSize<<" x i32], align 4\n";
		}

		if(node->node_type == "dec_init")
		{
			INIT_genCode(node->child->sibling,config,ncName);
		}
	}
}

void CodeGenContext::INIT_genCode(TreeNode* node,Configure& config, string name)
{
	if(config.scope == "global")
	{
		//global variable init
		if(node->node_type == "set_init")
		{
			int arrSize = symbolTable.find(name)->second.arrSize;

			fout<<				\
			'[';

			///////////////////////////////////////////////
			// value of arg is inverse, os use a stack
			///////////////////////////////////////////////
			node = node->child;//node->node_type == "args"
			int* tempResult = new int[arrSize];
			for(int i=0;i<arrSize;i++)
				tempResult[i] = 0;

			bool pass = false;
			if(node->node_type != "no_arg")
			{
				for(int i=0;i<arrSize;++i)
				{
					if(node->node_type == "arg")
					{
						tempResult[i] = node->child->iVal;
						pass = true;
						break;
					}
					if(node->child->node_type != "CONST")
					{
						cerr<<"Line "<<node->lineno<<":error: initializer element is not a comiler-time constant\n";
						close();
						exit(1);
					}
					tempResult[i] = node->child->iVal;
					node = node->child->sibling;
				}

				if(node != nullptr && !pass)
				{
					cerr<<"Line "<<node->lineno<<":warning:excess elements in array initializer\n";
				}
			}
			for(int i=0;i<arrSize-1;i++)
				fout<<"i32 "<<tempResult[i]<<", ";

			fout<<"i32 "<<tempResult[arrSize-1];

			fout<<				\
			']';
		}
		else 
		{
			TreeNode* n = node->child;
			if(n->node_type == "CONST")
			{
				fout<<n->iVal;
			}
			else 
			{
				cerr<<"Line "<<n->lineno<<": error: initializer element is not a compiler-time constant\n";
				close();
				exit(1);
			}
		}
	}
	else
	{
		//local variable init
		if(node->node_type == "set_init")
		{
			/* different from clang, e.g. give int a[3]={1,2}
			 * the IR will be 
			 * %a = alloca [3 x i32], align 4
			 * %arrIdx0 = getelementptr [3 x i32], [3 x i32]* %a, i32 0, i32 0
			 * %arrIdx1 = getelementptr [3 x i32], [3 x i32]* %a, i32 0, i32 1
			 * %arrIdx2 = getelementptr [3 x i32], [3 x i32]* %a, i32 0, i32 2
			 * store i32 1, i32* %arrIdx0, align 4
			 * store i32 2, i32* %arrIdx1, align 4
			 * store i32 0, i32* %arrIdx2, align 4
			 *
			 */
			int arrSize = symbolTable.find(name)->second.arrSize;
			int* tempResult = new int[arrSize];
			node = node->child;

			for(int i=0;i<arrSize;++i)
			{
				if(node->node_type == "arg")
				{
					tempResult[i] = node->child->iVal;
					break;
				}
				if(node->child->node_type != "CONST")
				{
					cerr<<"Line "<<node->lineno<<":error: initializer element is not a comiler-time constant\n";
					close();
					exit(1);
				}
				tempResult[i] = node->child->iVal;
				node = node->child->sibling;
			}

			if(node->node_type == "args")
			{
				cerr<<"Line "<<node->lineno<<":warning:excess elements in array initializer\n";
			}

			for(int i=0;i<arrSize;i++)
				fout<<"  %arrIdx"<<i<<" = getelementptr inbounds ["<<arrSize<<" x i32], ["<<arrSize<<" x i32]*"
					<<" %"<<name<<", i32 0, i32 "<<i<<endl;
			for(int i=0;i<arrSize;i++)
				fout<<"  store i32 "<<tempResult[i]<<", i32* %arrIdx"<<i<<", align 4\n";
		}
		else
		{
			//exp
			TreeNode* n = node->child;
			if(n->node_type == "CONST")
			{
				fout<<"  store ";
				if(config.type == "int")
					fout<<"i32 "<<n->iVal<<", i32* %"<<name<<", align 4\n";
			}
			else 
			{
				cerr<<"Line "<<n->lineno<<": error: Initializer element is not a compiler-time constant\n";
			}
		}
	}
}

char* CodeGenContext::CondEXP_genCode(TreeNode* node)
{
	char* ret = new char[10];
	string nnodetype = node->node_type;
	if(nnodetype == "CONST")
	{
		fout<<"  %t"<<tempRegNumber<<" = icmp ne i32 "<<node->iVal<<", 0\n";
		tempRegNumber++;
	}
	else if(nnodetype == "logic_and" || nnodetype == "logic_or" || nnodetype == "logic_not" ||
			nnodetype == "less_than" || nnodetype == "less_equal" || nnodetype == "greater_than" ||
			nnodetype == "greater_equal" || nnodetype == "equal" || nnodetype == "not_equal")
	{
		EXP_genCode(node);
	}
	else
	{
		char* exp = new char[10];
		exp = EXP_genCode(node);
		fout<<"  %t"<<tempRegNumber<<" = icmp ne i32 %"<<exp<<", 0\n";
		tempRegNumber++;
	}

	sprintf(ret,"t%d",tempRegNumber-1);
	return ret;
}

char* CodeGenContext::CondFEXP_genCode(TreeNode* node)
{
	string ncnodetype = node->child->node_type;
	char* ret = new char[10];
	if(ncnodetype == "CONST")
	{
		fout<<"  %t"<<tempRegNumber<<" = icmp ne i32 "<<node->iVal<<", 0\n";
		tempRegNumber++;
	}	
	else if(ncnodetype == "logic_and" || ncnodetype == "logic_or" || ncnodetype == "logic_not" ||
			ncnodetype == "less_than" || ncnodetype == "less_equal" || ncnodetype == "greater_than" ||
			ncnodetype == "greater_equal" || ncnodetype == "equal" || ncnodetype == "not_equal")
	{
		EXP_genCode(node->child);
	}
	else
	{
		char* exp = new char[10];
		exp = EXP_genCode(node->child);
		fout<<"  %t"<<tempRegNumber<<" = icmp ne i32 %"<<exp<<", 0\n";
		tempRegNumber++;
	}
	sprintf(ret,"t%d",tempRegNumber-1);
	return ret;
}

char* CodeGenContext::EXP_genCode(TreeNode* node)
{
	string nnodetype =  node->node_type;
	if(nnodetype == "assign")
	{
		//LHS must be a ID, array or struct member, or function call, cannot be the other expressions
		string ncnodetype = node->child->node_type;
		if(ncnodetype != "ID" && ncnodetype != "array" && ncnodetype != "member")
		{
			cerr<<"Line "<<node->child->lineno<<": lvalue cannot be an expression\n";
			
			exit(1);
		}
		else if(ncnodetype == "member")
		{
			string name = node->child->child->name;
			string memberName = node->child->child->sibling->name;

			if(!symbolTable.exist(name))
			{
				cerr<<"Line "<<node->child->child->lineno<<":error: \' "<<name<<"\' undeclared\n";
				close();
				exit(1);
			}
			
			if(symbolTable.find(name)->second.type != "struct")
			{
				cerr<<"Line "<<node->child->child->lineno<<":error: request for member \'"
					<<memberName<<"\' in something not a structure\n";
				close();
				exit(1);
			}

			if(!symbolTable.exist(memberName))
			{
				cerr<<"Line "<<node->child->child->sibling->lineno<<":error: \'"<<memberName<<"\' undeclared\n";
				close();
				exit(1);
			}
			
			//this struct is the struct variable
			Symbol thisStruct = symbolTable.find(name)->second;
			string structName = thisStruct.structName;
			Symbol memberSym = symbolTable.find(memberName)->second;

			if(memberSym.structName != structName)
			{
				cerr<<"Line "<<node->child->sibling->lineno<<":error: no member named \'"
					<<memberName<<"\' in 'struct "<<structName<<"\'\n";
				close();
				exit(1);
			}

			int memberIdx = memberSym.structIdx;
			if(node->child->sibling->node_type == "CONST")
			{
				fout<<"  store i32 "<<node->child->sibling->iVal<<", i32* getelementptr inbounds (%struct."
					<<structName<<", %struct."<<structName<<"* @"<<name<<", i32 0, i32 "<<memberIdx<<"), align 4\n";
			}
			else
			{
				char* rhs = new char[10];
				rhs = EXP_genCode(node->child->sibling);
				fout<<"  store i32 %"<<rhs<<", i32* getelementptr inbounds (%struct."<<structName
					<<", %struct."<<structName<<"* @"<<name<<", i32 0, i32 "<<memberIdx<<"), align 4\n";
			}
		}
		else if(ncnodetype == "array")
		{
			int lhs;
			if(node->child->sibling->node_type == "CONST")
			{
				lhs = atoi(EXP_genCode(node->child)+1);
				fout<<"  store i32 "<<node->child->sibling->iVal<<", i32* %t"<<lhs-1<<", align 4\n";
			}
			else
			{
				char* rhs = new char[10];
				rhs = EXP_genCode(node->child->sibling);
				lhs = atoi(EXP_genCode(node->child)+1);
				fout<<"  store i32 %"<<rhs<<", i32* %t"<<lhs-1<<", align 4\n";
			}
		}
		else if(ncnodetype == "ID")
		{
			if(!symbolTable.exist(node->child->name))
			{
				cerr<<"Line "<<node->child->lineno<<":error: \'"<<node->child->name<<"\' undeclared\n";
				exit(1);
			}

			string ncsNodeType = node->child->sibling->node_type;

			vector<string> scopeAndISarg;
			if(ncsNodeType == "CONST")
			{
				scopeAndISarg = scopeAndIsarg(node->child->name);
				fout<<"  store i32 "<<node->child->sibling->iVal<<", i32* "
					<<scopeAndISarg[0]<<node->child->name<<scopeAndISarg[1]<<", align 4\n";
			}
			else
			{
				char* rhs = new char[10];
				rhs = EXP_genCode(node->child->sibling);
				scopeAndISarg = scopeAndIsarg(node->child->name);
				fout<<"  store i32 %"<<rhs<<", i32* "<<scopeAndISarg[0]
					<<node->child->name<<scopeAndISarg[1]<<", align 4\n";
			}
		}
		char* ret = new char[10];
		sprintf(ret,"t%d",tempRegNumber-1);
		return ret;
	}
	else if(nnodetype == "member")
	{
		string name = node->child->name;
		string memberName = node->child->sibling->name;
		string structName;

		if(!symbolTable.exist(name))
		{
			cerr<<"Line "<<node->child->lineno<<":error: \' "<<name<<"\' undeclared\n";
			close();
			exit(1);
		}
		if(!symbolTable.exist(memberName))
		{
			cerr<<"Line "<<node->child->sibling->lineno<<":error: \'"<<memberName<<"\' undeclared\n";
			close();
			exit(1);
		}

		Symbol thisStruct = symbolTable.find(name)->second;
		structName = thisStruct.structName;
		Symbol memberSym = symbolTable.find(memberName)->second;

		if(thisStruct.type != "struct")
		{
			cerr<<"Line "<<node->child->lineno<<"error: \'"<<thisStruct.name<<"\' is not a struct\n";
			close();
			exit(1);
		}
		if(memberSym.structName != structName)
		{
			cerr<<"Line "<<node->child->sibling->lineno<<":error: no member named \'"
				<<memberName<<"\' in 'struct "<<structName<<"\'\n";
			close();
			exit(1);
		}
		int memberIdx = memberSym.structIdx;
		fout<<"  %t"<<tempRegNumber<<" = load i32, i32* getelementptr inbounds (%struct."<<structName
			<<", %struct."<<structName<<"* @"<<name<<", i32 0, i32 "<<memberIdx<<"), align 4\n";
		tempRegNumber++;

		char* ret = new char[10];
		sprintf(ret,"t%d",tempRegNumber-1);
		return ret;
	}
	else if(nnodetype == "array")
	{
		//1-dimension array
		string name = node->child->name;
		SymbolTable::iterator sym_ite = symbolTable.find(name);

		//check if declared
		if(sym_ite == symbolTable.end())
		{
			cerr<<"Line "<<node->child->lineno<<":error: \'"<<name<<"\' undeclared\n";
			close();
			exit(1);
		}

		if(sym_ite->second.type != "array")
		{
			cerr<<"Line "<<node->lineno<<":error: subscripted value is not array\n";
			close();
			exit(1);
		}
		vector<string> scopeAndISarg = scopeAndIsarg(name);
		int arrSize = sym_ite->second.arrSize;
		if(node->child->sibling->child->node_type == "CONST")
		{
			int index = node->child->sibling->child->iVal;
			fout<<"  %t"<<tempRegNumber<<" = getelementptr inbounds ["<<arrSize<<" x i32], ["<<arrSize<<" x i32]* "
				<<scopeAndISarg[0]<<name<<", i32 0, i32 "<<index<<'\n';
			tempRegNumber++;
		}
		else
		{
			char* exp = new char[10];
			exp = EXP_genCode(node->child->sibling->child);
			fout<<"  %t"<<tempRegNumber<<" = getelementptr inbounds ["<<arrSize<<" x i32], ["<<arrSize
				<<" x i32]* "<<scopeAndISarg[0]<<name<<", i32 0, i32 %"<<exp<<endl;
			tempRegNumber++;
		}
		fout<<"  %t"<<tempRegNumber<<" = load i32, i32* %t"<<tempRegNumber-1<<", align 4\n";
		tempRegNumber++;

		char* ret = new char[10];
		sprintf(ret,"t%d",tempRegNumber-1);
		return ret;
	}
	else if(nnodetype == "CONST")
	{
		char* ret = new char[CODE_LENGTH];
		sprintf(ret,"t%d",node->iVal);
		return ret;
	}
	else if(nnodetype == "func_call")
	{
		if(node->child->name == "read")
		{
			char* exp = new char[10];
			//exp = EXP_genCode(node->child->sibling);
			char* code = new char[CODE_LENGTH];

			string ncscNodeType = node->child->sibling->child->node_type;

			if( ncscNodeType == "ID")
			{
				string name = node->child->sibling->child->name;
				vector<string> scopeAndISarg = scopeAndIsarg(name);
				sprintf(code,"  %%t%d = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* %s%s%s)\n",\
						tempRegNumber,scopeAndISarg[0].c_str(),name.c_str(),scopeAndISarg[1].c_str());
				tempRegNumber++;
			}
			else if(ncscNodeType == "array")
			{
				string name = node->child->sibling->child->name;
				vector<string> scopeAndISarg = scopeAndIsarg(name);
				int op = atoi(EXP_genCode(node->child->sibling->child)+1) - 1;
				sprintf(code,"  %%t%d = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* %%t%d)\n",\
					tempRegNumber,op);
				tempRegNumber++;
			}
			else if(ncscNodeType == "member")
			{
				TreeNode* memberNode = node->child->sibling->child;
				string name = memberNode->child->name;
				string memberName = memberNode->child->sibling->name;

				if(!symbolTable.exist(name))
				{
					cerr<<"Line "<<memberNode->child->lineno<<":error: \' "<<name<<"\' undeclared\n";
					close();
					exit(1);
				}
				if(!symbolTable.exist(memberName))
				{
					cerr<<"Line "<<memberNode->child->sibling->lineno<<":error: \'"<<memberName<<"\' undeclared\n";
					close();
					exit(1);
				}

				Symbol thisStruct = symbolTable.find(name)->second;
				string structName = thisStruct.structName;
				Symbol memberSym = symbolTable.find(memberName)->second;

				if(memberSym.structName != structName)
				{
					cerr<<"Line "<<node->child->sibling->lineno<<":error: no member named \'"
						<<memberName<<"\' in 'struct "<<structName<<"\'\n";
					close();
					exit(1);
				}

				int memberIdx = memberSym.structIdx;
				fout<<"  %t"<<tempRegNumber
					<<" = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* getelementptr inbounds (%struct."
					<<structName<<", %struct."<<structName<<"* @"<<name<<", i32 0, i32 "<<memberIdx<<"))\n";
				tempRegNumber++;
				
				char* ret = new char[10];
				sprintf(ret,"t%d",tempRegNumber-1);
				return ret;
			}
			else
			{
				cerr<<"Line "<<node->child->sibling->lineno<<":error: args of function \'read\' should be a variable\n";
				close();
				exit(1);
			}
			fout<<code;
			
			sprintf(exp,"t%d",tempRegNumber-1);
			return exp;
		}
  		else if(node->child->name == "write")
  		{
  			if(node->child->sibling->child->node_type == "CONST")
  			{
  				fout<<"  %t"<<tempRegNumber<<" = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 "
  					<<node->child->sibling->child->iVal<<")\n";
				tempRegNumber++;
			}
			else
			{
				char* exp = new char[10];
				exp = ARGS_genCode(node->child->sibling);
				char* code = new char[CODE_LENGTH];

				sprintf(code,"  %%t%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %%%s)\n",\
					tempRegNumber,exp);
				
				fout<<code;
				tempRegNumber++;
			}
			char* ret = new char[10];
			sprintf(ret,"t%d",tempRegNumber-1);
			return ret;
  		}
  		else
  		{
  			//function call other than read or write
  			if(!symbolTable.exist(node->child->name))
  			{
  				cerr<<"Line "<<node->child->lineno<<"error: function \'"<<node->child->name<<"\n undeclared\n";
  				close();
  				exit(1);
  			}

  			//deal with arguments
  			vector<string> type,args;
  			TreeNode* tmp_node = node->child->sibling;
  			
  			while(tmp_node->node_type == "args")
  			{
  				if(tmp_node->child->node_type == "CONST")
  				{
  					args.push_back(to_string(tmp_node->child->iVal));
  					type.push_back("");
  				}
  				else
  				{
  					args.push_back(EXP_genCode(tmp_node->child));
  					type.push_back("%");
  				}
  				tmp_node = tmp_node->child->sibling;
  			}
  			//the last argument
  			if(tmp_node->node_type == "arg")
  			{
  				if(tmp_node->child->node_type == "CONST")
  				{
  					args.push_back(to_string(tmp_node->child->iVal));
  					type.push_back("");
  				}
  				else
  				{
  					args.push_back(EXP_genCode(tmp_node->child));	
  					type.push_back("%");
  				}
  			}

  			Symbol sym = symbolTable.find(node->child->name)->second;
  			if(args.size() > sym.paraNum)
  			{
  				cerr<<"Line "<<node->child->lineno<<":error: too many arguments to call function \'"<<sym.name<<"\', expected "
  					<<sym.paraNum<<", have "<<args.size()<<endl;
  				close();
  				exit(1);
  			}
  			else if(args.size() < sym.paraNum)
  			{
  				cerr<<"Line "<<node->child->lineno<<":error: too few arguments to call function \'"<<sym.name<<"\', expected "<<
  					sym.paraNum<<", have "<<args.size()<<endl;
  				close();
  				exit(1);
  			}

  			fout<<"  %t"<<tempRegNumber<<" = call i32 @"<<node->child->name<<"(";
  			tempRegNumber++;
  			if(sym.paraNum > 1)
  			{
  				for(int i=0;i<sym.paraNum-1;i++)
  				{
  					fout<<"i32 "<<type[i]<<args[i]<<", ";
  				}
  				fout<<"i32 "<<type[type.size()-1]<<args[args.size()-1];
  			}
  			else if(sym.paraNum == 1)
  			{
  				fout<<"i32 "<<type[0]<<args[0];
  			}
  			else
  			{}
  			fout<<")\n";

  			char* ret = new char[10];
  			sprintf(ret,"t%d",tempRegNumber-1);
  			return ret;
  		}
	}
	else if(nnodetype == "ID")
	{
		SymbolTable::iterator sym_ite = symbolTable.find(node->name);	
		if(sym_ite == symbolTable.end())
		{
			cerr<<"Line "<<node->lineno<<" : error: \'"<<node->name<<"\' undeclared\n";
			
			exit(1);
		}
		else
		{
			if(sym_ite->second.type == "int")	
			{
				vector<string> scopeAndISarg = scopeAndIsarg(node->name);
				fout<<"  %t"<<tempRegNumber<<" = load i32, i32* "<<scopeAndISarg[0]
					<<node->name<<scopeAndISarg[1]<<", align 4\n";
			}
			char *ret = new char[10];
			sprintf(ret,"t%d",tempRegNumber);

			tempRegNumber++;

			return ret;
		}
	}
	else if(nnodetype == "plus" || nnodetype == "minus" || nnodetype == "mul" || 
			nnodetype == "div")
	{
		bool exp1IsConst = node->child->node_type == "CONST";
		bool exp2IsConst = node->child->sibling->node_type == "CONST";

		string oper;
		if(nnodetype == "plus")
			oper = "add";
		else if(nnodetype == "minus")
			oper = "sub";
		else if(nnodetype == "mul")
			oper = "mul";
		else if(nnodetype == "div")
			oper = "div";

		if( exp1IsConst && exp2IsConst )
		{
			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" nsw i32 "
				<<node->child->iVal<<", "<<node->child->sibling->iVal<<endl;
			tempRegNumber++;
		}
		else if( !exp1IsConst && exp2IsConst )
		{
			char* op1 = EXP_genCode(node->child);
			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" nsw i32 %"<<op1
				<<", "<<node->child->sibling->iVal<<endl;
			tempRegNumber++;
		}
		else if( !exp1IsConst && !exp2IsConst )
		{
			char* op1 = EXP_genCode(node->child);
			char* op2 = EXP_genCode(node->child->sibling);
			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" nsw i32 %"<<op1<<", %"<<op2<<endl;
			tempRegNumber++;
		}
		else
		{
			//!exp1IsConst && exp2IsConst
			char* op1 = EXP_genCode(node->child);
			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" nsw i32 %"<<op1
				<<", "<<node->child->sibling->iVal<<endl;
			tempRegNumber++;
		}
		//deal with type error
		//omit now

		char* ret = new char[10];
		sprintf(ret,"t%d",tempRegNumber-1);
		return ret;
	}
	else if(nnodetype == "mod" || nnodetype == "bit_and" || nnodetype == "bit_or" ||
			nnodetype == "xor" || nnodetype == "shift_left" || nnodetype == "shift_right")
	{
		string oper;
		if(nnodetype == "mod")
			oper = "srem";
		else if(nnodetype == "bit_and")
			oper = "and";
		else if(nnodetype == "bit_or")
			oper = "or";
		else if(nnodetype == "xor")
			oper = "xor";
		else if(nnodetype == "shift_left")
			oper = "shl";
		else if(nnodetype == "shift_right")
			oper = "ashr";

		bool exp1IsConst = node->child->node_type == "CONST";
		bool exp2IsConst = node->child->sibling->node_type == "CONST";

		if( exp1IsConst && exp2IsConst )
		{
			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" i32 "
				<<node->child->iVal<<", "<<node->child->sibling->iVal<<endl;
			tempRegNumber++;
		}
		else if( !exp1IsConst && exp2IsConst )
		{
			char* op1 = EXP_genCode(node->child);
			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" i32 %"<<op1
				<<", "<<node->child->sibling->iVal<<endl;
			tempRegNumber++;
		}
		else if( !exp1IsConst && !exp2IsConst )
		{
			char* op1 = EXP_genCode(node->child);
			char* op2 = EXP_genCode(node->child->sibling);
			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" i32 %"<<op1<<", %"<<op2<<endl;
			tempRegNumber++;
		}
		else
		{
			//!exp1IsConst && exp2IsConst
			char* op1 = EXP_genCode(node->child);
			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" i32 %"<<op1
				<<", "<<node->child->sibling->iVal<<endl;
			tempRegNumber++;
		}
		//deal with type error
		//omit now

		char* ret = new char[10];
		sprintf(ret,"t%d",tempRegNumber-1);
		return ret;
	}
	else if( nnodetype == "equal" || nnodetype == "not_equal" || nnodetype == "less_than" || 
			nnodetype == "greater_than" || nnodetype == "less_equal" || nnodetype == "greater_equal" )
	{
		char* ret = new char[10];
		string oper;
		string nodeType(nnodetype);

		if(nodeType == "equal")
			oper = "eq";
		else if(nodeType == "not_equal")
			oper = "ne";
		else if(nodeType == "less_than")
			oper = "slt";
		else if(nodeType == "greater_than")
			oper = "sgt";
		else if(nodeType == "less_equal")
			oper = "sle";
		else if(nodeType == "greater_equal")
			oper = "sge";

		//cannot both be CONST, guranteed by YACC parser
		if(node->child->node_type == "CONST")
		{
			char* op = new char[10];
			op = EXP_genCode(node->child->sibling);

			fout<<"  %t"<<tempRegNumber<<" = icmp "<<oper<<" i32 "<<node->child->iVal<<", %"<<op<<endl;
			tempRegNumber++;
		}
		else if(node->child->sibling->node_type == "CONST")
		{
			char* op = new char[10];
			op = EXP_genCode(node->child);
			fout<<"  %t"<<tempRegNumber<<" = icmp "<<oper<<" i32 %"<<op<<", "
				<<node->child->sibling->iVal<<endl;
			tempRegNumber++;
		}
		else
		{
			char* op1 = new char[10];
			char* op2 = new char[10];
			op1 = EXP_genCode(node->child);
			op2 = EXP_genCode(node->child->sibling);

			fout<<"  %t"<<tempRegNumber<<" = icmp "<<oper<<" i32 %"<<op1<<", %"<<op2<<endl;
			tempRegNumber++;
		}
		sprintf(ret,"t%d",tempRegNumber-1);
		return ret;
	}
	else if(nnodetype == "logic_and" || nnodetype == "logic_or")
	{
		string oper;
		if(nnodetype == "logic_and")
			oper = "and";
		else
			oper = "or";

		if(node->child->node_type == "CONST")
		{
			char* op = new char[10];
			op = EXP_genCode(node->child->sibling);

			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" i1 "<<node->child->iVal<<", %"<<op<<endl;
			tempRegNumber++;
		}
		else if(node->child->sibling->node_type == "CONST")
		{
			char* op = new char[10];
			op = EXP_genCode(node->child);

			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" i1 %"<<op<<", "<<node->child->sibling->iVal<<endl;
			tempRegNumber++;
		}
		else
		{
			char* op1 = new char[10];
			char* op2 = new char[10];
			op1 = EXP_genCode(node->child);
			op2 = EXP_genCode(node->child->sibling);

			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" i1 %"<<op1<<", %"<<op2<<endl;
			tempRegNumber++;
		}
		char* ret = new char[10];
		sprintf(ret,"t%d",tempRegNumber-1);
		return ret;
	}
	else if(nnodetype == "logic_not")
	{
		string ncnodetype = node->child->node_type;
		char* op = new char[10];
		op = EXP_genCode(node->child);
		if(ncnodetype == "logic_and" || ncnodetype == "logic_or" || ncnodetype == "logic_not")
		{	
			fout<<"  %t"<<tempRegNumber<<" = xor i1 %"<<op<<", 1\n";
			tempRegNumber++;
		}
		else
		{
			//an express other than && or || or !
			fout<<"  %t"<<tempRegNumber<<" = icmp eq i32 %"<<op<<", 0\n";
			tempRegNumber++;
		}

		char* ret = new char[10];
		sprintf(ret,"t%d",tempRegNumber-1);
		return ret;
	}
	else if(nnodetype == "inc" || nnodetype == "dec")
	{
		/* inc and dec can use similar method, e.g. :
		 * inc is %2 = add %1, 1
		 * dec is %2 = add %1, -1
		 */
		string ncName = node->child->name;
		int sign;

		if(!symbolTable.exist(ncName))
		{
			cerr<<"Line "<<node->child->lineno<<"error: \'"<<ncName<<"\' undeclared\n";
			close();
			exit(1);
		}

		if(nnodetype == "inc")
			sign = 1;
		else 
			sign = -1;

		vector<string> scopeAndISarg = scopeAndIsarg(ncName);

		fout<<"  %t"<<tempRegNumber<<" = load i32, i32* "<<scopeAndISarg[0]
			<<ncName<<scopeAndISarg[1]<<", align 4\n";
		tempRegNumber++;
		fout<<"  %t"<<tempRegNumber<<" = add nsw i32 %t"<<tempRegNumber-1<<", "<<sign<<"\n";
		tempRegNumber++;
		fout<<"  store i32 %t"<<tempRegNumber-1<<", i32* "<<scopeAndISarg[0]
			<<ncName<<scopeAndISarg[1]<<", align 4\n";

		char* ret = new char[10];
		sprintf(ret,"t%d",tempRegNumber-2);
		return ret;
	}
	else if(nnodetype == "shift_right_assign" || nnodetype == "shift_left_assign")
	{
		string oper;
		if(nnodetype == "shift_right_assign")
			oper = "ashr";
		else
			oper = "shl";

		if(node->child->node_type != "ID")
		{
			cerr<<"Line "<<node->child->lineno<<"error: lvalue cannot be an expression\n";
			close();
			exit(1);
		}

		char* ret = new char[10];
		vector<string> scopeAndISarg = scopeAndIsarg(node->child->name);

		if(node->child->sibling->node_type == "CONST")
		{
			char* lhs = new char[10];
			lhs = EXP_genCode(node->child);

			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" i32 %"<<lhs<<", "<<node->child->sibling->iVal<<'\n';
			tempRegNumber++;
			fout<<"  store i32 %t"<<tempRegNumber-1<<", i32* "<<scopeAndISarg[0]
				<<node->child->name<<scopeAndISarg[1]<<", align 4\n";

			sprintf(ret,"t%d",tempRegNumber-1);
			return ret;
		}
		else
		{
			char* lhs = new char[10];
			char* rhs = new char[10];
			rhs = EXP_genCode(node->child->sibling);
			lhs = EXP_genCode(node->child);

			fout<<"  %t"<<tempRegNumber<<" = "<<oper<<" i32 %"<<lhs<<", %"<<rhs<<endl;
			tempRegNumber++;
			fout<<"  store i32 %t"<<tempRegNumber-1<<", i32* "<<scopeAndISarg[0]
				<<node->child->name<<scopeAndISarg[1]<<", align 4\n";

			sprintf(ret,"t%d",tempRegNumber-1);
			return ret;
		}
	}
}

char* CodeGenContext::FEXP_genCode(TreeNode* node)
{
	if(node->node_type != "no_expr")
		return EXP_genCode(node->child);
	else
		return strdup("");
}

void CodeGenContext::ARRS_genCode(TreeNode* node)
{
}

char* CodeGenContext::ARGS_genCode(TreeNode* node)
{
	//used in function call
	char* ret = new char[10];
	if(node->node_type == "args")
	{
		EXP_genCode(node->child);
		ret = ARGS_genCode(node->child->sibling);
	}
	else if(node->node_type == "arg")
	{
		ret = EXP_genCode(node->child);
	}
	else ret = strdup("");
	return ret;
}
