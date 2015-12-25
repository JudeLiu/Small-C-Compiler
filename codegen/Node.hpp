#ifndef __NODE_HPP
#define __NODE_HPP

#include <iostream>
#include <vector>

#include <stdint.h> // for int16_t

class CodeGenContext;
class FunctionNode;
class StmtNode;

typedef std::vector<FunctionNode*> FunctionNodeList;
typedef std::vector<StmtNode*> StmtNodeList;
typedef std::vector<std::string> ArgumentList;

/* AST Node */
class Node
{
	virtual ~Node() {}
	virtual void codeGen(CodeGenContext& ctx) = 0;
};

class FunctionNode : public Node
{
	friend Program;

private:
	std::string m_name;
	ArgumentList m_arguments;
	bool m_void;
	StmtNodeList m_stmts;

public:
	std::string name() { return m_name; }
	ArgumentList arguments() { return m_arguments; }
	bool isVoid() { return m_void; }
	StmtNodeList stmts() { return m_stmts;}

	static FunctionNode* mainFunction(const StmtNodeList& stmts)
	{
		return FunctionNode::voidFunction(std::string(main),ArgumentList(),stmts);
	}

	static FunctionNode* nonVoidFunction(const std::string& name,const ArgumentList& arguments, const StmtNodeList& stmts)
	{
		FunctionNode* f = FunctionNode::voidFunction(name,arguments,stmts);
		f->m_void = false;
		return f;
	}

	static FunctionNode* voidFunction(const std::string& name, const ArgumentList& arguments,const StmtNodeList& stmts)
	{
		FunctionNode* f = new FunctionNode();
		f->m_name = name;
		f->m_void = true;
		f->m_arguments = arguments;
		f->m_stmts = stmts;
		return f;
	}

	virtual ~FunctionNode() {}
	virtual void codeGen(CodeGenContext& context);
};

////////statements///////////

class StmtNode : public Node
{};

class ReturnStmtNode : public StmtNode
{
	virtual ~ReturnStmtNode() {}
	virtual void codeGen(CodeGenContext& ctx);
};

class VarDecStmtNode : public StmtNode
{
private:
	std::string m_name;
	int16_t m_number;

public:
	virtual ~VarDecStmtNode() {}
	VarDecStmtNode(std::string& name, int16_t number)
	{
		m_name = name;
		m_number = number;
	}

	virtual void codeGen(CodeGenContext& ctx);
};

////////program///////////
class Program
{
public:
	FunctionNodeList m_functions;
	Program(FunctionNodeList& functions)
	{
		m_functions = functions;
	}

	FunctionNode* getMainFunction()
	{
		for(auto &it : m_functions)
		{
			if(it->m_name == "main")
				return it;
		}
	std::cerr<<"No main function!\n";
	return nullptr;
	}

	FunctionNodeList getAuxiliaryFunctions()
	{
		FunctionNodeList functions;
		for(auto &it : m_functions)
		{
			if(it->m_name != "main")
				functions.push_back(it);
		}

		return functions;
	}

	virtual ~Program() {}
};

////////expr//////
class ExprNode
{
public:
	virtual ~ExprNode() {}
	virtual void codeGen(CodeGenContext& ctx) = 0;
}
#endif //__NODE_HPP