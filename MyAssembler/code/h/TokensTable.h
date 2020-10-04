#ifndef HEADERS_TOKENSTABLE_H_
#define HEADERS_TOKENSTABLE_H_

#include <vector>
#include "myscanner.h"
#include <string>

extern "C" int yylex();
extern int yylineno;
extern char* yytext;

class TokensTable {
public:
	TokensTable() = default;
	virtual ~TokensTable() = default;

	void addToken(Token token, std::string string);
	void addTokens();
	void first();
	int next();
	int prev();
	void last();
	std::pair<Token, std::string> get();
	std::vector<std::pair<Token, std::string>> getLine(); //throws exception if iterator == tokensVct.end();
	void prevLine();

	friend std::ostream& operator<<(std::ostream& os, const TokensTable& tt);
private:
	std::vector<std::pair<Token, std::string>> tokensVct;
	std::vector<std::pair<Token, std::string>>::iterator iterator = tokensVct.begin();
	std::vector<std::pair<Token, std::string>> line;
};

#endif /* HEADERS_TOKENSTABLE_H_ */
