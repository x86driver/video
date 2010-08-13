#include <stdio.h>

struct RGB {
	unsigned char r,g,b;
} __attribute__((packed));

struct YUV {
//	unsigned char y1, u, y2, v; //ok in Notebook
	unsigned char u, y1, v, y2;
} __attribute__((packed));

int main()
{
	FILE *out = fopen("out2.raw", "wb");
	FILE *in = fopen("out.raw", "rb");

	struct RGB rgb;
	struct YUV yuv;

	int r,g,b;

	while (!feof(in)) {
		fread((void*)&yuv, sizeof(yuv), 1, in);

		r = yuv.y1 + (1.4075*(yuv.v-128));
		g = yuv.y1 - (0.3455*(yuv.u-128)-(0.7169*(yuv.v-128)));
		b = yuv.y1 + (1.7790*(yuv.u-128));
		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;
		rgb.r = r;
		rgb.g = g;
		rgb.b = b;
		fwrite((void*)&rgb, sizeof(rgb), 1, out);

                r = yuv.y2 + (1.4075*(yuv.v-128));
                g = yuv.y2 - (0.3455*(yuv.u-128)-(0.7169*(yuv.v-128)));
                b = yuv.y2 + (1.7790*(yuv.u-128));
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;
                if (r < 0) r = 0;
                if (g < 0) g = 0;
                if (b < 0) b = 0;
		rgb.r = r;
		rgb.g = g;
		rgb.b = b;
		fwrite((void*)&rgb, sizeof(rgb), 1, out);
	}

	fclose(in);
	fclose(out);
	return 0;
}
