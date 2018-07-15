all:
	gcc -o homeworkServer server.c -lpthread -lrt -lm
	gcc -o clientApp client.c -lpthread -lrt -lm
clean:
	rm homeworkServer clientApp log.*