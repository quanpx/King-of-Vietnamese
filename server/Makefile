CFLAGS = -Wall
OBJFILES = user.o server.o authentication.o room.o player.o question.o ../utils/utils.o ../utils/notification.o game.o
TARGET = server
all :$(TARGET)
$(TARGET):$(OBJFILES)
	$(CC) -pthread $(CFLAGS) -o $(TARGET) $(OBJFILES)
clean:
	rm -f $(OBJFILES) $(TARGET) *~