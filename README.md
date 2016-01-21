# Small-C-compiler #
It is a small-c(a subset of c language) compiler.

<del>Actually it is a compiler project of cs215 SJTU.</del>

## List of items ##

* codegen/codegen.* : Files that generate LLVM IR, major work of project 2
* LexicalAnalyser.l : tokenizer, part of frondend
* SyntaxAnalyser.y : parser, part of frontend
* ParsingTree.* : data structures of abstract parsing tree(AST)
* SymbolTable.h : data structure of symbol table
* main.cpp : main function of this compiler
* makefile
* README.md
* Project2 Report.pdf

## Build ##
enter:

		make

## Run ##
	
		./scc SourceFileName TargetFileName

