#
# this makefile will compile and and all source
# found in the "CryptoMagic" directory. The purpose
# of this program is to encrypt and decrypt files.
# Effectively converting from txt to crp and back.
# 
# 
# =======================================================
#                  Server
# =======================================================
# FINAL BINARY Target
.bin/server : ./obj/server.o 
	cc ./obj/server.o -lpthread -o  ./bin/chat-server
#
# =======================================================
#                     Dependencies
# =======================================================
./obj/server.o : 
	cc -c ./src/server.c -o ./obj/server.o
	      
  
#
# =======================================================
# Other targets
# =======================================================                     
all : ./bin/server

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o  
