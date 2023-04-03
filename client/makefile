
#
# this makefile will compile and and all source for
# the client application found in the "canWeTalk directory.
# The purpose of this program is set up a client that will
# create, send and receive messages from up to other 9 clients
# with their agreed message protocol. This will stablish a
# complete chat system
# 
# 
# =======================================================
#						Client
# =======================================================
# FINAL BINARY Target
./bin/client: ./obj/client.o ./obj/winControl.o 
	cc ./obj/client.o ./obj/winControl.o -lpthread -lncurses -o ./bin/chat-client

#
# =======================================================
#                     Dependencies
# =======================================================
./obj/client.o: ./src/client.c ./inc/winControl.h ./inc/client.h
	cc -c ./src/client.c -o ./obj/client.o

./obj/winControl.o: ./src/winControl.c ./inc/winControl.h
	cc -c ./src/winControl.c -o ./obj/winControl.o
#
# =======================================================
# Other targets
# =======================================================    
all: ./bin/client

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o