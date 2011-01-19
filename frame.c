#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

#define FRAMEBUFFER "/dev/graphics/fb0"
//#define FRAMEBUFFER "/dev/overlay"
//#define FRAMEBUFFER "/dev/fb0"


int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s [RGB565 file]\n", argv[0]);
		exit(1);
	}

	int fd = open(FRAMEBUFFER, O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(1);
	}
	struct fb_var_screeninfo screeninfo;
	ioctl(fd, FBIOGET_VSCREENINFO, &screeninfo);
	int width = screeninfo.xres;
	int height = screeninfo.yres;
	int pixel = screeninfo.bits_per_pixel;

	int x, y;
	printf("x: %d, y: %d, pixel: %d\n", width, height, pixel);


	unsigned char *data = (unsigned char *)mmap(NULL, width*height*(screeninfo.bits_per_pixel/8),
		PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	if (data == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	FILE *fp = fopen(argv[1], "wb");
	if (!fp) {
		perror("fopen");
		exit(1);
	}
	fwrite(data, width*height*(pixel/8), 1, fp);
	fclose(fp);
/*
	for (x = 0; x < height; ++x) {
		for (y = 0; y < width; ++y) {
			data[y + x * width] = 0x55;
		}
	}
*/
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
