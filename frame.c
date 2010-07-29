#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

#define FRAMEBUFFER "/dev/graphics/fb0"

/*
struct Pixel {
	unsigned char r:5;
	unsigned char g:6;
	unsigned char b:5;
} __attribute__((packed)) pixel;
*/

int main()
{
	int fd = open(FRAMEBUFFER, O_RDWR);
	struct fb_var_screeninfo screeninfo;
	ioctl(fd, FBIOGET_VSCREENINFO, &screeninfo);
	int width = screeninfo.xres;
	int height = screeninfo.yres;

	int x, y;
	printf("x: %d, y: %d\n", width, height);
//	pixel.r = 0x1f;
//	pixel.g = pixel.b = 0;

	unsigned short *data = (unsigned short *)mmap(NULL, width*height*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	for (x = 0; x < height; ++x) {
		for (y = 0; y < width; ++y) {
			data[y + x * width] = 0x07e0;
		}
	}

	munmap(data, width*height);


	close(fd);
	return 0;
}
