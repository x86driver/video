#include <stdio.h>
#include <stdlib.h>

#define WIDTH 176
#define HEIGHT 144

typedef unsigned short __u16;

static void ccvt_420p_rgb565(int width, int height, const unsigned
			     char *src, unsigned short *dst)
{
	int line, col, linewidth;
	int y, u, v, yy, vr, ug, vg, ub;
	int r, g, b;
	const unsigned char *py, *pu, *pv;

	linewidth = width >> 1;
	py = src;
	pu = py + (width * height);
	pv = pu + (width * height) / 4;

	y = *py++;
	yy = y << 8;
	u = *pu - 128;
	ug =   88 * u;
	ub = 454 * u;
	v = *pv - 128;
	vg = 183 * v;
	vr = 359 * v;

	for (line = 0; line < height; line++) {
		for (col = 0; col < width; col++) {
			r = (yy +      vr) >> 8;
			g = (yy - ug - vg) >> 8;
			b = (yy + ub     ) >> 8;

			if (r < 0)   r = 0;
			if (r > 255) r = 255;
			if (g < 0)   g = 0;
			if (g > 255) g = 255;
			if (b < 0)   b = 0;
			if (b > 255) b = 255;
			*dst++ = (((__u16)r>>3)<<11) | (((__u16)g>>2)<<5) | (((__u16)b>>3)<<0);

			y = *py++;
			yy = y << 8;
			if (col & 1) {
				pu++;
				pv++;

				u = *pu - 128;
				ug =   88 * u;
				ub = 454 * u;
				v = *pv - 128;
				vg = 183 * v;
				vr = 359 * v;
			}
		} /* ..for col */
		if ((line & 1) == 0) { // even line: rewind
			pu -= linewidth;
			pv -= linewidth;
		}
	} /* ..for line */
}

void rgb5652rgb888(unsigned int width, unsigned int height, unsigned char *image,unsigned char *image888)
{
	unsigned int i = 0;
	for (; i < width*height; ++i) {
		unsigned char R,G,B;
		B=(*image) & 0x1F;//000BBBBB
		G=(( *(image+1) << 3 ) & 0x38) + (( *image >> 5 ) & 0x07) ;//得到00GGGGGG00
		R=( *(image+1) >> 3 ) & 0x1F; //得到000RRRRR
		*(image888+0)=B * 255 / 63; // 把5bits映射到8bits去，自己可以优化一下算法，下同
		*(image888+1)=G * 255 / 127;
		*(image888+2)=R * 255 / 63;
		++image;
		++image888;
	}
}

int main()
{
	FILE *fp = fopen("my.raw", "rb");
	if (!fp) {
		perror("fopen");
		exit(1);
	}

	FILE *outfp = fopen("out.raw", "wb");
	if (!outfp) {
		perror("out.raw");
		exit(1);
	}

	FILE *rgb888fp = fopen("out2.raw", "wb");
	if (!rgb888fp) {
		perror("out2.raw");
		exit(1);
	}

	unsigned int imagesize = WIDTH*HEIGHT+(WIDTH*HEIGHT/2);
	unsigned char *inbuf = (unsigned char*)malloc(imagesize);
	unsigned short *outbuf = (unsigned short*)malloc(WIDTH*HEIGHT*2);
	unsigned char *rgb888buf = (unsigned char*)malloc(WIDTH*HEIGHT*3);
	if (inbuf == NULL || outbuf == NULL || rgb888buf == NULL)
		perror("malloc");


	fread(inbuf, imagesize, 1, fp);
	ccvt_420p_rgb565(WIDTH, HEIGHT, inbuf, outbuf);
	fwrite(outbuf, WIDTH*HEIGHT*2, 1, outfp);

	rgb5652rgb888(WIDTH, HEIGHT, (unsigned char*)outbuf, rgb888buf);
	unsigned int i = 0;
	unsigned char *buf = rgb888buf;
	for (; i < WIDTH*HEIGHT; ++i) {
		*buf++ = 0xff;
		*buf++ = 0x0;
		*buf++ = 0x0;
	}
	fwrite(rgb888buf, WIDTH*HEIGHT*3, 1, rgb888fp);

	free(inbuf);
	free(outbuf);
	free(rgb888buf);
	fclose(fp);
	fclose(outfp);
	fclose(rgb888fp);

	printf("Open: my.raw\n");
	printf("Output RGB565: out.raw\n");
	printf("Output RGB888: out2.raw\n");
	return 0;
}

