#ifndef ASSEMBLY_H_
#define ASSEMBLY_H_

#include "VarTable.h"
#include <fstream>
#include "TokensTable.h"
#include "InstrDescr.h"
#include "myscanner.h"
#include "RelocationTable.h"

extern FILE *yyin;

class Assembly {
public:
	Assembly();
	Assembly(std::string fileName);
	virtual ~Assembly();

	void translate();
	void printVarTable();
	void writeToFile(const std::string &name);
	void defineLabel(bool& labelDefinitionMode, std::vector<std::pair<Token, std::string>>::iterator& currElem,
			const std::vector<std::pair<Token, std::string>> &currLine);
	void shortenOperands(int oneByteSize, int numImmed, int numReg, int numRegOffset, const std::string &instruction);
	void checkSameSection(unsigned int offset, unsigned int section);
	void checkVarValidity();
//	void updateEQURelTable(VarTable& varTable);
private:
	void translateInstruction(
			std::vector<std::pair<Token, std::string>>::iterator& currElem,
			const std::vector<std::pair<Token, std::string>> &currLine);
	void translateDirective(std::vector<std::pair<Token, std::string>>::iterator &currElem,
	const std::vector<std::pair<Token, std::string>> &currLine);

	TokensTable tokensTable;
	InstrDescr instructions;
	VarTable varTable;
};

#endif /* ASSEMBLY_H_ */
