#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 320
#define HEIGHT 480

#define RGB565_MASK_RED        0xF800
#define RGB565_MASK_GREEN                         0x07E0
#define RGB565_MASK_BLUE                         0x001F

void rgb565_2_rgb24(unsigned char *rgb24, unsigned short *rgb565)
{
	unsigned int i = 0;
	unsigned char R, G, B;
	for (i = 0; i < WIDTH*HEIGHT; ++i) {
		R = (*rgb565 & RGB565_MASK_RED) >> 11;
		G = (*rgb565 & RGB565_MASK_GREEN) >> 5;
		B = (*rgb565 & RGB565_MASK_BLUE);

		R <<= 3;
		G <<= 2;
		B <<= 3;
		*rgb24++ = R;
		*rgb24++ = G;
		*rgb24++ = B;
		++rgb565;
	}
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("Usage: %s file\n", argv[0]);
		exit(1);
	}

	FILE *fp = fopen(argv[1], "rb");
	if (!fp) {
		perror(argv[1]);
		exit(1);
	}

	int size565 = WIDTH*HEIGHT*2;
	int size888 = WIDTH*HEIGHT*3;
	unsigned short *image565 = (unsigned short*)malloc(size565);
	unsigned char *image888 = (unsigned char*)malloc(size888);

	fread(image565, size565, 1, fp);
	rgb565_2_rgb24(image888, image565);

	FILE *fpout = fopen("out.raw", "wb");
	if (!fpout) {
		perror("out.raw");
		goto _error;
	}

	fwrite(image888, size888, 1, fpout);
	fclose(fpout);
	printf("Output: out.raw (RGB888)\n");
_error:
	free(image565);
	free(image888);
	fclose(fp);
	return 0;
}

