#ifndef INSTRDESCR_H_
#define INSTRDESCR_H_

#include <map>
#include <string>
#include <cstdint>

class InstrDescr {
public:
	enum class OperandsNum{NONE, SRC, DST, TWO};

	InstrDescr();
	virtual ~InstrDescr();

	static std::uint8_t getOpCode(const std::string& key);
	static int getOperandNum(const std::string& key);
	static OperandsNum getOperandNumKind(const std::string& key);
private:
	struct Desctriptor{
		std::uint8_t operationCode;
		OperandsNum operandNum;
	};
	static std::map<std::string, Desctriptor> instrMap;
};

#endif /* INSTRDESCR_H_ */
