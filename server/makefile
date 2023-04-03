#
# this makefile will compile and and all source for
# the server application found in the "canWeTalk directory.
# The purpose of this program is set up a server that will
# process messages from up to 10 clients with their agreed
# message protocol. This will stablish a complete chat system
# 
# 
# =======================================================
#						Server
# =======================================================
# FINAL BINARY Target
.bin/server: ./obj/server.o ./obj/clientHandler.o
	cc ./obj/server.o ./obj/clientHandler.o -lpthread -o ./bin/chat-server
#
# =======================================================
#                     Dependencies
# =======================================================
./obj/server.o: ./src/server.c ./inc/clientHandler.h
	cc -c ./src/server.c -o ./obj/server.o

./obj/clientHandler.o: ./src/clientHandler.c ./inc/clientHandler.h
	cc -c ./src/clientHandler.c -o ./obj/clientHandler.o
#
# =======================================================
# Other targets
# =======================================================                     
all: ./bin/server

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o  
