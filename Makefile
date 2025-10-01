CFLAGS = -Wno-incompatible-pointer-types

main: arc.h arc.o

clean:
	rm main *.o
