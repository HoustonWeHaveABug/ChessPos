CHESSPOS_DEBUG_C_FLAGS=-c -g -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

chesspos_debug: chesspos_debug.o
	gcc -g -o chesspos_debug chesspos_debug.o

chesspos_debug.o: chesspos.c chesspos_debug.make
	gcc ${CHESSPOS_DEBUG_C_FLAGS} -o chesspos_debug.o chesspos.c

clean:
	rm -f chesspos_debug chesspos_debug.o
