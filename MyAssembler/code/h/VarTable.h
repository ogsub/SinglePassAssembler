#ifndef VARTABLE_H_
#define VARTABLE_H_

#include <string>
#include <vector>
#include "RelocationTable.h"
#include "BinaryTable.h"

class VarTable {
public:
	enum Visibility {
		GLOBAL, LOCAL
	};

	struct FLink{
		int addr;
		bool sign; //true - positive, false - negative
		int size;	//velicina 0 - 1 B, 1 - 2 B, da znam kolko da upisem kad budem upisivao

		bool variable;		//true - znaci da se relokacioni zapis odnosi na simbol u var tabeli, sto znaci da je offset u stvari ID simbola iz tabele
		bool pcRel;			//da znam dal je u pitanju pcRel adresiranje, jer se u tom slucaju vrednost promenljive dodaje

		unsigned int section = 0;

		FLink(int addr, bool sign, int size): addr(addr), sign(sign), size(size), variable(false), pcRel(false){}
		FLink(int addr, bool sign, int size, bool variable, bool pcRel = false): addr(addr), sign(sign), size(size), variable(variable), pcRel(pcRel){}
		void setPCRel(){
			pcRel = true;
		}

		void setSection(unsigned int section){
			this->section = section;
		}
	};

	struct Field {
		std::string name;			//ime promenljive
		bool valid;
		unsigned int section;//broj sekcije
		short value;			//vrednost
		Visibility visibility;		//true -> global, false -> local
		unsigned int id;				//koji po redu
		bool externVar;		//extern flag
		std::vector<FLink> fLink;

		unsigned int links = 0;
		unsigned int linked = 0;

		//koriste se za sekcije
		int size;//koristi se samo za simbole koji su sekcije, =-1 za polja koja nisu sekcije
		BinaryTable binary;
		RelocationTable relocationTable;

		static unsigned int idGenerator;

		Field(std::string name) :
				name(name), valid(false), section(0), value(0), visibility(Visibility::LOCAL),
				id(idGenerator++), externVar(false), size(-1) {
		}

		const RelocationTable& getRelocationTable() const{
			return relocationTable;
		}
	};
	VarTable();
	virtual ~VarTable();

	std::vector<Field>::iterator exists(std::string name);
	std::vector<VarTable::Field>::iterator exists(unsigned int id);

	int add(const std::string &name);//po defaultu je svaka variabla pri dodavanju lokalna
	void setValid(const std::string &name);
	void setValid(unsigned int id);
	void setSection(const std::string &name, int section);
	void setValue(const std::string &name, int value);
	void setNum(const std::string &name, int num);
	void setExtern(const std::string &name);
	void setGlobal(const std::string &name);
	void setSize(const std::string &name, int size);
	void addSection(const std::string &name);

	std::vector<Field>::iterator getEnd();
	unsigned short getValue(const std::string &name);
	unsigned short getValue(unsigned int id);
	std::string getName(unsigned int id);
	unsigned int getId(const std::string &name);
	Field& getEntry(const std::string &name);
	Field& getEntry(unsigned int id);

	void addValue(unsigned int id, short value, bool sign);
	void addLinked(unsigned int id);
	void addLinks(unsigned int id);
	void addLinked(std::string &name);
	void addLinks(std::string &name);
	bool isValid(unsigned int id);
	void checkSameSection(unsigned int offset, unsigned int id);

	void fLinkLink(const std::string &name);
	void fLinkLink(unsigned int id);

	BinaryTable& getBinary();
	BinaryTable& getBinary(unsigned int currSection);
	RelocationTable& getRelocationTable();
	std::vector<Field>& getVarTable();
	unsigned int getCurrSection();
	void checkVarValidity();

//	void updateEQU();

	friend std::ostream& operator<<(std::ostream& os, const VarTable& vt);
private:
	std::vector<Field> varTable;
	unsigned int currSection = 0;
};

#endif /* VARTABLE_H_ */
