#include "c.h"

static u32 stack[1 << 16];
static u32 stack_pos = 0;

static void checkElements(u32 n);
static void invalid(Token t);
static void number(Token t);
static void stringf(Token t);
static void add(Token t);
static void sub(Token t);
static void mul(Token t);
static void div(Token t);
static void swap(Token t);
static void pop(Token t);
static void dup(Token t);
static void drop(Token t);
static void nip(Token t);
static void rot(Token t);

void (*instrTable[TOKENTYPE_COUNT])(Token) = {
	[INVALID] = invalid,
	[NUMBER] = number,
	[STRING] = stringf,
	[ADD] = add,
	[SUB] = sub,
	[MUL] = mul,
	[DIV] = div,
	[SWAP] = swap,
	[POP] = pop,
	[DUP] = dup,
	[DROP] = drop,
	[NIP] = nip,
	[ROT] = rot
};

void VM_execute(Token t) {
	instrTable[t.type](t);
}

static void checkElements(u32 n) {
	assert(n > 0);
	if (unlikely(stack_pos < n)) {
		io_write(getStdErr(), str("Not enough stack elements for the executing operation. Exiting.\n"));
		die(1);
	}
}

static void invalid(Token t) {
	io_write(getStdErr(), str("Invalid token found. Exiting.\n"));
	die(1);
}

static void number(Token t) {
	stack[stack_pos] = t.number;
	stack_pos += 1;
}

static void stringf(Token t) {
	ArenaState as = Arena_saveState(&a);
	string conc = string_build(&a, t.s, str("\n"));
	io_write(getStdOut(), conc);
	Arena_rollback(as);
}

static void add(Token t) {
	checkElements(2);
	stack_pos -= 1;
	stack[stack_pos-1] = stack[stack_pos-1]+stack[stack_pos];
}

static void sub(Token t) {
	checkElements(2);
	stack_pos -= 1;
	stack[stack_pos-1] = stack[stack_pos-1]-stack[stack_pos];
}

static void mul(Token t) {
	checkElements(2);
	stack_pos -= 1;
	stack[stack_pos-1] = stack[stack_pos-1]*stack[stack_pos];
}

static void div(Token t) {
	checkElements(2);
	stack_pos -= 1;
	stack[stack_pos-1] = stack[stack_pos-1]/stack[stack_pos];
}

static void swap(Token t) {
	checkElements(2);
	u32 i = stack_pos - 1;
	u32 tmp = stack[i];
	stack[i] = stack[i-1];
	stack[i-1] = tmp;
}

static void pop(Token t) {
	checkElements(1);
	stack_pos -= 1;
	u32 num = stack[stack_pos];
	ArenaState as = Arena_saveState(&a);
	string s = string_fmtu64(&a, num);
	string conc = string_build(&a, s, str("\n"));
	io_write(getStdOut(), conc);
	Arena_rollback(as);
}

static void dup(Token t) {
	checkElements(1);
	stack[stack_pos] = stack[stack_pos-1];
	stack_pos += 1;
}

static void drop(Token t) {
	checkElements(1);
	stack_pos -= 1;
}

static void nip(Token t) {
	checkElements(2);
	stack_pos -= 1;
	stack[stack_pos-1] = stack[stack_pos];
}

static void rot(Token t) {
	u32 i = stack_pos - 1;
	u32 second = stack[i];
	u32 first = stack[i-1];
	u32 third = stack[i-2];
	stack[i-2] = first;
	stack[i-1] = second;
	stack[i] = third;
}
