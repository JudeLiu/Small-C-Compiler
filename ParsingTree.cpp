/** 
 *  @file ParsingTree.cpp
 *
 *  @brief implementation of ParsingTree.h
 */
#include "ParsingTree.h"

/**
 *  @brief create a new node, and link all the nodes in the arguments as its child(ren)
 *
 *  @param nodeType type of the node, can be 'defs','decs','stmts',etc
 *  @param argNum number of the rest arguments
 *  @param ... variable arguments, type should be TreeNode*
 *
 *  @return the new node created by this function
 */
TreeNode* ParsingTree::link(std::string nodeType, int argNum,...)
{
	TreeNode* ret=new TreeNode(yylineno);//set lineno=yylineno
	ret->node_type = nodeType;
	
	va_list nodeList;
	va_start(nodeList,argNum);
	
	if(argNum > 0)
	{	
		TreeNode* cur=va_arg(nodeList,TreeNode*);
		argNum--;
		ret->child = cur;
	
		if(argNum > 0)
		{
			TreeNode* prev;
			while(argNum > 0)
			{
				prev = cur;
				cur = va_arg(nodeList,TreeNode*);
				prev->sibling = cur;
				argNum--;
			}
		}
	}
	
	va_end(nodeList);
	return ret;
}

/**
 *  @brief Wrapper of private function __printTree, which is a recursive function
 */
void ParsingTree::printTree(char* filename)
{
	//fout.open(filename,ios::app);
	fout.open(filename);
	
	fout<<"Parsing tree:\n";
	__printTree(root);
}

/**
 *  @brief recursively preorder traverse the AST
 */
void ParsingTree::__printTree(TreeNode* root)
{
	if(root==nullptr)	return;
	
	if(root->node_type=="ID" && root->name!="")
		fout<<indent<<'|'<<root->node_type<<'('<<root->name<<')'<<endl;
	else if(root->node_type=="CONST" || root->node_type=="ERROR_CONST")
		fout<<indent<<'|'<<root->node_type<<'('<<root->iVal<<')'<<endl;
	else
		fout<<indent<<'|'<<root->node_type<<endl;	
	
	indent += "--";
	__printTree(root->child);
	
	indent.erase(indent.length()-2,indent.length());//delete the last "\t"
	__printTree(root->sibling);
}
