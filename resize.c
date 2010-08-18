#include <stdio.h>
#include <stdlib.h>

#define WIDTH 720
#define HEIGHT 240

void resize(unsigned char *inbuf, unsigned char *outbuf)
{
	unsigned int i = 0;
	for (; i < 240*240; ++i) {
		*outbuf++ = *inbuf++;
		*outbuf++ = *inbuf++;
		*outbuf++ = *inbuf++;

                *outbuf++ = *inbuf++;
                *outbuf++ = *inbuf++;
                *outbuf++ = *inbuf++;

		inbuf += 3;
	}
}

int main()
{
	FILE *fp = fopen("out.raw", "rb");
	FILE *fpout = fopen("out480.raw", "wb");
	unsigned int size = WIDTH*HEIGHT;

	unsigned char *inbuf = (unsigned char*)malloc(size*3);
	unsigned char *outbuf = (unsigned char*)malloc(480*240*3);
	fread(inbuf, size*3, 1, fp);
	resize(inbuf, outbuf);

	fwrite(outbuf, 480*240*3, 1, fpout);
	printf("In: out.raw (720x240)\n");
	printf("Out: out480.raw (480x240)\n");
	fclose(fp);
	fclose(fpout);
	return 0;
}

