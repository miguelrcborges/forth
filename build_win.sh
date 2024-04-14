if [ -z "$CC" ]; then
	CC="cc"
fi

$CC -nostdlib -g *.c -L. -llib -luser32 -lkernel32
