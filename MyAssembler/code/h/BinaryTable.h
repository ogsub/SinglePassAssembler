#ifndef BINARYTABLE_H_
#define BINARYTABLE_H_

#include <vector>
#include <string>

class BinaryTable {
public:
	BinaryTable();
	virtual ~BinaryTable();

	enum Addressing{NEPOSREDNO, REG_DIREKTNO, REG_INDIREKTNO, REG_INDIREKTNO_POM, MEM};

	void pushInstruction(const std::string& instrName);
	void pushInstructionCode(int instrCode);
	void setOneBOperands();
	void setTwoBOperands();	//set two Bytes operands
	void setOneBOperands(std::vector<char>::iterator it);
	void setTwoBOperands(std::vector<char>::iterator it);
	void setAddressType(Addressing addrType);
	void setAddressReg(char regNo);
	void setHigh();
	void setLow();
	void pushOperandByte(short byte); //ubacuje nizi byte po defaultu
	void pushOperandBytes(short bytes); //ubacuje oba byte-a, prvo nizi pa posle visi, po little-endian 511 = ff 01
	void setOperandByte(short byte, int addr, bool pcRel);
	void setOperandBytes(short bytes, int addr, bool pcRel);
	int getSize();
	void erase(std::vector<char>::iterator it);
	std::vector<char>::iterator getBack();	//return iterator to a last element

	bool isEmpty() const;

	friend std::ostream& operator<<(std::ostream& os, const BinaryTable &bt);
private:
	std::vector<char> binaryTable;
};

#endif /* BINARYTABLE_H_ */
