CFLAGS = -Wno-incompatible-pointer-types

main: arc.h arc.o

clean:
	rm main *.o

format:
	# Requires GNU Indent
	indent -kr --no-tabs *.c
	rm *.c~

