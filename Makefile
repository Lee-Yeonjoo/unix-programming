CC = gcc
CFLAGS = -g
CLIENT_TARGET = client
SERVER_TARGET = server
CLIENT_OBJECTS = client.o client-fun.o
SERVER_OBJECTS = server.o server-fun.o
SRCS = client.c client-fun.c server.c server-fun.c

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	$(CC) -o $@ $(CFLAGS) $(CLIENT_OBJECTS)

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(CC) -o $@ $(CFLAGS) $(SERVER_OBJECTS) -pthread

client.o: client.c
	$(CC) -c $(CFLAGS) $< -o $@

client-fun.o: client-fun.c
	$(CC) -c $(CFLAGS) $< -o $@

server.o: server.c
	$(CC) -c $(CFLAGS) $< -o $@

server-fun.o: server-fun.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(CLIENT_OBJECTS) $(SERVER_OBJECTS) $(CLIENT_TARGET) $(SERVER_TARGET)
