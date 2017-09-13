CHESSPOS_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

chesspos: chesspos.o
	gcc -o chesspos chesspos.o

chesspos.o: chesspos.c chesspos.make
	gcc ${CHESSPOS_C_FLAGS} -o chesspos.o chesspos.c

clean:
	rm -f chesspos chesspos.o
