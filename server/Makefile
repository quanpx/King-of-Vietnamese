CFLAGS = -Wall
OBJFILES = user.o server.o authentication.o ../utils/utils.o
TARGET = server
all :$(TARGET)
$(TARGET):$(OBJFILES)
	$(CC) -pthread $(CFLAGS) -o $(TARGET) $(OBJFILES)
clean:
	rm -f $(OBJFILES) $(TARGET) *~