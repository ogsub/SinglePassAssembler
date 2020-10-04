#include "RelocationTable.h"
#include <algorithm>

RelocationTable::RelocationTable() {
	// TODO Auto-generated constructor stub

}

RelocationTable::~RelocationTable() {
	// TODO Auto-generated destructor stub
}

void RelocationTable::addEntry(unsigned int offset, AddrType addrType, unsigned int varTableEntry, bool sectionOrId, bool variable){
	relocationTable.emplace_back(offset, addrType, varTableEntry, sectionOrId, variable);
}

bool RelocationTable::isEmpty() const{
	return this->relocationTable.empty();
}

RelocationTable::Field& RelocationTable::getBack(){
	return this->relocationTable.back();
}

std::vector<RelocationTable::Field>& RelocationTable::getRelocationTable(){
	return this->relocationTable;
}

const std::vector<RelocationTable::Field>& RelocationTable::getRelocationTable() const{
	return this->relocationTable;
}

std::ostream& operator<<(std::ostream& os, const RelocationTable &rc){
	if(!rc.isEmpty()){
		os << "#offset\t\t" << "tip\t\t" << "vrednost\t\t" << "variable\t\t\n";
		for(const auto& elem : rc.relocationTable){
			os  << elem.offset << "\t\t"<< (elem.addrType == 0 ? "R_386_32" : "R_386_PC32") << "\t\t";
			if(!elem.sectionOrId){
				os << elem.varTableEntry <<"s";
			}
			else{
				os << 99;
			}
			os <<"\t\t"<< elem.variable <<"\n";
		}
	}
	return os;
}
