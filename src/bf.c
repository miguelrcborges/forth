#include "c.h"

#include "map.c"
#include "lex.c"
#include "comp.c"
#include "vm.c"

#ifdef _WIN32
#include "win32.c"

int mainCRTStartup(void) {
#else
#include "x86_64-linux.c"

int main(void) {
#endif
	usize fd = io_open(str("src.bf"), IO_READ);
	usize len = io_len(fd);
	string src;
	src.str = mem_rescommit(len);
	src.len = len;
	io_read(fd, (u8 *)src.str, src.len);
	io_close(fd);
	Lex l = Lex_create(src);
	Bytecode *bytes = Compiler_compile(&l);
	mem_release((void *)src.str, src.len);
	VM_run(bytes);
	// let leak because program will end lol
	return 0;
}
