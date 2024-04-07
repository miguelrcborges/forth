#include "lib.h"

enum TokenType {
	EOF,
	INVALID,
	NUMBER,
	STRING,
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
	TOKENTYPE_COUNT
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


/* main.c */
extern Arena a;

/* lex.c */
Lex Lex_create(string src);
Token Lex_next(Lex *l);

/* map.c */
u8 Map_getTokenType(string token);

/* vm.c */
void VM_execute(Token t);
