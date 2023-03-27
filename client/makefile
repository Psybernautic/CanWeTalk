
./bin/client: ./obj/client.o ./obj/winControl.o
	cc ./obj/client.o ./obj/winControl.o -lpthread -lncurses -o ./bin/chat-client

./obj/client.o: ./src/client.c ./inc/winControl.h
	cc -c -ggdb ./src/client.c -o ./obj/client.o

./obj/winControl.o: ./src/winControl.c ./inc/winControl.h
	cc -c -ggdb ./src/winControl.c -o ./obj/winControl.o

all: ./bin/client

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o