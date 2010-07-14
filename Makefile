TARGET = v list yuv

all:$(TARGET)

v:v.c
	gcc -Wall -o $@ $<

list:list.c
	gcc -Wall -o $@ $<

yuv:yuv.c
	gcc -Wall -o $@ $<

clean:
	rm -rf $(TARGET) *.raw

