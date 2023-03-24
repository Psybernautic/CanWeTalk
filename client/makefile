
./bin/client: ./obj/client.o ./obj/winControl.o
	cc ./obj/client.o ./obj/winControl.o -lpthread -lncurses -o ./bin/client

./obj/client.o: ./src/client.c ./inc/winControl.h
	cc -c ./src/client.c -o ./obj/client.o

./obj/winControl.o: ./src/winControl.c ./inc/winControl.h
	cc -c ./src/winControl.c -o ./obj/winControl.o

all: ./bin/client

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o