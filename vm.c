#include "c.h"

static i32 stack[1 << 16];
static u32 stack_pos = 0;
static u32 instr_pos = 0;

static void checkElements(u32 n);

void VM_run(Bytecode *bytes) {
	for (;;) {
		Bytecode b = bytes[instr_pos];
		u8 instr = b.instr & 0b00111111;
		switch (instr) {
			case EOF: {
				goto end;
			}
			case ADD: {
				checkElements(2);
				stack_pos -= 1;
				stack[stack_pos-1] = stack[stack_pos-1]+stack[stack_pos];
				instr_pos += 1;
				break;
			}
			case SUB: {
				checkElements(2);
				stack_pos -= 1;
				stack[stack_pos-1] = stack[stack_pos-1]-stack[stack_pos];
				instr_pos += 1;
				break;
			}
			case MUL: {
				checkElements(2);
				stack_pos -= 1;
				stack[stack_pos-1] = stack[stack_pos-1]*stack[stack_pos];
				instr_pos += 1;
				break;
			}
			case DIV: {
				checkElements(2);
				stack_pos -= 1;
				stack[stack_pos-1] = stack[stack_pos-1]/stack[stack_pos];
				instr_pos += 1;
				break;
			}
			case SWAP: {
				checkElements(2);
				u32 i = stack_pos - 1;
				u32 tmp = stack[i];
				stack[i] = stack[i-1];
				stack[i-1] = tmp;
				instr_pos += 1;
				break;
			}
			case POP: {
				u8 writeBuffer[12] = {
					[sizeof(writeBuffer)-1] = '\n'
				};
				checkElements(1);
				u8 *w = writeBuffer + (sizeof(writeBuffer) - 2);
				stack_pos -= 1;
				i32 num = stack[stack_pos];
				u32 unum = num >= 0 ? num : -num;
				do {
					*w = unum % 10 + '0';
					unum /= 10;
					w -= 1;
				} while (unum > 0);
				if (num < 0) {
					*w = '-';
					w -= 1;
				}
				string s;
				s.str = (const u8 *)w + 1;
				s.len = (writeBuffer + (sizeof(writeBuffer) - 1)) - w;
				io_write(getStdOut(), s);
				instr_pos += 1;
				break;
			}
			case DUP: {
				checkElements(1);
				stack[stack_pos] = stack[stack_pos-1];
				stack_pos += 1;
				instr_pos += 1;
				break;
			}
			case DROP: {
				checkElements(1);
				stack_pos -= 1;
				instr_pos += 1;
				break;
			}
			case NIP: {
				checkElements(2);
				stack_pos -= 1;
				stack[stack_pos-1] = stack[stack_pos];
				instr_pos += 1;
				break;
			}
			case ROT: {
				checkElements(3);
				u32 i = stack_pos - 1;
				u32 second = stack[i];
				u32 first = stack[i-1];
				u32 third = stack[i-2];
				stack[i-2] = first;
				stack[i-1] = second;
				stack[i] = third;
				instr_pos += 1;
				break;
			}
			case NUMBER: {
				u8 n_bytes = (0b11000000 & b.instr) >> 6;
				u32 number = 0;
				do {
					instr_pos += 1;
					number <<= 8;
					number |= (u32) (bytes[instr_pos].instr);
				} while (n_bytes --> 0);
				stack[stack_pos] = number;
				stack_pos += 1;
				instr_pos += 1;
				break;
			}
			case STRING: {
				u8 n_bytes = (0b11000000 & b.instr) >> 6;
				string s = {0};
				do {
					instr_pos += 1;
					s.len <<= 8;
					s.len |= (u32) (bytes[instr_pos].instr);
				} while (n_bytes --> 0);
				instr_pos += 1;
				s.str = &(bytes[instr_pos].instr);
				usize stdout = getStdOut();
				io_write(stdout, s);
				io_write(stdout, str("\n"));
				instr_pos += s.len;
				break;
			}
			default: {
				io_write(getStdErr(), str("Invalid instruction found. Exiting.\n"));
				die(2);
				__builtin_unreachable();
			}
		}
	}
end:
	return;
}

static void checkElements(u32 n) {
	assert(n > 0);
	if (unlikely(stack_pos < n)) {
		io_write(getStdErr(), str("Not enough stack elements for the executing operation. Exiting.\n"));
		die(1);
	}
}
