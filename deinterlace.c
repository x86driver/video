#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 720
#define HEIGHT 240

int main()
{
	int i = 0;
	char buf0[WIDTH*HEIGHT*3];
	char buf1[WIDTH*HEIGHT*3];
	char out[WIDTH*HEIGHT*2*3];

	FILE *fp0 = fopen("my0.rgb", "rb");
	FILE *fp1 = fopen("my1.rgb", "rb");
	FILE *outfp = fopen("out.rgb", "wb");

	if (!fp0 || !fp1 || !outfp) {
		perror("fopen");
		exit(1);
	}

	if (fread(buf0, sizeof(buf0), 1, fp0) != 1)
		printf("Can't read buf0\n");
	if (fread(buf1, sizeof(buf1), 1, fp1) != 1)
		printf("Can't read buf1\n");

	for (i = 0; i < HEIGHT; ++i) {
		//把 buf0 第 i 條 copy 到 out 第 i*2 條
		//把 buf1 第 i 條 copy 到 out 第 i*2+1 條
//		memcpy(out+i*WIDTH*3, buf0+i*WIDTH*3, WIDTH*3);
		memcpy(out+i*2*WIDTH*3, buf0+i*WIDTH*3, WIDTH*3);
		memcpy(out+(i*2+1)*WIDTH*3, buf1+i*WIDTH*3, WIDTH*3);
	}

//	memcpy(out, buf0, WIDTH*HEIGHT*3);

	fwrite(out, sizeof(out), 1, outfp);

	fclose(fp0);
	fclose(fp1);
	fclose(outfp);
	return 0;
}
