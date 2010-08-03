#include <stdio.h>
#include <stdlib.h>

struct RGB {
	unsigned char r,g,b;
} __attribute__((packed));

int main()
{
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

	struct RGB rgb;
	unsigned char data;
	unsigned int i = 0;

	for (; i < 240*136; ++i) {
		fread((void*)&data, sizeof(data), 1, fp);
		rgb.r = rgb.g = rgb.b = data;
		fwrite((void*)&rgb, sizeof(rgb), 1, out);
	}

	fclose(fp);
	fclose(out);
	return 0;
}

