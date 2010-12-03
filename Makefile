TARGET = v list yuv frame show eav parse parse2 fake trans overlay seq conv rgb565 rgbconv resize c play deinterlace
ARMCC=arm-none-linux-gnueabi-gcc
CC=gcc
CFLAGS=-static

all:$(TARGET)

v:v.c
	$(ARMCC) -Wall -o $@ $< $(CFLAGS) -O2

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
	$(ARMCC) -O2 -Wall -o $@ $< $(CFLAGS)

fake:fake.c
	$(CC) -Wall -o $@ $<

trans:trans.c
	$(CC) -Wall -g -o $@ $<

overlay:overlay.c
	$(ARMCC) -Wall -o $@ $< $(CFLAGS)

seq:seq.c
	$(CC) -Wall -o $@ $< -g

conv:conv.c
	$(CC) -Wall -o $@ $< -g

rgb565:rgb565.c
	$(CC) -Wall -o $@ $< -g

rgbconv:rgbconv.c
	$(CC) -Wall -o $@ $< -g

resize:resize.c
	$(CC) -Wall -o $@ $< -g

c:capture.c
	$(ARMCC) -Wall -o c $< -g $(CFLAGS)

play:play.c
	$(ARMCC) -Wall -O2 -o $@ $< $(CFLAGS)

deinterlace:deinterlace.c
	$(CC) -Wall -o $@ $< -g

clean:
	rm -rf $(TARGET) *.raw

