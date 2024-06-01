#ifndef C_H
#define C_H

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

/* NOT INSTRUCTIONS */
	TOKEN_IF,
	TOKEN_ELSE,
	TOKEN_DONE,

	TOKENTYPE_COUNT,
};

enum Instructions {
	LAST_INSTRUCTION = STRING,

	JUMP,
	JUMPIFNOT,
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
 *     - jump: the address to jump 
 *     - jumpifnot: the address to jump to
 *
 * Number values are little endian
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
void VM_run(Bytecode *bytes);

#endif /* C_H */
