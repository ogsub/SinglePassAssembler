#include "VarTable.h"
#include <algorithm>
#include <iostream>

VarTable::VarTable() {
	this->varTable.emplace_back("UND");
	this->setSize("UND", 0);
}

VarTable::~VarTable() {
	// TODO Auto-generated destructor stub
}

unsigned int VarTable::Field::idGenerator = 0;

std::vector<VarTable::Field>::iterator VarTable::exists(std::string name) {
	auto it = std::find_if(this->varTable.begin(), this->varTable.end(),
			[&name](const Field &f) {
				if (f.name == name)
					return 1;
				else
					return 0;
			});

	//ovo sam morao da zakomentiram jer bi ovaj if kod add bio netacan.
	//if(it == this->globalVarTable.end())
	//	return static_cast<std::vector<GlobalVarTable::Field>::iterator>(0);
	return it;
}

std::vector<VarTable::Field>::iterator VarTable::exists(unsigned int id) {
	auto it = std::find_if(this->varTable.begin(), this->varTable.end(),
			[&id](const Field &f) {
				if (f.id == id)
					return 1;
				else
					return 0;
			});

	//ovo sam morao da zakomentiram jer bi ovaj if kod add bio netacan.
	//if(it == this->globalVarTable.end())
	//	return static_cast<std::vector<GlobalVarTable::Field>::iterator>(0);
	return it;
}

int VarTable::add(const std::string &name) { //returns negative number if symbol name already exists
	auto it = this->exists(name);

	if (it != this->varTable.end())
		return -1;
	else {
		this->varTable.emplace_back(name);
		return 0;
	}
}

void VarTable::setValid(const std::string &name){
	auto it = this->exists(name);
	if (it != this->varTable.end() && it->section != 0) {
		it->valid = true;
	} else {
		std::cout<<"Nema te vrednosti u var tabeli\n";
		exit(-1);
	}
}

void VarTable::setValid(unsigned int id){
	auto it = std::find_if(this->varTable.begin(), this->varTable.end(),
				[&id](const Field &f) {
					if (f.id == id)
						return 1;
					else
						return 0;
				});
	if (it != this->varTable.end() && it->section != 0) {
		it->valid = true;
	}
	else {
		std::cout<<"Nema te vrednosti u var tabeli\n";
	}
}

void VarTable::setSection(const std::string &name, int section) {
	auto it = this->exists(name);
	if (it != this->varTable.end() && it->section == 0) {
		it->section = section;
	} else {
		throw std::exception();
	}
}

void VarTable::setValue(const std::string &name, int value) {
	auto it = this->exists(name);
	if (it != this->varTable.end()) {
		it->value = value;
	} else {
		throw std::exception();
	}
}

void VarTable::setNum(const std::string &name, int num) {
	auto it = this->exists(name);
	if (it != this->varTable.end()) {
		it->id = num;
	} else {
		throw std::exception();
	}
}

void VarTable::setExtern(const std::string &name) {
	auto it = this->exists(name);
	if (it != this->varTable.end()) {
		it->externVar = true;
		it->visibility = Visibility::GLOBAL;
		it->valid = true;
	} else {
		throw std::exception();
	}
}

void VarTable::setGlobal(const std::string &name) {
	auto it = this->exists(name);
	if (it != this->varTable.end()) {
		it->visibility = Visibility::GLOBAL;
	} else {
		throw std::exception();
	}
}

void VarTable::setSize(const std::string &name, int size) {
	auto it = this->exists(name);
	if (it != this->varTable.end()) {
		it->size = size;
	} else {
		throw std::exception();
	}
}

void VarTable::addSection(const std::string &name) {		
	auto it = std::find_if(this->varTable.begin(), this->varTable.end(),
			[](const Field &f) {
				if (f.size == -1) //vraca iterator na prvi element koji nije sekcija, da bi na tacno to mesto upisali sekciju
					return true;
				return false;
			});

	VarTable::Field newSection(name);
		newSection.size = 0;
		newSection.visibility = Visibility::LOCAL;
	if(it != this->varTable.end()) {	//u slucaju da postoje neki simboli u varTable, sekciju ubacujemo pre njih i onda ide ono menjanje vrednosti id simbola, treba nam slucaju za kad nema simbola pre ubacivanja sekcije
		newSection.id = it->id;
	}
	newSection.section = newSection.id;


	varTable.insert(it, newSection);

	this->currSection = newSection.section;

	//update ujemo id, za jedan veci od svih simbola koji su u tabeli nakon dodate sekcije
	std::for_each(this->varTable.begin(), this->varTable.end(), [](Field &f) {
		if(f.size == -1){
			f.id++;
		}
	});

	//ovo radi update zapisa u relokativnoj tabeli za one zapise koje se odnose na .equ direktivu
	std::for_each(this->varTable.begin(), this->varTable.end(), [](Field &f) {
		std::for_each(f.relocationTable.getRelocationTable().begin(), f.relocationTable.getRelocationTable().end(), [](RelocationTable::Field& f2){
			if(f2.variable){
				f2.offset++;
			}
		});
	});

	std::for_each(this->varTable.begin(), this->varTable.end(), [](Field &f) {
		std::for_each(f.relocationTable.getRelocationTable().begin(), f.relocationTable.getRelocationTable().end(), [](RelocationTable::Field& f2){
			f2.varTableEntry++;
		});
	});
	//ovde takodje treba proci kroz rel. table i ispraviti potencijalno ako postoji nesto sto treba da se ispravi
}

BinaryTable& VarTable::getBinary(){
	return this->varTable[this->currSection].binary;
}

BinaryTable& VarTable::getBinary(unsigned int currSection){
	return this->varTable[currSection].binary;
}

RelocationTable& VarTable::getRelocationTable(){
	return this->varTable[this->currSection].relocationTable;
}

std::vector<VarTable::Field>& VarTable::getVarTable(){
	return this->varTable;
}

unsigned int VarTable::getCurrSection(){
	return this->currSection;
}

void VarTable::checkVarValidity(){
	std::for_each(this->varTable.begin(), this->varTable.end(), [](VarTable::Field& f){
		if(f.size == -1){
			if(f.valid == false){
				std::cout<<"Promenljiva nije validna\n";
				exit(-1);
			}
		}
	});
}

std::vector<VarTable::Field>::iterator VarTable::getEnd(){
	return this->varTable.end();
}

unsigned short VarTable::getValue(const std::string &name){
	auto it = this->exists(name);
	if(it != this->varTable.end()){
		return it->value;
	}
	std::cout<<"Greska102\n";
	exit(-1);
}

unsigned short VarTable::getValue(unsigned int id){
	auto it = this->exists(id);
	if(it != this->varTable.end()){
		return it->value;
	}
	std::cout<<"Greska102\n";
	exit(-1);
}

std::string VarTable::getName(unsigned int id){
	auto it = this->exists(id);
	if(it != this->varTable.end()){
		return it->name;
	}
	std::cout<<"Greska102\n";
	exit(-1);
}

unsigned int VarTable::getId(const std::string &name){
	auto it = this->exists(name);
	if(it != this->varTable.end()){
		return it->id;
	}
	std::cout<<"Greska102\n";
	exit(-1);
}

VarTable::Field& VarTable::getEntry(const std::string &name){
	auto it = this->exists(name);
	if(it != this->varTable.end()){
		return *it;
	}
	std::cout<<"Greska112\n";
		exit(-1);
}

VarTable::Field& VarTable::getEntry(unsigned int id){
	auto it = this->exists(id);
	if(it != this->varTable.end()){
		return *it;
	}
	std::cout<<"Greska112\n";
		exit(-1);
}

void VarTable::addValue(unsigned int id, short value, bool sign){
	auto it = exists(id);


	if(it != this->varTable.end()){
		it->value = it->value + (sign? value : -value);
	}
}

void VarTable::addLinked(unsigned int id){
	auto it = exists(id);
	if(it != this->varTable.end()){
			it->linked++;
		}
}

void VarTable::addLinks(unsigned int id){
	auto it = exists(id);
	if(it != this->varTable.end()){
			it->links++;
		}
}

void VarTable::addLinked(std::string &name){
	auto it = this->exists(name);
	if(it != this->varTable.end()){
			it->linked++;
		}
}

void VarTable::addLinks(std::string &name){
	auto it = this->exists(name);
	if(it != this->varTable.end()){
			it->links++;
		}
}

bool VarTable::isValid(unsigned int id){
	auto it = exists(id);
	return it->linked == it->links;
}

void VarTable::checkSameSection(unsigned int offset, unsigned int id){
	std::for_each(this->varTable.begin(), this->varTable.end(), [&](Field& f){
		std::for_each(f.relocationTable.getRelocationTable().begin(), f.relocationTable.getRelocationTable().end(), [&](RelocationTable::Field& f2){
			if(f2.variable && f2.offset != offset){	//gledamo samo ako je u pitanju variabla nastala .equ
				unsigned int myId;
				if(f2.sectionOrId){
					myId = this->varTable[f2.varTableEntry].section;	//ekvivalent sekciji
				}
				else{
					myId = f2.varTableEntry;	//ekvivalent id-ju promenljive
				}
				if(myId != id) {
					std::cout << "Greska, nije dozvoljeno da se u .equ koriste promenljive iz razlicitih sekcija\n";
					exit(-1);
				}
			}
		});
	});
}

void VarTable::fLinkLink(const std::string &name){
	auto it = this->exists(name);
	std::for_each(it->fLink.begin(), it->fLink.end(), [&](VarTable::FLink& fLink){
		//slucaj kada fLink nije ubacen zbog .equ direktive
		if(!fLink.variable){
			if(fLink.size == 0){
				this->getBinary(fLink.section).setOperandByte(this->getValue(name), fLink.addr, fLink.pcRel);	//kad je pcRel argument == true, radimo sabiranje sa trenutnom vrednosti elementa, a ne samo upis preko trenutne vrednosti
			}
			else{
				this->getBinary(fLink.section).setOperandBytes(this->getValue(name), fLink.addr, fLink.pcRel);
			}
		}
		//slucaj kada je fLink ubacen zbog .equ direktive, i sada updateujemo vrednosti u varTabeli
		else {
			checkSameSection(fLink.addr, it->section);
			this->addValue(fLink.addr, this->getValue(name), fLink.sign);
			this->addLinked(fLink.addr);
			if(this->isValid(fLink.addr)){
				this->setValid(fLink.addr);
				//sad treba da resavamo njegove fLinkove, najbolje da oba budu neke funkcije koje cemo da pozivamo mozda i rekurzivno
				this->fLinkLink(fLink.addr);
			}
		}
	});

	it->valid = true;
}

void VarTable::fLinkLink(unsigned int id){
	auto it = this->exists(id);
	std::for_each(it->fLink.begin(), it->fLink.end(), [&](VarTable::FLink& fLink){
		//slucaj kada fLink nije ubacen zbog .equ direktive
		if(!fLink.variable){
			if(fLink.size == 0){
				this->getBinary().setOperandByte(this->getValue(id), fLink.addr, fLink.pcRel);
			}
			else{
				this->getBinary().setOperandBytes(this->getValue(id), fLink.addr, fLink.pcRel);
			}
		}
		//slucaj kada je fLink ubacen zbog .equ direktive, i sada updateujemo vrednosti u varTabeli
		else {
			this->addValue(fLink.addr, this->getValue(id), fLink.sign);
			this->addLinked(fLink.addr);
			if(this->isValid(fLink.addr)){
				this->setValid(fLink.addr);
				//sad treba da resavamo njegove fLinkove, najbolje da oba budu neke funkcije koje cemo da pozivamo mozda i rekurzivno
				this->fLinkLink(fLink.addr);
			}
		}
	});
}

//void VarTable::updateEQU(){
//	std::for_each(this->varTable.begin(), this->varTable.end(), [](Field& f){
//		std::for_each(f.relocationTable.getRelocationTable().begin(), f.relocationTable.getRelocationTable().end(), [&f](RelocationTable::Field& f2){
//			if(f2.variable == 1){
//				f2.id
//			}
//		});
//	});
//}

std::ostream& operator<<(std::ostream &os, const VarTable &vt) {
	os<<"#Tabela simbola\n"
		"#ime\t\t sekcija\t vr\t\t vidljivost\t r.b.\t\t valid\t\t\n";
	for (const auto &ref : vt.varTable) {
		os << ref.name << "\t\t" << std::hex << ref.section << "\t\t" << std::hex << ref.value << "\t\t" <<(ref.visibility ? "Local" : "Global") << "\t\t" << std::hex<< ref.id << "\t\t" << ref.valid <<"\t\t\n";
	}

	for (const auto &ref : vt.varTable) {
		if(ref.section == ref.id && ref.id != 0){
			os << "\n" << ref.name << "\n";
//			os << ref.relocationTable;
			if(!ref.relocationTable.isEmpty()){
				ref.getRelocationTable();
				os << "#offset\t\t" << "tip\t\t" << "vrednost\t\t" << "variable\t\t\n";
				for(const auto& elem : ref.getRelocationTable().getRelocationTable()){
					os  << std::hex << elem.offset << "\t\t"<< (elem.addrType == 0 ? "R_386_32" : "R_386_PC32") << "\t\t";
					unsigned int myId;
					if(elem.sectionOrId){
						myId = vt.varTable[elem.varTableEntry].section;
					}
					else{
						myId = elem.varTableEntry;
					}
					os << std::hex << myId;
//					if(!elem.sectionOrId){
//						os << elem.varTableEntry <<"s";
//					}
//					else{
//						os << 99;
//					}
					os <<"\t\t"<< std::hex << elem.variable <<"\n";
				}
			}
		}
	}

	for (const auto &ref : vt.varTable) {
		if(!ref.binary.isEmpty()){
			os<< "\n\n#" << ref.name << "\n";
			os << ref.binary;
		}
	}

	return os;
}

