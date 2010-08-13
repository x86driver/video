#include <stdio.h>
#include <stdlib.h>

struct RGB {
	unsigned char r,g,b;
} __attribute__((packed));

int main()
{
	unsigned int x = 720;
	unsigned int y = 240;

	FILE *fp = fopen("my.raw", "rb");
	FILE *out = fopen("out.raw", "wb");
	if (!fp) {
		perror("fopen");
		exit(1);
	}
	if (!out) {
		perror("fopen out");
		exit(1);
	}

	unsigned char *buf = malloc(x*y*2);
	unsigned char outbuf[4];
	if (buf == NULL) {
		perror("malloc");
		fclose(fp);
		fclose(out);
		exit(1);
	}

	fread((void*)buf, x*y*2, 1, fp);
	unsigned char *ptrY = buf;
	unsigned char *ptrU = buf+(x*y);
	unsigned char *ptrV = buf+(x*y)+(x*y/2);

	unsigned int i = 0;

	for (; i < x*y/2; ++i) {
		outbuf[0] = *ptrU++;
		outbuf[1] = *ptrY++;
		outbuf[2] = *ptrV++;
		outbuf[3] = *ptrY++;
		fwrite((void*)&outbuf[0], sizeof(outbuf), 1, out);
	}


/*
	unsigned char rgb[3];
	for (; i < 480*272; ++i) {
		rgb[0] = *ptrY++;
		rgb[1] = *ptrU++;
		rgb[2] = *ptrY++;
		fwrite((void*)&rgb[0], sizeof(rgb), 1, out);
	}
*/
	free(buf);
	fclose(fp);
	fclose(out);
	return 0;
}

