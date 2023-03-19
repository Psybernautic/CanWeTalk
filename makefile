#
# This makefile will compile all sources listed for all.
# =======================================================


all :
	+$(MAKE) -C client
	+$(MAKE) -C server

	rm -rf ./program

	mkdir program
	mkdir program/bin
	mkdir tmp

	cp server/bin/server program/bin
	cp client/bin/client program/bin


clean:
	rm -rf */bin/*
	rm -rf */obj/*.o
	rm -rf */inc/*.h~
	rm -rf */src/*.c~
	rm -rf */lib/*.a
	rm -rf */bin/*.so
	rm -rf ./program
	rm -rf ./tmp/

