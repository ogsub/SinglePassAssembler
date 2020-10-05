assembler : src/main.cpp h/myscanner.h src/lex.yy.c h/TokensTable.h src/TokensTable.cpp src/InstrDescr.cpp h/InstrDescr.h src/Assembly.cpp h/Assembly.h src/VarTable.cpp h/VarTable.h src/BinaryTable.cpp h/BinaryTable.h src/RelocationTable.cpp h/RelocationTable.h
	gcc -I ./h/ src/main.cpp src/lex.yy.c src/TokensTable.cpp src/InstrDescr.cpp src/Assembly.cpp src/VarTable.cpp src/BinaryTable.cpp src/RelocationTable.cpp -ll -lstdc++ -o assembler 

lex.yy.c: ./flex/myscanner.l
	lex flex/myscanner.l

clean:
	rm assembler