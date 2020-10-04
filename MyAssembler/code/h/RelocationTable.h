#ifndef RELOCATIONTABLE_H_
#define RELOCATIONTABLE_H_

#include <vector>
#include <iostream>

class RelocationTable {
public:
	RelocationTable();
	virtual ~RelocationTable();

	enum AddrType {R_386_32, R_386_PC32}; //apsolutno adresiranje i pc relativno adresiranje (respektivno)

	struct Field{
		unsigned int offset;
		AddrType addrType;
		unsigned int varTableEntry;
		bool sectionOrId;

		bool variable;		//true - znaci da se relokacioni zapis odnosi na simbol u var tabeli, sto znaci da je offset u stvari ID simbola iz tabele

		Field(unsigned int offset, AddrType addrType, unsigned int varTableEntry, bool sectionOrId): offset(offset), addrType(addrType), varTableEntry(varTableEntry), sectionOrId(sectionOrId),variable(false){};
		Field(unsigned int offset, AddrType addrType, unsigned int varTableEntry, bool sectionOrId, bool variable): offset(offset), addrType(addrType), varTableEntry(varTableEntry), sectionOrId(sectionOrId),variable(variable){};
		//Field(unsigned int offset, AddrType addrType, unsigned int& id): offset(offset), addrType(addrType), id(id), variable(false){};
		//Field(unsigned int offset, AddrType addrType, unsigned int& id, bool variable): offset(offset), addrType(addrType), id(id), variable(variable){};
//		Field(const Field& f): id(f.id) {
//			offset = f.offset;
//			addrType = f.addrType;
//			id = f.id;
//			variable = f.variable;
//		}


//		Field& operator=(const Field& f) {
//			offset = f.offset;
//			addrType = f.addrType;
//			id = f.id;
//			return *this;
//		}

		void setAddrType(AddrType addrType){
			this->addrType = addrType;
		}
	};

//	void addEntry(unsigned int offset, AddrType addrType, unsigned int& id, bool variable = false);
	void addEntry(unsigned int offset, AddrType addrType, unsigned int varTableEntry, bool sectionOrId, bool variable = false);
	bool isEmpty() const;
	Field& getBack();
	std::vector<Field>& getRelocationTable();
	const std::vector<Field>& getRelocationTable() const;


	friend std::ostream& operator<<(std::ostream& os, const RelocationTable &rc);
private:
	std::vector<Field> relocationTable;
};

#endif /* RELOCATIONTABLE_H_ */
