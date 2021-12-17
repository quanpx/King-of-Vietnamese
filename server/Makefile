CFLAGS = -Wall
OBJFILES = user.o question.o server.o player.o room.o authentication.o game.o
TARGET = server
all :$(TARGET)
$(TARGET):$(OBJFILES)
	$(CC) -pthread $(CFLAGS) -o $(TARGET) $(OBJFILES)
clean:
	rm -f $(OBJFILES) $(TARGET) *~