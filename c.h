#include "lib.h"

enum TokenType {
/* Translation => can be used directly as bytecode instruction */
/* TRANSLATION_FREE_SECTION */
	EOF,
	ADD,
	SUB,
	MUL,
	DIV,
	SWAP,
	POP,
	DUP,
	DROP,
	NIP,
	ROT,

/* REQUIRES_TRANSLATION */
	NUMBER,
	STRING,

	TOKENTYPE_COUNT,
};

typedef struct {
	u8 type;
	union {
		string s;
		u32 number;
	};
} Token;

typedef struct {
	string src;
	u32 pos;
} Lex;


/** 
 * Instruction is 8 bits
 * Bottom 6 bits define the instruction
 * Top 2 bits define the number of bytes of the number to parse (if instruction needs a number)
 *     n_bytes = ((instr & 0xc0) >> 6) + 1
 * 
 * The instructions that parse a number are:
 *     - string: the following number is the length of the string
 *     - number: the number to parse
 *
 * For the sake of simplicity, the instruction code will be the same as the token value.
 * (for now, stuff like ifs and elses will need to be different)
 */
typedef struct {
	u8 instr;
} Bytecode;


/* lex.c */
Lex Lex_create(string src);
Token Lex_next(Lex *l);

/* comp.c */
Bytecode *Compiler_compile(Lex *l);
void Compiler_free(void);

/* map.c */
u8 Map_getTokenType(string token);

/* vm.c */
/**
 * @return Pointer to last bytecode instruction.
 */
void VM_run(Bytecode *bytes);
