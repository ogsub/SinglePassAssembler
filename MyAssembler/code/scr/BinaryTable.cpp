#include "BinaryTable.h"
#include "InstrDescr.h"
#include <iostream>

BinaryTable::BinaryTable() {
	// TODO Auto-generated constructor stub

}

BinaryTable::~BinaryTable() {
	// TODO Auto-generated destructor stub
}

//ove operacije moraju se pozivati redosledom ubacivanja bajtova u vektor
void BinaryTable::pushInstruction(const std::string &instrName) {
	char byte = InstrDescr::getOpCode(instrName) << 3;
	this->binaryTable.push_back(byte);
}

void BinaryTable::pushInstructionCode(int instrCode) {
	char byte = instrCode << 3;
	this->binaryTable.push_back(byte);
}

void BinaryTable::setOneBOperands() {
	this->binaryTable.back() &= ~4;
}

void BinaryTable::setTwoBOperands() {
	this->binaryTable.back() |= 4;
}

void BinaryTable::setOneBOperands(std::vector<char>::iterator it){
	*it &= ~4;
}
void BinaryTable::setTwoBOperands(std::vector<char>::iterator it){
	*it |= 4;
}

void BinaryTable::setAddressType(Addressing addrType) {
	char byte = addrType << 5;
	this->binaryTable.push_back(byte);
}

void BinaryTable::setAddressReg(char regNo){
	this->binaryTable.back() |= regNo << 1;
}

void BinaryTable::setHigh(){
	this->binaryTable.back() |= 1;
}

void BinaryTable::setLow(){
	this->binaryTable.back() &= ~1;
}

void BinaryTable::pushOperandByte(short byte){
	byte &= 0xFF;
	this->binaryTable.push_back(byte);
}

void BinaryTable::pushOperandBytes(short bytes){
	char byte = bytes & 0xff;
	this->binaryTable.push_back(byte);
	byte = (bytes >> 8) & 0xff;
	this->binaryTable.push_back(byte);
}

int BinaryTable::getSize(){
	return this->binaryTable.size();
}

void BinaryTable::erase(std::vector<char>::iterator it){
	this->binaryTable.erase(it);
}

std::vector<char>::iterator BinaryTable::getBack() {
	return --this->binaryTable.end();
}

void BinaryTable::setOperandByte(short byte, int addr, bool pcRel){
	byte &= 0xFF;
	if(!pcRel)
		this->binaryTable[addr] = byte;
	else
		this->binaryTable[addr] += byte;
}
void BinaryTable::setOperandBytes(short bytes, int addr, bool pcRel){
	if(!pcRel){
		char byte = bytes & 0xff;
		this->binaryTable[addr] = byte;
		byte = (bytes >> 8) & 0xff;
		this->binaryTable[addr + 1] = byte;
	}
	else {
		short currVal = 0xFFFF;
		currVal = currVal & this->binaryTable[addr];
		currVal = currVal & ((this->binaryTable[addr + 1] << 8) + 0xFF);
		currVal += bytes;

		char byte = currVal & 0xff;
		this->binaryTable[addr] = byte;
		byte = (currVal >> 8) & 0xff;//(bytes >> 8) & 0xff;
		this->binaryTable[addr + 1] = byte;
	}
}

bool BinaryTable::isEmpty() const{
	return this->binaryTable.empty();
}

std::ostream& operator<<(std::ostream& os, const BinaryTable &bt){
	if(!bt.isEmpty()){
		for(const auto& elem : bt.binaryTable){
			os  << std::hex << (int)(unsigned char)elem << " ";
		}
	}
	return os;
}



