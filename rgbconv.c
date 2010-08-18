#include <stdio.h>
#include <stdlib.h>

#define WIDTH 480
#define HEIGHT 240

void rgb888to565(unsigned char *rgb888, unsigned short *rgb565)
{
	unsigned int i = 0;
	unsigned char r,g,b;
	unsigned short rgb;
	for (; i < WIDTH*HEIGHT; ++i) {
		r = *rgb888++;
		g = *rgb888++;
		b = *rgb888++;
		r /= 8;
		g /= 4;
		b /= 8;
		*rgb565++ = (r << 11) | (g << 5) | b;
	}
}

int main()
{
	FILE *fp = fopen("out.raw", "rb");
	if (!fp) {
		perror("fopen");
		exit(1);
	}
	FILE *fpout = fopen("out565.raw", "wb");
	if (!fpout) {
		perror("fopen");
		exit(1);
	}

	unsigned int size = WIDTH*HEIGHT;
	unsigned char *rgb888 = (unsigned char*)malloc(size*3);
	unsigned short *rgb565 = (unsigned short*)malloc(size*2);

	fread(rgb888, size*3, 1, fp);
	rgb888to565(rgb888, rgb565);

	fwrite(rgb565, size*2, 1, fpout);
	free(rgb888);
	free(rgb565);
	printf("In: out.raw (rgb888)\n");
	printf("Out: out565.raw (rgb565)\n");
	return 0;
}
