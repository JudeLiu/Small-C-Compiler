/**
 * @file codegen.hpp
 *
 * @brief This file declares the class CodeGenContext that is used to 
 * generate LLVM IR, struct Configure is used as argument when
 * generating LLVM IR.
 */
#ifndef __CODEGEN_H
#define __CODEGEN_H

#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <string.h>
#include "ParsingTree.h"
#include "SymbolTable.h"

extern SymbolTable symbolTable;

struct Configure
{
	string type;
	string scope;
	string structName;
};

class CodeGenContext
{
private:
	void EXTDEFS_genCode(TreeNode*);
	void EXTDEF_genCode(TreeNode*);
	void EXTVARS_genCode(TreeNode*);
	void EXTVARS_genCode(TreeNode*,Configure&);
	string SPEC_genCode(TreeNode*);
	string STSPEC_genCode(TreeNode*);
	void OPTTAG_genCode(TreeNode*);
	void VAR_genCode(TreeNode*,Configure&);
	void FUNC_genCode(TreeNode*,Configure&);
	//void PARAS_genCode(TreeNode**);
	//void PARA_genCode(TreeNode**);
	void STMTBLOCK_genCode(TreeNode*);
	void STMTS_genCode(TreeNode*);
	void STMT_genCode(TreeNode*);
	void ESTMT_genCode(TreeNode*);
	void DEFS_genCode(TreeNode*);
	void DEF_genCode(TreeNode*);
	void DECS_genCode(TreeNode*,Configure&);
	void DEC_genCode(TreeNode*,Configure&);
	void INIT_genCode(TreeNode*,Configure&,string name="");
	char* EXP_genCode(TreeNode*);
	char* FEXP_genCode(TreeNode*);
	char* CondFEXP_genCode(TreeNode*);//condition expression
	char* CondEXP_genCode(TreeNode*);
	void ARRS_genCode(TreeNode*);
	char* ARGS_genCode(TreeNode*);

public:
	CodeGenContext(string fname) 
	{
		init();
		anonymousStructNumber = 1;
		fout.open(fname,ofstream::out);
		openFileName = fname;
	}
	~CodeGenContext() {fout.close();}

	void generateCode(ParsingTree& pt);

	inline void init()
	{
		tempRegNumber = 1;
		ifNumber = ifelseNumber = forNumber = bexprNumber = 0;
	}

private:	
	std::ofstream fout;
	string openFileName;
	int anonymousStructNumber;
	int tempRegNumber;
	int ifNumber;
	int ifelseNumber;
	int forNumber;
	int bexprNumber;

	void close()
	{
		fout.close();
		fout.open(openFileName);
		fout<<"Error.";
		fout.close();
	}

	std::vector<std::string> scopeAndIsarg(std::string name)
	{
		std::vector<std::string> ans = {"%",""};
		std::string scope = symbolTable.find(name)->second.scope;
		if(scope == "global")
			ans[0] = "@";
		else if(scope == "arg")
			ans[1] = ".addr";

		return ans;
	}

	inline std::vector<std::string> scopeAndIsarg(TreeNode* node)
	{
		return scopeAndIsarg(node->name);
	}
};
#endif //"CODEGEN_H"
