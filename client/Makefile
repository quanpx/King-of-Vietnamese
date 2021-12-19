CFLAGS = -Wall -pthread
OBJFILES = client.o ../utils/utils.o
TARGET = client
all :$(TARGET)
$(TARGET):$(OBJFILES)
	$(CC)  $(CFLAGS)  -o $(TARGET) $(OBJFILES)
clean:
	rm -f $(OBJFILES) $(TARGET) *~