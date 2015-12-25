# Makefile

FILES	=  ParsingTree.cpp lex.yy.cpp y.tab.cpp ./codegen/codegen.cpp main.cpp
CFLAGS	= -std=c++11 -I. -I..

scc: $(FILES)
	g++ -o scc $(FILES) $(CFLAGS)
	
y.tab.cpp: SyntaxAnalyser.y
	bison -o y.tab.cpp -d SyntaxAnalyser.y
	
lex.yy.cpp:LexicalAnalyser.l
	flex -o lex.yy.cpp LexicalAnalyser.l
	
clean:
	-rm -f scc 
	-rm -f *.tab.* 
	-rm -f lex.yy.*
