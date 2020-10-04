#include <iostream>
#include <stdio.h>
#include <fstream>
#include "myscanner.h"
#include "TokensTable.h"
#include "InstrDescr.h"
#include "Assembly.h"

const char *names[] = {"END", "GLOBAL", "EXTERN", "SECTION", "BREAK_LNE", "SEPARATOR", "DIRECTIVE", "LABEL", "INSTRUCTION", "LITERAL", "REG", "SYMBOL", "SPEC_CHAR", "SIGN"};

int main(int argc, char* argv[]) {
//	extern FILE *yyin;
//
//	TokensTable tokensTable;
//	InstrDescr instructions;
//
//
//	yyin = fopen("/home/student/Documents/SS/Projekat SS/prviZadatak/source/input.s", "r");
//
//	if(yyin!=nullptr)
//		std::cout<<"Okej je!\n";
//	else{
//		std::cout<<"Greska! Fajl nije otvoren\n";
//		return -1;
//	}
//
//	tokensTable.addTokens();
//
//	std::cout<<"----------------------------------------------------------------\n";
//	std::cout << tokensTable;
//	std::cout<<"----------------------------------------------------------------\n";
//
//	std::vector<std::pair<Token, std::string>> linija = tokensTable.getLine();
//	for(const auto& i: linija){
//		std::cout<<i.first;
//		std::cout<<i.second;
//	}
//
//	std::cout<<"\n-------------------------------------------------------------\n";
//	std::cout<< static_cast<int>(instructions.getOpCode("jeq"));
//	std::cout << static_cast<int>(instructions.getOperandNum("jeq"));
//
//	fclose(yyin);
	if(argc != 4){
		std::cout<<"Usage: program_name -o output_filename input_filename\n";
		exit(-1);
	}
	else{
		if(std::string(argv[1]) != "-o"){
			std::cout<<"Pogresna opcija\n";
			exit(-1);
		}
		Assembly assembly(argv[3]);
		assembly.translate();
		assembly.checkVarValidity();
		std::cout<<"**************************\n";
		assembly.printVarTable();
		std::cout<<"\n-------Sve super------------------------------------------------------\n";
		assembly.writeToFile(argv[2]);
		return 0;
	}
}
