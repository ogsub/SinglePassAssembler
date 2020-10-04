#include "TokensTable.h"
#include <algorithm>
#include <iostream>
#include <exception>

void TokensTable::addToken(Token token, std::string string){
	this->tokensVct.emplace_back(token, string);
}

void TokensTable::addTokens(){
	int ntoken;
	ntoken = yylex();												//yylex() matchuje i to stavlja u yytext, a vraca vrednost koju smo mi definisali da vraca u .l fajlu
	while(ntoken) {
		std::cout << names[ntoken] << " - " << yytext << "\n";		//yytext - ono sto je matchovano
		addToken(static_cast<Token>(ntoken), std::string(yytext));
		ntoken = yylex();											//kad dodje do .end vraca 0, tako smo definisali u .l
	}
	std::cout<<"**************************************************\n";
	first();
}

void TokensTable::first(){
	iterator = tokensVct.begin();
}

void TokensTable::last(){
	iterator = tokensVct.end();
}

int TokensTable::next(){
	iterator++;
	if(iterator == tokensVct.end())
		return 0;
	return 1;
}
int TokensTable::prev(){
	if(iterator == tokensVct.begin())
		return 0;
	iterator--;
	return 1;
}

std::pair<Token, std::string> TokensTable::get(){
	return *iterator;
}

std::vector<std::pair<Token, std::string>> TokensTable::getLine(){
	line.clear();
	if(iterator == tokensVct.end()){
		//throw std::exception();
		return line;
	}

	//preskace prazan red
	while(iterator->first == BREAK_LNE){
		this->next();
	}

	if(iterator == tokensVct.end()){
		return line;
	}

	//vraca sve tokene iz istog reda
	while(iterator->first != BREAK_LNE){
		line.push_back(*iterator);
		if(!this->next())
			break;
	}
	this->next(); //preskace \n
	return line;
}

void TokensTable::prevLine(){
	int sizeOfLine = line.size();
	for(int i = 0; i <= sizeOfLine; i++){
		prev();
	}
}

std::ostream& operator<<(std::ostream& os, const TokensTable& tt){
	for(const std::pair<Token, std::string>& s: tt.tokensVct){
		os << names[s.first] << "-" << s.second << "\n";
	}
	return os;
}
