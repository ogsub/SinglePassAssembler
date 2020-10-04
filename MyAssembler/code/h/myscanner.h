#ifndef MYSCANNER_H_
#define MYSCANNER_H_

extern const char *names[];


enum Token {
	END,
	GLOBAL,
	EXTERN,
	SECTION,
	BREAK_LNE,
	SEPARATOR,
	DIRECTIVE,
	LABEL,
	INSTRUCTION,
	LITERAL,
	REG,
	SYMBOL,
	SPEC_CHAR,
	SIGN
};
#endif
