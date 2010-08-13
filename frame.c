#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

//#define FRAMEBUFFER "/dev/graphics/fb0"
#define FRAMEBUFFER "/dev/overlay"
//#define FRAMEBUFFER "/dev/fb0"


struct Pixel {
	unsigned char r:5;
	unsigned char g:6;
	unsigned char b:5;
} __attribute__((packed)) pixel;


int main()
{
	int fd = open(FRAMEBUFFER, O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(1);
	}
	struct fb_var_screeninfo screeninfo;
	ioctl(fd, FBIOGET_VSCREENINFO, &screeninfo);
	int width = screeninfo.xres;
	int height = screeninfo.yres;

	int x, y;
	printf("x: %d, y: %d, pixel: %d\n", width, height, screeninfo.bits_per_pixel);


	unsigned char *data = (unsigned char *)mmap(NULL, width*height*(screeninfo.bits_per_pixel/8),
		PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	for (x = 0; x < height; ++x) {
		for (y = 0; y < width; ++y) {
			data[y + x * width] = 0x55;
		}
	}

	munmap(data, width*height);

/*
	unsigned char *buf = malloc(480*272*4);
	for (x = 0; x < 480*272*2; x+=4) {
		buf[x] = 0xff;
		buf[x+1] = 0x00;
		buf[x+2] = 0;
		buf[x+3] = 0;
	}

	write(fd, buf, 480*272*4);
	free(buf);
*/
	close(fd);
	return 0;
}
