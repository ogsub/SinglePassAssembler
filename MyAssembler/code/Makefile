asembler : main.cpp myscanner.h lex.yy.c TokensTable.h TokensTable.cpp InstrDescr.cpp InstrDescr.h Assembly.cpp Assembly.h VarTable.cpp VarTable.h BinaryTable.cpp BinaryTable.h RelocationTable.cpp RelocationTable.h
	gcc main.cpp lex.yy.c TokensTable.cpp InstrDescr.cpp Assembly.cpp VarTable.cpp BinaryTable.cpp RelocationTable.cpp -ll -lstdc++ -o asembler 

lex.yy.c: myscanner.l
	lex myscanner.l

clean:
	rm myscanner
	rm lex.yy.c