#include "Assembly.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <algorithm>

Assembly::Assembly() {
	yyin = fopen(
			"/home/student/Documents/SS/Projekat SS/prviZadatak/source/input.s",
			"r");
	if (yyin == nullptr) {
		std::cout << "Greska22! Fajl nije otvoren\n";
		exit(-1);
	}
	tokensTable.addTokens();
}

Assembly::Assembly(std::string fileName) {
	std::string absolutePath = "./";
	absolutePath.append(fileName);
	yyin = fopen(
			absolutePath.data(),
			"r");
	if (yyin == nullptr) {
		std::cout << "Greska22! Fajl nije otvoren\n";
		exit(-1);
	}
	tokensTable.addTokens();
}

Assembly::~Assembly() {
	fclose(yyin);
}

void Assembly::translateInstruction(std::vector<std::pair<Token, std::string>>::iterator &currElem,	//currElem je mozda i bespotreban, jer je on samo prvi element iz currLine, mozda u nekom slucaju nece biti prvi, pa ga ipak ostavljam
const std::vector<std::pair<Token, std::string>> &currLine) {

	std::string instruction = currElem->second;
	int size = 1;
	bool definedSize = false;
	int oneByteSize = 0;	//povecacu ovo svaki put kada bi ispunilo uslov za size = 1B
	//posle proveravam jel je oneByteSize == 2 za sve instrukcije sem push i pop, za njih proveravam
	//jel oneByteSize == 1
	int numImmed = 0;	//ove dve promenljive mi trebaju da bi znao kolko zauzima bajtova ova instrukcija
	int numReg = 0;
	int numRegOffset = 0;
	bool op1PCrel = false;	//koristimo za slucaj da treba vrednost op1 da smanjimo za jos 2B u slucaju da 2. operand koristi oba B, u slucaju da je op1 pc_rel

	if (instruction.back() == 'b') {
		definedSize = true;
		size = 0;
		instruction.pop_back();
	} else if (instruction.back() == 'w') {
		definedSize = true;
		size = 1;
		instruction.pop_back();
	}

	int operationCode = InstrDescr::getOpCode(instruction);
	int operandsNum = InstrDescr::getOperandNum(instruction);

	this->varTable.getBinary().pushInstructionCode(operationCode);
	if(!(definedSize && size == 0))
		this->varTable.getBinary().setTwoBOperands();	//po defaultu ce se prvo upisivati 2 b
	else
		this->varTable.getBinary().setOneBOperands();

	for (int i = 0; i < operandsNum; i++) {
		currElem++;
		if (currElem == currLine.end()) {
			std::cout << "Greska23, posle instrukcije moraju postojati promenljive\n";
			exit(-1);
		}

		//ovo > 2 nema potrebe, jer instrukcije sa kodovima 0, 1, 2 ni nemaju argumente
		if (operationCode > 2 && operationCode < 9) {//proveravamo sintaksu operanada koji u okviru asemblerskih naredbi predstavljaju skok
			switch (currElem->first){
			case Token::LITERAL:{
				this->varTable.getBinary().setAddressType(BinaryTable::NEPOSREDNO);

				std::stringstream ss(currElem->second);
				short literalValue;
				ss >> literalValue;
				if(literalValue <= 127 && literalValue >= -127 && !definedSize){
					size = 0;
					//update size, posto je default bilo 2 B
					auto it = this->varTable.getBinary().getBack();
					it--;
					this->varTable.getBinary().setOneBOperands(it);
				}
				if (size == 0) {
					//this->varTable.getBinary().setOneBOperands();
					this->varTable.getBinary().pushOperandByte(
							literalValue);
				} else {
					this->varTable.getBinary().pushOperandBytes(
							literalValue);
				}

				//OVAJ DEO NAM NI NE TREBA JER OVE INSTRUKCIJE PRIMAJU SAMO JEDAN PARAMETAR
				//ZAKOMENTARISI KASNIJE OVAJ DEO
				currElem++;
				if (currElem == currLine.end()) {
					if (i != operandsNum - 1) {
						std::cout << "Greska24, nedostaju parametri\n";
						exit(-1);
					}
					return;
				} else if (currElem->first != Token::SEPARATOR) {
					std::cout << "Greska25, ovde je morao ici zarez";
					exit(-1);
				}
				break;
			}
			case Token::SYMBOL:{
				this->varTable.getBinary().setAddressType(BinaryTable::NEPOSREDNO);

				auto it = this->varTable.exists(currElem->second);
				if (it != this->varTable.getVarTable().end()) {	//proverimo da li simbol vec postoji

					if (it->visibility == VarTable::Visibility::LOCAL) {

						if (it->valid == true) { //proverimo da li je u njega upisana vrednost (da li je validan ili nepoznat)
							this->varTable.getRelocationTable().addEntry(
									this->varTable.getBinary().getSize(),
									RelocationTable::AddrType::R_386_32,
									it->id, true);//it->section);

							if(it->value <= 127 && it->value >= -127){
								size = 0;
							}
							if (size == 0) {
								this->varTable.getBinary().pushOperandByte(
										it->value);
							} else {
								this->varTable.getBinary().pushOperandBytes(
										it->value);
							}
						} else if (it->valid == false) {
							//upisujemo u FLink i u relTable
							this->varTable.getRelocationTable().addEntry(
									this->varTable.getBinary().getSize(),
									RelocationTable::AddrType::R_386_32,
									it->id, true);//it->section);

							it->fLink.emplace_back(
									this->varTable.getBinary().getSize(),true, size);
							it->fLink.back().setSection(this->varTable.getCurrSection());

							if (size == 0) {
								this->varTable.getBinary().pushOperandByte(0);
							}
							else {
								this->varTable.getBinary().pushOperandBytes(0);
							}
						}

					} else if (it->visibility
							== VarTable::Visibility::GLOBAL) { //slucaj kad je u pitanju globalni simbol
						//upisujemo sve nule, i upisujemo relTable u odnosu na pormenljivu
						this->varTable.getRelocationTable().addEntry(
								this->varTable.getBinary().getSize(),
								RelocationTable::AddrType::R_386_32,
								it->id, false);//it->id);

						if (size == 0) {
							this->varTable.getBinary().pushOperandByte(0);
						}
						else {
							this->varTable.getBinary().pushOperandBytes(0);
						}

					}

				}
				else {//ukoliko ne postoji variabla, prvi put se pojavljuje
						//upisujemo je u tabelu kao lokalnu, i upisujemo u FLink i u relTable i upisujemo nule
					this->varTable.add(currElem->second);

					it = this->varTable.exists(currElem->second);

					this->varTable.getRelocationTable().addEntry(
							this->varTable.getBinary().getSize(),
							RelocationTable::AddrType::R_386_32,
							it->id, true);//it->section);

					it->fLink.emplace_back(
								this->varTable.getBinary().getSize(), true,	size);
					it->fLink.back().setSection(this->varTable.getCurrSection());

					if (size == 0) {
						this->varTable.getBinary().pushOperandByte(0);
					}
					else {
						this->varTable.getBinary().pushOperandBytes(0);
					}
				}

				currElem++;
				if (currElem == currLine.end()) {
					if (i != operandsNum - 1) {
						std::cout << "Greska26, nedostaju parametri\n";
						exit(-1);
					}
					return;
				} else if (currElem->first != Token::SEPARATOR) {
					std::cout << "Greska27, ovde je morao ici zarez";
					exit(-1);
				}

				break;
			}
			case Token::SPEC_CHAR:{
				if(currElem->second == "*"){
					currElem++;
					switch(currElem->first){
					case Token::SPEC_CHAR:{
						if (currElem->second == "(") {
							currElem++;

							if (currElem == currLine.end() || currElem->first != Token::REG) {
								std::cout << "Greska28, nedostaju parametri\n";
								exit(-1);
							}
							else {
								this->varTable.getBinary().setAddressType(
										BinaryTable::REG_INDIREKTNO);
								char regNo = currElem->second[2] - '0';
								this->varTable.getBinary().setAddressReg(regNo);

								if(!definedSize){
									if(currElem->second.back() == 'h' || currElem->second.back() == 'l'){
										size = 0;
										//update size, posto je default bilo 2 B
										auto it = this->varTable.getBinary().getBack();
										it--;
										this->varTable.getBinary().setOneBOperands(it);
									}
								}
								if (size == 0 && currElem->second.back() == 'h') {
									this->varTable.getBinary().setHigh();
								}
								currElem++;
								if (currElem->second != ")") {
									std::cout << "Greska29 Nije zatvorena zagrada\n";
									exit(-1);
								}
							}
						}
						else{
							std::cout<<"Greska30 U pitanju nije odgovarajuci specijalni karakter\n";
							exit(-1);
						}
						break;
					}
					case Token::LITERAL:{

						/////////////////////////////////////////////////////////////////

						std::stringstream ss(currElem->second);
						short literalValue;
						ss >> literalValue;

						currElem++;
						if(currElem != currLine.end() && currElem->first != Token::SEPARATOR) {	//slucaj kada posle ovoga ima jos (registar u zagradama, indirektno sa pomerajem)
							this->varTable.getBinary().setAddressType(BinaryTable::REG_INDIREKTNO_POM);

							if(currElem->second != "("){
								std::cout<<"Greska31, ovde treba da bude otvorena zagrada\n";
								exit(-1);
							}

							currElem++;

							if (currElem == currLine.end() || currElem->first != Token::REG) {
								std::cout << "Greska32, nedostaju parametri\n";
								exit(-1);
							} else {
								char regNo;
								if(currElem->second != "%pc"){
									regNo = currElem->second[2] - '0';
								}
								else{
									regNo = 7;
								}
								this->varTable.getBinary().setAddressReg(regNo);
								if(!definedSize){
									if(currElem->second.back() == 'h' || currElem->second.back() == 'l'){
										size = 0;
										//update size, posto je default bilo 2 B
										auto it = this->varTable.getBinary().getBack();
										it--;
										this->varTable.getBinary().setOneBOperands(it);
									}
								}
								if (size == 0 && currElem->second.back() == 'h') {
									this->varTable.getBinary().setHigh();
								}
								currElem++;
								if (currElem->second != ")") {
									std::cout << "Greska33 Nije zatvorena zagrada\n";
									exit(-1);
								}

							}
						}
						else if(currElem == currLine.end() || currElem->first == Token::SEPARATOR){
							this->varTable.getBinary().setAddressType(BinaryTable::MEM);
						}
						else {
							std::cout<<"Greska34!\n";
							exit(-1);
						}
						this->varTable.getBinary().pushOperandBytes(literalValue);

						if(currElem != currLine.end() && currElem->first != Token::SEPARATOR)
							currElem++;	//preskacemo zarez, ili stizemo do currLine.end()
						if (currElem == currLine.end()) {
							if (i != operandsNum - 1) {
								std::cout << "Greska35, nedostaju parametri\n";
								exit(-1);
							}
							return;
						} else if (currElem->first != Token::SEPARATOR) {
							std::cout << "Greska36, ovde je morao ici zarez";
							exit(-1);
						}




						////////////////////////////////////////////////////////////









						break;
					}
					case Token::SYMBOL:{

						/////////////////////////////////////////////////////////////////////


						short value = 0;

						std::string symbolName = currElem->second;
						auto it = this->varTable.exists(currElem->second);
						if (it != this->varTable.getVarTable().end()) {	//proverimo da li simbol vec postoji


							if (it->visibility == VarTable::Visibility::LOCAL) {

								if (it->valid == true) { //proverimo da li je u njega upisana vrednost (da li je validan ili nepoznat)
									this->varTable.getRelocationTable().addEntry(
											this->varTable.getBinary().getSize() + 1,
											RelocationTable::AddrType::R_386_32,
											it->id, true);//it->section);

									value = it->value;
									/*if (size == 0) {
										this->varTable.getBinary().pushOperandByte(
												it->value);
									} else {
										this->varTable.getBinary().pushOperandBytes(
												it->value);
									}*/
								} else if (it->valid == false) {
									//upisujemo u FLink i u relTable
									this->varTable.getRelocationTable().addEntry(
											this->varTable.getBinary().getSize() + 1,
											RelocationTable::AddrType::R_386_32,
											it->id, true);//it->section);

									it->fLink.emplace_back(
											this->varTable.getBinary().getSize() + 1,
											true, 1);
									it->fLink.back().setSection(this->varTable.getCurrSection());
									value = 0;
									/*if (size == 0) {
										this->varTable.getBinary().pushOperandByte(
												0);
									} else {
										this->varTable.getBinary().pushOperandBytes(
												0);
									}*/
								}

							} else if (it->visibility
									== VarTable::Visibility::GLOBAL) { //slucaj kad je u pitanju globalni simbol
								//upisujemo sve nule, i upisujemo relTable u odnosu na pormenljivu
								this->varTable.getRelocationTable().addEntry(
										this->varTable.getBinary().getSize() + 1,
										RelocationTable::AddrType::R_386_32,
										it->id, false);//it->id);

								value = 0;
								/*if (size == 0) {
									this->varTable.getBinary().pushOperandByte(
											0);
								} else {
									this->varTable.getBinary().pushOperandBytes(
											0);
								}*/

							}

						} else {//ukoliko ne postoji variabla, prvi put se pojavljuje
								//upisujemo je u tabelu kao lokalnu, i upisujemo u FLink i u relTable i upisujemo nule
							this->varTable.add(currElem->second);

							it = this->varTable.exists(currElem->second);

							this->varTable.getRelocationTable().addEntry(
									this->varTable.getBinary().getSize() + 1,
									RelocationTable::AddrType::R_386_32,
									it->id, true);//it->section);

							it->fLink.emplace_back(
									this->varTable.getBinary().getSize() + 1, true,
									1);
							it->fLink.back().setSection(this->varTable.getCurrSection());

							value = 0;
							/*
							if (size == 0) {
								this->varTable.getBinary().pushOperandByte(0);
							} else {
								this->varTable.getBinary().pushOperandBytes(0);
							}*/
						}

						currElem++;
						if(currElem != currLine.end() && currElem->first != Token::SEPARATOR) {
							this->varTable.getBinary().setAddressType(BinaryTable::REG_INDIREKTNO_POM);

							if(currElem->second != "("){
								std::cout<<"Greska37, ovde treba da bude otvorena zagrada\n";
								exit(-1);
							}

							currElem++;

							if (currElem == currLine.end() || currElem->first != Token::REG) {
								std::cout << "Greska38, nedostaju parametri\n";
								exit(-1);
							}
							else {
								char regNo;
								if(currElem->second != "%pc"){
									regNo = currElem->second[2] - '0';
								}
								else{
									value -= 2;	//ovo dodajem za pc vrednost operanda kod pc relativnog adresiranja u slucaju skokova
									//takodje treba da se promeni adresiranje u rel Tabeli iz apsolutnog u pc relativno
									this->varTable.getRelocationTable().getBack().setAddrType(RelocationTable::R_386_PC32);
									VarTable::Field& varTableEntry = this->varTable.getEntry(symbolName);
									if(varTableEntry.fLink.size() > 0)
										varTableEntry.fLink.back().pcRel = true;
									regNo = 7;
								}
								this->varTable.getBinary().setAddressReg(regNo);
								if(!definedSize){
									if(currElem->second.back() == 'h' || currElem->second.back() == 'l'){
										size = 0;
										//update size, posto je default bilo 2 B
										auto it = this->varTable.getBinary().getBack();
										it--;
										this->varTable.getBinary().setOneBOperands(it);
									}
								}
								if (size == 0 && currElem->second.back() == 'h') {
									this->varTable.getBinary().setHigh();
								}
								currElem++;
								if (currElem->second != ")") {
									std::cout << "Greska39 Nije zatvorena zagrada\n";
									exit(-1);
								}
							}
						}
						else if(currElem == currLine.end() || currElem->first == Token::SEPARATOR){
							this->varTable.getBinary().setAddressType(BinaryTable::MEM);
						}

						this->varTable.getBinary().pushOperandBytes(value);

						if(currElem != currLine.end())//proveravamo jer je mogao da bude MEM address type
							currElem++;	//preskacemo zatvorenu zagradu
						if (currElem == currLine.end()) {
							if (i != operandsNum - 1) {
								std::cout << "Greska40, nedostaju parametri\n";
								exit(-1);
							}
							return;
						} else if (currElem->first != Token::SEPARATOR) {
							std::cout << "Greska41, ovde je morao ici zarez";
							exit(-1);
						}






						/////////////////////////////////////////////////////////////////////
						break;
					}
					case Token::REG:{




						////////////////////////////////////
						this->varTable.getBinary().setAddressType(BinaryTable::REG_DIREKTNO);
						char regNo = currElem->second[2] - '0';
						this->varTable.getBinary().setAddressReg(regNo);
						if(!definedSize){
							if(currElem->second.back() == 'h' || currElem->second.back() == 'l'){
								size = 0;
								//update size, posto je default bilo 2 B
								auto it = this->varTable.getBinary().getBack();
								it--;
								this->varTable.getBinary().setOneBOperands(it);
							}
						}
						if (size == 0 && currElem->second.back() == 'h') {
							this->varTable.getBinary().setHigh();
						}

						//!OVO JE PRELAZAK NA ZAREZ VALJDA A NE PRESKAKANJE
						currElem++;	//preskacemo zarez ukoliko postoji, a ako ne postoji, to onda mora biti kraj linije
						if (currElem == currLine.end()) {
							if (i != operandsNum - 1) {
								std::cout << "Greska42, nedostaju parametri\n";
								exit(-1);
							}
							return;
						} else if (currElem->first != Token::SEPARATOR) {
							std::cout << "Greska43, ovde je morao ici zarez";
							exit(-1);
						}

						////////////////////////////////////






						break;
					}
					default:{
						std::cout<<"Greska44 u zapisu\n";
						exit(-1);
					}
					}
				}
				else{
					std::cout << "Greska45, ovde treba * da stoji\n";
					exit(-1);
				}
				break;
			}
			default:{
				std::cout << "Greska46, nije dobro napisano\n";
				exit(-1);
			}
			}
		}
		else {//proveravamo sintaksu operanada u okviru asemblerskih naredbi koje pristupaju podacima
			switch (currElem->first) {
			case Token::SPEC_CHAR: {

				if (currElem->second == "$") {
					if (i == 0) {
						if (instruction == "pop" || instruction == "shr") {
							std::cout
									<< "Greska47 Prvi operand ovih operacija ne moze da bude neposredna vrednost\n";
							exit(-1);
						}
					}
					else if (i == 1) {
						if (instruction != "pop" && instruction != "shr") {
							std::cout
									<< "Greska47 Prvi operand ovih operacija ne moze da bude neposredna vrednost\n";
							exit(-1);
						}
					}

					if(op1PCrel == true) {	//prethodna instrukcija je pc_rel
						this->varTable.getBinary().setOperandBytes(-2, this->varTable.getEntry(this->varTable.getCurrSection()).binary.getSize() - 2, true);
					}

					this->varTable.getBinary().setAddressType(BinaryTable::NEPOSREDNO);
					currElem++;

					if (currElem == currLine.end()) {
						std::cout << "Greska49, nedostaju parametri\n";
						exit(-1);

					} else if (currElem->first == Token::LITERAL) {
						std::stringstream ss(currElem->second);
						short literalValue;
						ss >> literalValue;
						if(literalValue <= 127 && literalValue >= -127 && !definedSize){
							oneByteSize++;
							numImmed++;
						}

						this->varTable.getBinary().pushOperandBytes(literalValue);


					} else if (currElem->first == Token::SYMBOL) {

						auto it = this->varTable.exists(currElem->second);
						if (it != this->varTable.getVarTable().end()) {	//proverimo da li simbol vec postoji

							if (it->visibility == VarTable::Visibility::LOCAL) {

								if (it->valid == true) { //proverimo da li je u njega upisana vrednost (da li je validan ili nepoznat)
									this->varTable.getRelocationTable().addEntry(
											this->varTable.getBinary().getSize(),
											RelocationTable::AddrType::R_386_32,
											it->id, true);//it->section);

									if (size == 0) {
										this->varTable.getBinary().pushOperandByte(
												it->value);
									} else {
										this->varTable.getBinary().pushOperandBytes(
												it->value);
									}
								} else if (it->valid == false) {
									//upisujemo u FLink i u relTable
									this->varTable.getRelocationTable().addEntry(
											this->varTable.getBinary().getSize(),
											RelocationTable::AddrType::R_386_32,
											it->id, true);//it->section);

									it->fLink.emplace_back(
											this->varTable.getBinary().getSize(),
											true, size);
									it->fLink.back().setSection(this->varTable.getCurrSection());

									if (size == 0) {
										this->varTable.getBinary().pushOperandByte(
												0);
									} else {
										this->varTable.getBinary().pushOperandBytes(
												0);
									}
								}

							} else if (it->visibility
									== VarTable::Visibility::GLOBAL) { //slucaj kad je u pitanju globalni simbol
								//upisujemo sve nule, i upisujemo relTable u odnosu na pormenljivu
								this->varTable.getRelocationTable().addEntry(
										this->varTable.getBinary().getSize(),
										RelocationTable::AddrType::R_386_32,
										it->id, false);//it->id);

								if (size == 0) {
									this->varTable.getBinary().pushOperandByte(
											0);
								} else {
									this->varTable.getBinary().pushOperandBytes(
											0);
								}

							}

						} else {//ukoliko ne postoji variabla, prvi put se pojavljuje
								//upisujemo je u tabelu kao lokalnu, i upisujemo u FLink i u relTable i upisujemo nule
							this->varTable.add(currElem->second);

							it = this->varTable.exists(currElem->second);

							this->varTable.getRelocationTable().addEntry(
									this->varTable.getBinary().getSize(),
									RelocationTable::AddrType::R_386_32,
									it->id, true);//it->section);

							it->fLink.emplace_back(
									this->varTable.getBinary().getSize(), true,
									size);
							it->fLink.back().setSection(this->varTable.getCurrSection());

							if (size == 0) {
								this->varTable.getBinary().pushOperandByte(0);
							} else {
								this->varTable.getBinary().pushOperandBytes(0);
							}
						}
					} else {
						std::cout << "Greska u zapisu\n";
					}

					/*currElem++;	//preskacemo zarez ukoliko postoji, a ako ne postoji, to onda mora biti kraj linije

					 if (currElem == currLine.end()) {
					 if (i != operandsNum - 1) {
					 std::cout << "Greska, nedostaju parametri\n";
					 exit(-1);
					 }
					 return;
					 } else if (currElem->first = !Token::SEPARATOR) {
					 std::cout << "Greska, ovde je morao ici zarez";
					 exit(-1);
					 }*/

				} else if (currElem->second == "(") {
					currElem++;

					if (currElem == currLine.end() || currElem->first != Token::REG) {
						std::cout << "Greska1, nedostaju parametri\n";
						exit(-1);
					} else {
						this->varTable.getBinary().setAddressType(
								BinaryTable::REG_INDIREKTNO);
						char regNo = currElem->second[2] - '0';
						this->varTable.getBinary().setAddressReg(regNo);
						if(!definedSize) {
							if(currElem->second.back() == 'h' || currElem->second.back() == 'l'){
								oneByteSize++;
								numReg++;
							}
						}
						if (currElem->second.back() == 'h') {//size == 0 && currElem->second.back() == 'h') {
							this->varTable.getBinary().setHigh();
						}
						currElem++;
						if (currElem->second != ")") {
							std::cout << "Greska2 Nije zatvorena zagrada\n";
							exit(-1);
						}
					}
				}
				//!OVO JE PRELAZAK NA ZAREZ VALJDA A NE PRESKAKANJE
				currElem++;	//preskacemo zarez ukoliko postoji, a ako ne postoji, to onda mora biti kraj linije
				if (currElem == currLine.end()) {
					if (i != operandsNum - 1) {
						std::cout << "Greska3, nedostaju parametri\n";
						exit(-1);
					}

					shortenOperands(oneByteSize, numImmed, numReg, numRegOffset, instruction);
					return;
				} else if (currElem->first != Token::SEPARATOR) {
					std::cout << "Greska4, ovde je morao ici zarez";
					exit(-1);
				}

				//!!!!!proveravamo da li je ispunjen uslov za smanjenje velicine operanada na 1B
				//shortenOperands(oneByteSize, numImmed, numReg, instruction);
				break;
			}
			case Token::REG: {
				this->varTable.getBinary().setAddressType(BinaryTable::REG_DIREKTNO);
				if(currElem->second == "%pc"){
					std::cout<<"Nije dozvoljeno koriscenje %pc registra na ovakav nacin\n";
					exit(-1);
				}
				char regNo = currElem->second[2] - '0';
				this->varTable.getBinary().setAddressReg(regNo);
				if(!definedSize) {
					if(currElem->second.back() == 'h' || currElem->second.back() == 'l'){
						oneByteSize++;
						numReg++;
//								size = 0;
//								//update size, posto je default bilo 2 B
//								auto it = this->varTable.getBinary().getBack();
//								it--;
//								this->varTable.getBinary().setOneBOperands(it);
					}
				}
				if (currElem->second.back() == 'h') {//(size == 0 && currElem->second.back() == 'h') {
					this->varTable.getBinary().setHigh();
				}
				//!OVO JE PRELAZAK NA ZAREZ VALJDA A NE PRESKAKANJE
				currElem++;	//preskacemo zarez ukoliko postoji, a ako ne postoji, to onda mora biti kraj linije
				if (currElem == currLine.end()) {
					if (i != operandsNum - 1) {
						std::cout << "Greska5, nedostaju parametri\n";
						exit(-1);
					}

					shortenOperands(oneByteSize, numImmed, numReg, numRegOffset, instruction);
					return;
				} else if (currElem->first != Token::SEPARATOR) {
					std::cout << "Greska6, ovde je morao ici zarez";
					exit(-1);
				}
				//shortenOperands(oneByteSize, numImmed, numReg, instruction);
				break;
			}
			case Token::SYMBOL: {

				unsigned short value = 0;

				if(op1PCrel == true) {	//prethodna instrukcija je pc_rel
					this->varTable.getBinary().setOperandBytes(-2, this->varTable.getEntry(this->varTable.getCurrSection()).binary.getSize() - 2, true);
				}

				std::string symbolName = currElem->second;
				auto it = this->varTable.exists(currElem->second);
				if (it != this->varTable.getVarTable().end()) {	//proverimo da li simbol vec postoji


					if (it->visibility == VarTable::Visibility::LOCAL) {

						if (it->valid == true) { //proverimo da li je u njega upisana vrednost (da li je validan ili nepoznat)
							this->varTable.getRelocationTable().addEntry(
									this->varTable.getBinary().getSize() + 1,
									RelocationTable::AddrType::R_386_32,
									it->id, true);//it->section);

							value = it->value;
							/*if (size == 0) {
								this->varTable.getBinary().pushOperandByte(
										it->value);
							} else {
								this->varTable.getBinary().pushOperandBytes(
										it->value);
							}*/
						} else if (it->valid == false) {
							//upisujemo u FLink i u relTable
							this->varTable.getRelocationTable().addEntry(
									this->varTable.getBinary().getSize() + 1,
									RelocationTable::AddrType::R_386_32,
									it->id, true);//it->section);

							it->fLink.emplace_back(
									this->varTable.getBinary().getSize() + 1,
									true, 1);
							it->fLink.back().setSection(this->varTable.getCurrSection());

							value = 0;
						}

					} else if (it->visibility
							== VarTable::Visibility::GLOBAL) { //slucaj kad je u pitanju globalni simbol
						//upisujemo sve nule, i upisujemo relTable u odnosu na pormenljivu
						this->varTable.getRelocationTable().addEntry(
								this->varTable.getBinary().getSize() + 1,
								RelocationTable::AddrType::R_386_32,
								it->id, false);//it->id);

						value = 0;
					}

				}
				else {//ukoliko ne postoji variabla, prvi put se pojavljuje
						//upisujemo je u tabelu kao lokalnu, i upisujemo u FLink i u relTable i upisujemo nule
					this->varTable.add(currElem->second);

					it = varTable.exists(currElem->second);

					this->varTable.getRelocationTable().addEntry(
							this->varTable.getBinary().getSize() + 1,
							RelocationTable::AddrType::R_386_32,
							it->id, true);//it->section);

					it->fLink.emplace_back(
							this->varTable.getBinary().getSize() + 1, true,
							1);
					it->fLink.back().setSection(this->varTable.getCurrSection());

					value = 0;
				}

				currElem++;
				if(currElem != currLine.end() && currElem->first != Token::SEPARATOR) {
					this->varTable.getBinary().setAddressType(BinaryTable::REG_INDIREKTNO_POM);

					if(currElem->second != "("){
						std::cout<<"Greska7, ovde treba da bude otvorena zagrada\n";
						exit(-1);
					}

					currElem++;

					if (currElem == currLine.end() || currElem->first != Token::REG) {
						std::cout << "Greska8, nedostaju parametri\n";
						exit(-1);
					}
					else {
						char regNo;
						if(currElem->second != "%pc"){
							regNo = currElem->second[2] - '0';
						}
						else{
							regNo = 7;


							////////////////////////////////////////////


							value -= 2;	//ovo dodajem za pc vrednost operanda kod pc relativnog adresiranja u slucaju skokova
							//takodje treba da se promeni adresiranje u rel Tabeli iz apsolutnog u pc relativno
							this->varTable.getRelocationTable().getBack().setAddrType(RelocationTable::R_386_PC32);
							VarTable::Field& varTableEntry = this->varTable.getEntry(symbolName);
							if(varTableEntry.fLink.size() > 0)
								varTableEntry.fLink.back().pcRel = true;
							regNo = 7;
							if(i != 1){
								value -= 1;	//ukoliko je u pitanju 1. operand, sigurno cemo morati da mu oduzmemo i ovaj bajt za nacin adresiranja
								op1PCrel = true;
							}

							///////////////////////////////////////////


						}
						this->varTable.getBinary().setAddressReg(regNo);
						if(!definedSize) {
							if(currElem->second.back() == 'h' || currElem->second.back() == 'l'){
								oneByteSize++;
								numRegOffset++;
							}
						}
						if (currElem->second.back() == 'h') {//(size == 0 && currElem->second.back() == 'h') {
							this->varTable.getBinary().setHigh();
						}
						currElem++;
						if (currElem->second != ")") {
							std::cout << "Greska9 Nije zatvorena zagrada\n";
							exit(-1);
						}
					}
				}
				else if(currElem == currLine.end() || currElem->first == Token::SEPARATOR){
					this->varTable.getBinary().setAddressType(BinaryTable::MEM);
					numRegOffset++;//zasto sam ovo dodao, jer linker moze da provali da su potrebna 2B za operand iz MEM tipa adresiranja, jer ono uvek koristi 2B, tako da dozvoljavam da drugi operand koristi 1B ako on ispunjava svoje uslove
					oneByteSize++;
				}

				this->varTable.getBinary().pushOperandBytes(value);

				//!OVDE NE PRESKACEMO ZAREZ NEGO SKACEMO NA NJEGA AKO JE TRENUTNO NA ")"
				if(currElem != currLine.end() && currElem->first != Token::SEPARATOR)
					currElem++;	//preskacemo zarez, ili stizemo do currLine.end()
				if (currElem == currLine.end()) {
					if (i != operandsNum - 1) {
						std::cout << "Greska10, nedostaju parametri\n";
						exit(-1);
					}

					shortenOperands(oneByteSize, numImmed, numReg, numRegOffset, instruction);
					return;
				} else if (currElem->first != Token::SEPARATOR) {
					std::cout << "Greska11, ovde je morao ici zarez";
					exit(-1);
				}

				break;
			}
			case Token::LITERAL: {
				std::stringstream ss(currElem->second);
				short literalValue;
				ss >> literalValue;

				if(op1PCrel == true) {	//prethodna instrukcija je pc_rel
					this->varTable.getBinary().setOperandBytes(-2, this->varTable.getEntry(this->varTable.getCurrSection()).binary.getSize() - 2, true);
				}

				currElem++;
				if(currElem != currLine.end() && currElem->first != Token::SEPARATOR) {	//slucaj kada posle ovoga ima jos (registar u zagradama, indirektno sa pomerajem)
					this->varTable.getBinary().setAddressType(BinaryTable::REG_INDIREKTNO_POM);

					if(currElem->second != "("){
						std::cout<<"Greska12, ovde treba da bude otvorena zagrada\n";
						exit(-1);
					}

					currElem++;

					if (currElem == currLine.end() || currElem->first != Token::REG) {
						std::cout << "Greska13, nedostaju parametri\n";
						exit(-1);
					} else {
						char regNo;
						if(currElem->second != "%pc"){
							regNo = currElem->second[2] - '0';
						}
						else{
							regNo = 7;
						}
						this->varTable.getBinary().setAddressReg(regNo);
						if(!definedSize) {
							if(currElem->second.back() == 'h' || currElem->second.back() == 'l'){
								oneByteSize++;
								numRegOffset++;
							}
						}
						if (currElem->second.back() == 'h') {//(size == 0 && currElem->second.back() == 'h') {
							this->varTable.getBinary().setHigh();
						}
						currElem++;
						if (currElem->second != ")") {
							std::cout << "Greska14, Nije zatvorena zagrada\n";
							exit(-1);
						}

					}
				}
				else if(currElem == currLine.end() || currElem->first == Token::SEPARATOR) {
					this->varTable.getBinary().setAddressType(BinaryTable::MEM);
					numRegOffset++;
					oneByteSize++;
				}
				this->varTable.getBinary().pushOperandBytes(literalValue);

				if(currElem != currLine.end() && currElem->first != Token::SEPARATOR)
					currElem++;	//preskacemo zarez, ili stizemo do currLine.end()
				if (currElem == currLine.end()) {
					if (i != operandsNum - 1) {
						std::cout << "Greska, nedostaju parametri\n";
						exit(-1);
					}

					shortenOperands(oneByteSize, numImmed, numReg, numRegOffset, instruction);
					return;
				} else if (currElem->first != Token::SEPARATOR) {
					std::cout << "Greska15, ovde je morao ici zarez";
					exit(-1);
				}
				break;
			}
			default:{
				std::cout << "Greska16, nije dobro napisano\n";
				exit(-1);
			}

			}
		}

	}
}

void Assembly::translate() {
	bool labelDefinitionMode = false;

	std::vector<std::pair<Token, std::string>> currLine = tokensTable.getLine();
	std::vector<std::pair<Token, std::string>>::iterator currElem =
					currLine.begin();
	while (currLine.size() != 0) {
		for (const auto &i : currLine) {
			std::cout << i.first;
			std::cout << "|";
			std::cout << i.second;
			std::cout << "||\n";
		}
		if(!labelDefinitionMode){
			currElem = currLine.begin();
		}
		else{
			labelDefinitionMode = false;
		}
		switch (currElem->first) {
		case Token::GLOBAL: {
			Token expected = Token::SYMBOL;
			currElem++;
			while (currElem != currLine.end()) {
				if (expected != currElem->first) {
					std::cout << "Greska17 u inputu\n";
					exit(-1);
				}
				if (expected == Token::SEPARATOR) {
					expected = Token::SYMBOL;
					currElem++;
					continue;
				}
				this->varTable.add(currElem->second);
				this->varTable.setGlobal(currElem->second);
				expected = Token::SEPARATOR;
				currElem++;
			}

			//proverava da se ne ocekuje mozda token SYMBOL, jer bi to znacilo da je zadnji token u redu SEPARATOR
			if (expected != Token::SEPARATOR) {
				std::cout
						<< "Greska18 u inputu, nije dozvoljen separator na kraju linije\n";
				exit(-1);
			}
			break;
		}
		case Token::EXTERN: {
			Token expected = Token::SYMBOL;
			currElem++;
			while (currElem != currLine.end()) {
				if (expected != currElem->first) {
					std::cout << "Greska19 u inputu\n";
					exit(-1);
				}
				if (expected == Token::SEPARATOR) {
					expected = Token::SYMBOL;
					currElem++;
					continue;
				}
				this->varTable.add(currElem->second);
				this->varTable.setExtern(currElem->second);
				expected = Token::SEPARATOR;
				currElem++;
			}
			//proverava da se ne ocekuje mozda token SYMBOL, jer bi to znacilo da je zadnji token u redu SEPARATOR
			if (expected != Token::SEPARATOR) {
				std::cout
						<< "Greska20 u inputu, nije dozvoljen separator na kraju linije\n";
				exit(-1);
			}
			break;
		}
		case Token::SECTION: {
			Token expected = Token::DIRECTIVE;
			currElem++;

			if (expected != currElem->first) {
				std::cout << "Greska21 u inputu\n";
				exit(-1);
			}
			this->varTable.addSection(currElem->second);

			break;
		}
		case Token::INSTRUCTION: {
			translateInstruction(currElem, currLine);
			break;
		}
		case Token::LABEL:{
			defineLabel(labelDefinitionMode, currElem, currLine);
			break;
		}
		case Token::DIRECTIVE:{
			if(this->varTable.getCurrSection() == 0){
				std::cout<<"Greska200\n";
				exit(-1);
			}

			translateDirective(currElem, currLine);
			break;
		}
		}	//end switch
		if(!labelDefinitionMode){
			currLine = tokensTable.getLine();
		}
	}
}

void Assembly::printVarTable() {
	std::cout << varTable;
}

void Assembly::writeToFile(const std::string& name) {
	std::string absoluteFilePath = "./";
	absoluteFilePath.append(name);
	std::ofstream myfile;
	myfile.open(absoluteFilePath);
	myfile << varTable;
	myfile.close();
}

void Assembly::defineLabel(bool& labelDefinitionMode, std::vector<std::pair<Token, std::string>>::iterator& currElem,
		const std::vector<std::pair<Token, std::string>> &currLine) {

	std::string labelName = currElem->second;
	if(labelName.back() == ':')
		labelName.pop_back();	//ovo radim zato sto mi lexer hvata labelu zajedno sa dve tacke (labela:)

	//auto it = this->varTable.exists(labelName);//currElem->second);

	//add(name) dodaje novi simbol ovog imena samo ako vec nije upisan u tabelu
	this->varTable.add(labelName);//currElem->second);

	auto it = this->varTable.exists(labelName);//currElem->second);

	if(it->valid == true || it->externVar == true){
		std::cout<<"Simbol je vec definisan ili je eksterni\n";
		exit(-1);
	}
	if(this->varTable.getCurrSection() == 0){
		std::cout<<"Greska100\n";
		exit(-1);
	}

	this->varTable.setSection(labelName, this->varTable.getCurrSection());//currElem->second, this->varTable.getCurrSection());
	this->varTable.setValue(labelName, this->varTable.getBinary().getSize());//currElem->second, this->varTable.getBinary().getSize());
	this->varTable.setValid(labelName);//currElem->second);

	//resavamo fLinkove
	this->varTable.fLinkLink(labelName);//currElem->second);

	currElem++;
	if(currElem != currLine.end()){
		labelDefinitionMode = true;
	}

}

void Assembly::shortenOperands(int oneByteSize, int numImmed, int numReg, int numRegOffset, const std::string &instruction){
	if(instruction == "push" || instruction == "pop"){
		if(oneByteSize == 1){
			if(numImmed == 1){
				auto it = this->varTable.getBinary().getBack();
				it -= 3;	//ovim sam dosao do OC
				this->varTable.getBinary().setOneBOperands(it);
				this->varTable.getBinary().erase(it);
			}
			else if(numReg == 1){
				auto it = this->varTable.getBinary().getBack();
				it--;	//ovim sam dosao do OC
				this->varTable.getBinary().setOneBOperands(it);
			}
		}
	}
	else if (oneByteSize == 2){
		if(numImmed == 2){
			auto it = this->varTable.getBinary().getBack();
			it -= 6;	//ovim sam dosao do OC
			this->varTable.getBinary().setOneBOperands(it);
			it += 3;
			this->varTable.getBinary().erase(it);
			it = this->varTable.getBinary().getBack();	//morao sam ponovo, jer posle brisanja, svi iteratori postaju nevalidni
			this->varTable.getBinary().erase(it);
		}
		else if(numReg == 2){
			auto it = this->varTable.getBinary().getBack();
			it -= 2;	//ovim sam dosao do OC
			this->varTable.getBinary().setOneBOperands(it);
		}
		else if(numRegOffset == 2){
			auto it = this->varTable.getBinary().getBack();
			it -= 6;	//ovim sam dosao do OC
			this->varTable.getBinary().setOneBOperands(it);
		}
		else if (numImmed + numReg == 2 || numImmed + numRegOffset == 2){
			auto it = this->varTable.getBinary().getBack();
			if(numReg)
				it -= 4;
			else if(numRegOffset)
				it -= 6;
			this->varTable.getBinary().setOneBOperands(it);
			it++;

			bool erased = false;
			while(!erased){
				if((*it & (15<<4)) == 0){	//proveravam kod, ovo je B za kod narednog operanda, i gledam koji je neposredni
					it += 2;
					this->varTable.getBinary().erase(it);
					erased = true;
				}
				else{
					it++;
				}
			}
		}
		else if(numReg + numRegOffset == 2){
			auto it = this->varTable.getBinary().getBack();
			it -= 4;
			this->varTable.getBinary().setOneBOperands(it);
		}
	}
}

void Assembly::checkSameSection(unsigned int offset, unsigned int section){
	this->varTable.checkSameSection(offset, section);
}
void Assembly::checkVarValidity(){
	this->varTable.checkVarValidity();
}

void Assembly::translateDirective(std::vector<std::pair<Token, std::string>>::iterator &currElem,	//currElem je mozda i bespotreban, jer je on samo prvi element iz currLine, mozda u nekom slucaju nece biti prvi, pa ga ipak ostavljam
		const std::vector<std::pair<Token, std::string>> &currLine){
	std::string directive = currElem->second;
	if(directive == ".byte" || directive == ".word") {
		currElem++;
		while(currElem != currLine.end()){
			if (currElem->first == Token::LITERAL) {
				std::stringstream ss(currElem->second);
				short literalValue;
				ss >> literalValue;
				if (directive == ".byte") {
					this->varTable.getBinary().pushOperandByte(
							literalValue);
				}
				else if(directive == ".word") {
					this->varTable.getBinary().pushOperandBytes(
							literalValue);
				}

			} else if (currElem->first == Token::SYMBOL) {

				auto it = this->varTable.exists(currElem->second);
				if (it != this->varTable.getVarTable().end()) {	//proverimo da li simbol vec postoji

					if (it->visibility == VarTable::Visibility::LOCAL) {

						if (it->valid == true) { //proverimo da li je u njega upisana vrednost (da li je validan ili nepoznat)
							this->varTable.getRelocationTable().addEntry(
									this->varTable.getBinary().getSize(),
									RelocationTable::AddrType::R_386_32,
									it->id, true);//it->section);

							if (directive == ".byte") {
								this->varTable.getBinary().pushOperandByte(
										it->value);
							}
							else if(directive == ".word") {
								this->varTable.getBinary().pushOperandBytes(
										it->value);
							}
						} else if (it->valid == false) {
							//upisujemo u FLink i u relTable
							this->varTable.getRelocationTable().addEntry(
									this->varTable.getBinary().getSize(),
									RelocationTable::AddrType::R_386_32,
									it->id, true);//it->section);

							it->fLink.emplace_back(
									this->varTable.getBinary().getSize(),
									true, directive == ".byte"? 0 : 1);
							it->fLink.back().setSection(this->varTable.getCurrSection());

							if (directive == ".byte") {
								this->varTable.getBinary().pushOperandByte(
										0);
							} else if(directive == ".word"){
								this->varTable.getBinary().pushOperandBytes(
										0);
							}
						}

					} else if (it->visibility
							== VarTable::Visibility::GLOBAL) { //slucaj kad je u pitanju globalni simbol
						//upisujemo sve nule, i upisujemo relTable u odnosu na pormenljivu
						this->varTable.getRelocationTable().addEntry(
								this->varTable.getBinary().getSize(),
								RelocationTable::AddrType::R_386_32,
								it->id, false);//it->id);

						if (directive == ".byte") {
							this->varTable.getBinary().pushOperandByte(
									0);
						} else if(directive == ".word"){
							this->varTable.getBinary().pushOperandBytes(
									0);
						}

					}

				} else {//ukoliko ne postoji variabla, prvi put se pojavljuje
						//upisujemo je u tabelu kao lokalnu, i upisujemo u FLink i u relTable i upisujemo nule
					this->varTable.add(currElem->second);

					it = this->varTable.exists(currElem->second);

					this->varTable.getRelocationTable().addEntry(
							this->varTable.getBinary().getSize(),
							RelocationTable::AddrType::R_386_32,
							it->id, true);//it->section);

					it->fLink.emplace_back(
							this->varTable.getBinary().getSize(), true,
							directive == ".byte"? 0 : 1);
					it->fLink.back().setSection(this->varTable.getCurrSection());

					if (directive == ".byte") {
						this->varTable.getBinary().pushOperandByte(0);
					}
					else if(directive == ".word"){
						this->varTable.getBinary().pushOperandBytes(0);
					}
				}
			} else {
				std::cout << "Greska u zapisu\n";
			}
			currElem++;
			if(currElem->first != Token::SEPARATOR && currElem != currLine.end()){
				std::cout << "Greska202 u zapisu\n";
				exit(-1);
			}
			else if(currElem != currLine.end()) {
				currElem++;
			}
		}

	}
	else if(directive == ".skip"){
		currElem++;
		std::stringstream ss(currElem->second);
		short num;
		ss >> num;
		for(unsigned short i = 0; i < num; i++) {
			this->varTable.getBinary().pushOperandByte(0);
		}
	}
	else if(directive == ".equ"){
		currElem++;

		std::string variableName;
		if(currElem->first == Token::SYMBOL){
			variableName = currElem->second;
			if(this->varTable.add(variableName) == 0) {	//znaci da ranije nije postojala, sad smo je ubacili
				VarTable::Field& field = this->varTable.getEntry(variableName);
				field.section = this->varTable.getCurrSection();
			}
			else {
				auto it = this->varTable.exists(variableName);
				if(it->valid){
					std::cout<<"Variabla vec postoji, dupla definicija\n";
					exit(-1);
				}
				if(it->section == 0){
					it->section = this->varTable.getCurrSection();
				}
			}
		}
		else {
			std::cout<<"Greska 2003\n";
			exit(-1);
		}
		currElem++;
		if(currElem->first != Token::SEPARATOR){
			std::cout<<"Greska 2004\n";
			exit(-1);
		}
		currElem++;

		short value = 0;

		bool sign = true; //true => +, false => -
		while(currElem != currLine.end()) {
			switch(currElem->first){
			case Token::SIGN:{
				sign = (currElem->second == "+" ? true : false);
				currElem++;
				break;
			}
			case Token::LITERAL:{
				std::stringstream ss(currElem->second);
				short literalValue;
				ss >> literalValue;
				value = value + (sign? literalValue : -literalValue);
				currElem++;
				break;
			}
			case Token::SYMBOL:{

				//////////////////////////////////////////////////////////////////
				auto it = this->varTable.exists(currElem->second);
				if (it != this->varTable.getVarTable().end()) {	//proverimo da li simbol vec postoji


					if (it->visibility == VarTable::Visibility::LOCAL) {

						if (it->valid == true) { //proverimo da li je u njega upisana vrednost (da li je validan ili nepoznat)
							checkSameSection(this->varTable.getId(variableName), it->section);	//prima id variable koja se definise pomocu .equ i sekciju simbola koji se pominje pri definisanju

							this->varTable.getRelocationTable().addEntry(
									this->varTable.getId(variableName),//this->varTable.getBinary().getSize() + 1,
									RelocationTable::AddrType::R_386_32,
									it->section, true, true);//it->section, true);	//it->section je referenca


							value = value + (sign? it->value : -it->value);
						}
						else if (it->valid == false) {
							//upisujemo u FLink i u relTable
							this->varTable.getRelocationTable().addEntry(
									this->varTable.getId(variableName),//this->varTable.getBinary().getSize() + 1,
									RelocationTable::AddrType::R_386_32,
									it->id, true, true);//it->section, true);

							it->fLink.emplace_back(
									this->varTable.getId(variableName),//this->varTable.getBinary().getSize() + 1,
									sign, 1, true);
							it->fLink.back().setSection(this->varTable.getCurrSection());
							this->varTable.addLinks(variableName);
						}

					} else if (it->visibility
							== VarTable::Visibility::GLOBAL) { //slucaj kad je u pitanju globalni simbol
						//upisujemo sve nule, i upisujemo relTable u odnosu na pormenljivu
//						this->varTable.getRelocationTable().addEntry(
//								this->varTable.getId(variableName),//this->varTable.getBinary().getSize() + 1,
//								RelocationTable::AddrType::R_386_32,
//								it->id, true);
						std::cout<<"Greska, nisu dozvoljeni globalni";
						exit(-1);

					}

				} else {//ukoliko ne postoji variabla, prvi put se pojavljuje
						//upisujemo je u tabelu kao lokalnu, i upisujemo u FLink i u relTable i upisujemo nule
					this->varTable.add(currElem->second);

					it = this->varTable.exists(currElem->second);

					this->varTable.getRelocationTable().addEntry(
							this->varTable.getId(variableName),//this->varTable.getBinary().getSize() + 1,	//uzima se id od x iz primera .equ x, y; i upisuje se u rel.Table y simbola
							RelocationTable::AddrType::R_386_32,
							it->id, true, true);//it->section, true);

					it->fLink.emplace_back(
							this->varTable.getId(variableName),//this->varTable.getBinary().getSize() + 1,
							sign,1, true);
					it->fLink.back().setSection(this->varTable.getCurrSection());
					this->varTable.addLinks(variableName);
				}
				//////////////////////////////////////////////////////////////////

				currElem++;
				break;
			}
			}
		}

		this->varTable.setValue(variableName, value);
		if(this->varTable.isValid(this->varTable.getId(variableName))){
			this->varTable.fLinkLink(variableName);
		}
	}
}

//void Assembly::updateEQURelTable(){
//	this->varTable.getRelocationTable().updateEQU(this->varTable);
//}





