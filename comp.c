#include "c.h"

// alloc context
static u8 *start;
static u8 *offset;
static u8 *commited;
static u8 *end;
static usize pageSize;

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
					insertByte(t.number);
					t.number >>= 8;
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
					insertByte(tmp_num);
					tmp_num >>= 8;
				} while (n_bytes --> 0);
				for (usize i = 0; i < t.s.len; ++i) {
					insertByte(t.s.str[i]);
				}
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
