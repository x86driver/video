TARGET = v list yuv frame
CC=arm-none-linux-gnueabi-gcc
CFLAGS=-static

all:$(TARGET)

v:v.c
	$(CC) -Wall -o $@ $< $(CFLAGS)

list:list.c
	$(CC) -Wall -o $@ $< $(CFLAGS)

yuv:yuv.c
	$(CC) -Wall -o $@ $< $(CFLAGS)

frame:frame.c
	$(CC) -Wall -o $@ $< $(CFLAGS)

clean:
	rm -rf $(TARGET) *.raw

