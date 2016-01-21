/**
 *  @file ParsingTree.h
 *
 *  @brief This header declares two classes, 'TreeNode' and 'ParsingTree', included by flex and bison files, and also main.cpp
 */
#ifndef PARSINGTREE_H
#define PARSINGTREE_H
#include <string>
#include <cstdarg>
#include <queue>
#include <iostream>
#include <stack>
#include <fstream>
using namespace std;

extern int yylineno;

/**
 * @brief describe the node of AST(abstract syntax tree)
 */
class TreeNode
{
	typedef string Type;	
public:
	TreeNode* child;
	TreeNode* sibling;

	int lineno;
	Type node_type;//can be dec stmt exp for if ... etc
	Type type;//can be int,float,etc. (in prj1 can only be int)
	
	//union{
		//float fVal;
		int iVal;
		string name;
	//};//attribute

	TreeNode(int l=-1):child(nullptr),sibling(nullptr)
	{
		lineno = l;
		node_type = type = "";
	}
	~TreeNode() {}
};

/** 
 *  @brief describe the AST
 */
class ParsingTree
{
private:
	string indent;
	ofstream fout;
	
	/**
	 *  @brief recursively preorder traverse the AST
 	*/
	void __printTree(TreeNode*);
	
public:
	TreeNode* root;
	
	ParsingTree(){indent="";}
	~ParsingTree() {fout.close();}
	
	/**
	 *  @brief create a new node, and link all the nodes in the arguments as its child(ren)
	 *  @param nodeType type of the node, can be 'defs','decs','stmts',etc
	 *  @param argNum number of the rest arguments
	 *  @param ... variable arguments, type should be TreeNode*
	 *  @return the new node created by this function
	 */
	TreeNode* link(string nodeType,int argNum,...);
	
	/**
	 *  @brief Wrapper of private function __printTree, which is a recursive function
	 */
	void printTree(char*);
};

#endif