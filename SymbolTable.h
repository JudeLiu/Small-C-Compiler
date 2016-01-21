/** 
 *  @file SymbolTable.h
 *
 *  @brief This header declares two classes,'Symbol' and 'SymbolTable'
 */
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H 1
#include <unordered_map>
#include <string>
using namespace std;

/** 
 *  @brief describe a symbol
 */
class Symbol
{
public:
	string name;	//name of the symbol
	string type;
	string scope;	
	int arrSize;	//vector<int> arrSize;
	int paraNum;	//parameter number
	string structName;
	int structIdx;

	Symbol(string n,string t="int",string s="local",int as=1,int para=0) : 
		name(n),
		type(t),
		scope(s),
		arrSize(as),
		paraNum(para),
		structName("")
	{}
	
	bool operator==(const Symbol& sym) const 
		{ return name==sym.name && type==sym.type && scope==sym.scope; }
};

//maintain a hash table, key is the name of a symbol, value is the corresponding struct Symbol of it.
class SymbolTable
{
private:
	unordered_map<string,Symbol> hash;
	
public:
	typedef unordered_map<string,Symbol>::iterator iterator;
	
	iterator begin()
	{ hash.begin();	}
	
	iterator end()
	{ return hash.end();}
	
	void insert(const Symbol& sym)
	{
		hash.insert({sym.name,sym});
	}
	
	bool exist(const string& name)
	{
		return (hash.find(name) == hash.end())? false : true ;
	}
	
	iterator find(const string& name)
	{
		return hash.find(name);
	}
	
	iterator find(const Symbol& sym)
	{
		return hash.find(sym.name);
	}
};

#endif// SYMBOLTABLE_H
