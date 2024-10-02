GSL_PATH ?= /net/ens/renault/save/gsl-2.6/install
CFLAGS = -std=c99 -Wall -Wextra -fPIC -g3 -I$(GSL_PATH)/include -ftest-coverage -fprofile-arcs
LDFLAGS = -L$(GSL_PATH)/lib \
	-Wl,-rpath,${GSL_PATH}/lib  -lgsl -lgslcblas -lm -ldl

all: build

build: server client

src/graph.o: src/graph.c src/graphext.h
	gcc -o $@ -c $< $(CFLAGS)

src/queen.o: src/queen.c src/queen.h
	gcc -o $@ -c $< $(CFLAGS)

src/player.o: src/player.c src/player.h
	gcc -o $@ -c $< $(CFLAGS)

libplayer.so: src/player.o src/queen.o src/graph.o
	gcc $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

src/player2.o: src/player2.c src/player.h
	gcc -o $@ -c $< $(CFLAGS)

libplayer2.so: src/player2.o src/queen.o src/graph.o
	gcc $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)



src/server.o: src/server.c 
	gcc -o $@ -c $^ $(CFLAGS)

server: src/server.o src/graph.o
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

client: libplayer.so libplayer2.so

tst/test.o: tst/test.c
	gcc -o $@ -c $< $(CFLAGS)

alltests: tst/test.o src/player.o src/queen.o src/graph.o 
	gcc $(CFLAGS) -Isrc -ftest-coverage -fprofile-arcs $^ $(LDFLAGS) -o alltests -lgcov


test: alltests

install: server client alltests
	mkdir -p install
	cp server ./install
	cp alltests ./install
	cp lib*.so ./install	

clean:
	rm -f *~ *.o src/*~ src/*.o tst/*.o

cleanall: clean
	rm -f server alltests *.so
	rm -f *.out *.gcno
	rm -f src/*.gcno src/*.gcda
	rm -f tst/*.gcno tst/*.gcda *.gcda

.PHONY: client install test clean
