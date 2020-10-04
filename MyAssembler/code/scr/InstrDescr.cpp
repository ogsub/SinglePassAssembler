/*
 * InstrDescr.cpp
 *
 *  Created on: Apr 20, 2020
 *      Author: student
 */

#include "InstrDescr.h"

std::map<std::string, InstrDescr::Desctriptor> InstrDescr::instrMap = {
			{"halt"	,	{0, OperandsNum::NONE}},
			{"iret"	,	{1, OperandsNum::NONE}},
			{"ret"	,	{2, OperandsNum::NONE}},
			{"int"	,	{3, OperandsNum::DST}},
			{"call"	,	{4, OperandsNum::DST}},
			{"jmp"	,	{5, OperandsNum::DST}},
			{"jeq"	,	{6, OperandsNum::DST}},
			{"jne"	,	{7, OperandsNum::DST}},
			{"jgt"	,	{8, OperandsNum::DST}},
			{"push"	,	{9, OperandsNum::SRC}},
			{"pop"	,	{10, OperandsNum::DST}},
			{"xchg"	,	{11, OperandsNum::TWO}},
			{"mov"	,	{12, OperandsNum::TWO}},
			{"add"	,	{13, OperandsNum::TWO}},
			{"sub"	,	{14, OperandsNum::TWO}},
			{"mul"	,	{15, OperandsNum::TWO}},
			{"div"	,	{16, OperandsNum::TWO}},
			{"cmp"	,	{17, OperandsNum::TWO}},
			{"not"	,	{18, OperandsNum::TWO}},
			{"and"	,	{19, OperandsNum::TWO}},
			{"or"	,	{20, OperandsNum::TWO}},
			{"xor"	,	{21, OperandsNum::TWO}},
			{"test"	,	{22, OperandsNum::TWO}},
			{"shl"	,	{23, OperandsNum::TWO}},
			{"shr"	,	{24, OperandsNum::TWO}}
	};

InstrDescr::InstrDescr() {
	// TODO Auto-generated constructor stub

}

InstrDescr::~InstrDescr() {
	// TODO Auto-generated destructor stub
}

std::uint8_t InstrDescr::getOpCode(const std::string& key){
	return instrMap.find(key)->second.operationCode;
}

int InstrDescr::getOperandNum(const std::string& key){
	OperandsNum op = instrMap.find(key)->second.operandNum;
	switch(op){
	case OperandsNum::NONE:
		return 0;
	case OperandsNum::SRC:
		return 1;
	case OperandsNum::DST:
		return 1;
	case OperandsNum::TWO:
		return 2;
	default:
		return 0;
	}
}

InstrDescr::OperandsNum InstrDescr::getOperandNumKind(const std::string& key){
	return instrMap.find(key)->second.operandNum;
}






