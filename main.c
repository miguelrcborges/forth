#include "c.h"

Arena a;

#ifdef _WIN32
int mainCRTStartup(void) {
#else
int main(void) {
#endif
	a = Arena_create(0);
	string src = io_readFile(&a, str("src.bf"));
	Lex l = Lex_create(src);
	Token t;
	while ((t = Lex_next(&l)).type) {
		VM_execute(t);
	}

	return 0;
}
