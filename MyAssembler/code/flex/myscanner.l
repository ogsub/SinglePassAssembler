%{
#include "myscanner.h"
%}
%option nounput yylineno

%%
".end"						return END;
".global"					return GLOBAL;
".extern"					return EXTERN;
".section"					return SECTION;
"\n"						return BREAK_LNE;
","							return SEPARATOR;

\.[a-zA-Z][a-zA-Z0-9_]+		return DIRECTIVE;
[a-zA-Z]([a-zA-Z0-9]?)+:	return LABEL;
(halt|iret|ret|int|call|jmp|jeq|jne|jgt|push|pop|xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)(b?|w?)				return INSTRUCTION;
[ \t]						;
(\+|-)?(0x)?[0-9]+			return LITERAL;
(%r[0-7](h|l)?)|(%pc)		return REG;
[a-zA-Z]([a-zA-Z0-9]?)+		return SYMBOL;
(\*|$|\(|\))				return SPEC_CHAR;
\+|-						return SIGN;
.							printf("unexpected character\n");

%%

int yywrap() {
	return 1;
}