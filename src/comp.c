#include "c.h"

// alloc context
static u8 *start;
static u8 *offset;
static u8 *commited;
static u8 *end;
static usize pageSize;

static struct {
	u8 *cache;
} ifstack[1 << 8];
static u8 ifstack_pos;

static void insertByte(u8 byte);

void Compiler_free(void) {
	if (likely(start)) {
		mem_release(start, (usize) (end - start));
	}
}

Bytecode *Compiler_compile(Lex *l) {
	if (start == NULL) {
		pageSize = mem_getPageSize();
		start = mem_reserve(1 << 24);
		commited = start;
		end = start + (1 << 24);
	}
	offset = start;

	Token t;
	for (;;) {
		t = Lex_next(l);
		switch (t.type) {
			case EOF: {
				goto end;
			}
			case ADD: case SUB: case MUL:
			case DIV: case SWAP: case POP:
			case DUP: case DROP: case NIP:
			case ROT: {
				insertByte(t.type);
				break;
			}
			case NUMBER: {
				u8 n_bytes = 0; 
				u32 tmp_num = t.number >> 8;
				while (tmp_num) {
					n_bytes += 1;
					tmp_num >>= 8;
				}
				insertByte(NUMBER | (n_bytes << 6));
				do {
					insertByte(t.number >> (n_bytes * 8));
				} while (n_bytes --> 0);
				break;
			}
			case STRING: {
				u8 n_bytes = 0; 
				usize strlen = t.s.len;
				u32 tmp_num = t.s.len >> 8;
				while (tmp_num) {
					n_bytes += 1;
					tmp_num >>= 8;
				}
				insertByte(STRING | (n_bytes << 6));
				tmp_num = t.s.len;
				do {
					insertByte(tmp_num >> (n_bytes * 8));
				} while (n_bytes --> 0);
				for (usize i = 0; i < t.s.len; ++i) {
					insertByte(t.s.str[i]);
				}
				break;
			}
			case TOKEN_IF: {
				// TODO: A better if system that uses variable number size
				insertByte(JUMPIFNOT | (0b11 << 6));
				ifstack[ifstack_pos].cache = offset;
				for (usize i = 0; i < 4; ++i) {
					insertByte(0);
				}
				ifstack_pos += 1;
				break;
			}
			case TOKEN_ELSE: {
				if (ifstack_pos == 0) {
					io_write(getStdErr(), str("Else without an if found. Exiting.\n"));
					die(2);
					__builtin_unreachable();
				}
				insertByte(JUMP | (0b11 << 6));
				u8 *if_done_cache = offset;
				for (usize i = 0; i < 4; ++i) {
					insertByte(0);
				}
				u32 else_start = offset - start;
				u32 nbytes = 0b11;
				u32 pos = 0;
				do {
					ifstack[ifstack_pos-1].cache[pos] = else_start >> (8 * nbytes);
					pos += 1;
				} while (nbytes --> 0);
				ifstack[ifstack_pos-1].cache = if_done_cache;
				break;
			}
			case TOKEN_DONE: {
				if (ifstack_pos == 0) {
					io_write(getStdErr(), str("Done without an if found. Exiting.\n"));
					die(2);
					__builtin_unreachable();
				}
				u32 jump = offset - start;
				u32 nbytes = 0b11;
				u32 pos = 0;
				do {
					ifstack[ifstack_pos-1].cache[pos] = jump >> (8 * nbytes);
					pos += 1;
				} while (nbytes --> 0);
				ifstack_pos -= 1;
				break;
			}
			default: {
				io_write(getStdErr(), str("Invalid token found. Exiting.\n"));
				die(2);
				__builtin_unreachable();
			}
		}
	}

end:
	if (ifstack_pos != 0) {
		io_write(getStdErr(), str("Non terminated ifs not found. Exiting.\n"));
		die(2);
		__builtin_unreachable();
	}
	insertByte(0);
	return (Bytecode *)start;
}

static void insertByte(u8 byte) {
	if (unlikely(offset == end)) {
		io_write(getStdErr(), str("Out of virtual memory on compilations. Oops.\n"));
		die(2);
		__builtin_unreachable();
	}
	if (unlikely(offset == commited)) {
		mem_commit(commited, pageSize);
		commited += pageSize;
	}
	*offset = byte;
	offset += 1;
}
