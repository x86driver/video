TARGET = v list yuv frame show eav parse parse2
ARMCC=arm-none-linux-gnueabi-gcc
CC=gcc
CFLAGS=-static

all:$(TARGET)

v:v.c
	$(ARMCC) -Wall -o $@ $< $(CFLAGS)

list:list.c
	$(ARMCC) -Wall -o $@ $< $(CFLAGS)

yuv:yuv.c
	$(CC) -Wall -o $@ $< $(CFLAGS)

frame:frame.c
	$(ARMCC) -Wall -o $@ $< $(CFLAGS)

eav:eav.c
	gcc -Wall -o $@ $< $(CFLAGS)

parse:parse.c
	gcc -Wall -o $@ $< -g

parse2:parse2.c
	gcc -Wall -o $@ $< -g

show:show.c
	$(ARMCC) -Wall -o $@ $< $(CFLAGS)

clean:
	rm -rf $(TARGET) *.raw

