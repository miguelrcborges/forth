#include "c.h"

static u8 ignore[(1<<8)-1] = {
	#define ign(c) [c] = 1
	ign(' '), ign('\t'), ign('\n'), ign('\r'), ign('\v'), ign('\f')
	#undef ign
};

Lex Lex_create(string src) {
	return (Lex) {
		.src = src,
		.pos = 0,
	};
}

Token Lex_next(Lex *l) {
	Token t;
	if (l->pos >= l->src.len) {
		t.type = EOF;
		return t;
	}

	while (ignore[l->src.str[l->pos]]) {
		l->pos += 1;
	}

	if (l->src.str[l->pos] >= '0' && l->src.str[l->pos] <= '9') {
		t.type = NUMBERS;
		t.number = 0;
		do {
			t.number *= 10;
			t.number += l->src.str[l->pos] - '0';
			l->pos += 1;
		} while (l->pos < l->src.len && l->src.str[l->pos] >= '0' && l->src.str[l->pos] <= '9');
		return t;
	}

	if (l->src.str[l->pos] == '\"') {
		t.type = STRING;
		l->pos += 1;
		u32 s = l->pos;
		while (l->pos < l->src.len && l->src.str[l->pos] == '\"') {
			l->pos += 1;
		}
		t.s.str = l->src.str + s;
		t.s.len = l->pos - s;
		return t;
	}

	string instr;
	u32 s = l->pos;
	l->pos += 1;
	while (l->pos < l->src.len && ignore[l->src.str[l->pos]]) {
		l->pos += 1;
	}
	instr.str = l->src.str + s;
	instr.len = l->pos - s;
	t.type = Map_getTokenType(instr);
	return t;
}
