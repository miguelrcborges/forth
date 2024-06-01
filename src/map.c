#include "c.h"

typedef struct {
	string str;
	u8 tok_type;
} TokenData;

static usize hashtable_size = 32;

static TokenData add = {
	.str = str("+"),
	.tok_type = ADD
};
static TokenData sub = {
	.str = str("-"),
	.tok_type = SUB
};
static TokenData mul = {
	.str = str("*"),
	.tok_type = MUL
};
static TokenData div = {
	.str = str("/"),
	.tok_type = DIV
};
static TokenData swap = {
	.str = str("swap"),
	.tok_type = SWAP
};
static TokenData rot = {
	.str = str("rot"),
	.tok_type = ROT
};
static TokenData pop = {
	.str = str("pop"),
	.tok_type = POP
};
static TokenData dup = {
	.str = str("dup"),
	.tok_type = DUP
};
static TokenData drop = {
	.str = str("drop"),
	.tok_type = DROP
};
static TokenData nip = {
	.str = str("nip"),
	.tok_type = NIP
};
static TokenData token_if = {
	.str = str("if"),
	.tok_type = TOKEN_IF
};
static TokenData token_else = {
	.str = str("else"),
	.tok_type = TOKEN_ELSE
};
static TokenData token_done = {
	.str = str("done"),
	.tok_type = TOKEN_DONE
};

static TokenData *hashtable[] = {
	&nip,
	NULL,
	&rot,
	NULL,
	&drop,
	NULL,
	NULL,
	NULL,
	&swap,
	NULL,
	&dup,
	NULL,
	NULL,
	NULL,
	&token_else,
	NULL,
	NULL,
	NULL,
	&token_if,
	NULL,
	NULL,
	&mul,
	&add,
	&token_done,
	&sub,
	NULL,
	&div,
	NULL,
	&pop,
	NULL,
	NULL,
	NULL
};

static u16 hash(string s) {
	u16 h = 21;
	for (usize i = 0; i < s.len; ++i) {
		h = h * 31 + s.str[i];
	}
	return h;
}

u8 Map_getTokenType(string tok) {
	u16 h = hash(tok);
	h = h & (hashtable_size - 1);
	if (hashtable[h] && string_equal(tok, hashtable[h]->str)) {
		return hashtable[h]->tok_type;
	}
	io_write(getStdErr(), str("Invalid token found. Exiting.\n"));
	die(2);
	__builtin_unreachable();
}
