#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 720
#define HEIGHT 240

double YuvToRgb[3][3]= {{1,      0, 1.4022},
                        {1,   -0.3456,-0.7145},
                        {1,  1.771,      0}};

struct RGB {
	unsigned char r,g,b;
} __attribute__((packed));

void WriteRaw(int width, int height, unsigned char *r, unsigned char *g, unsigned char *b, char *outfilename)
{
	struct RGB rgb;
	FILE *fp = fopen(outfilename, "wb");
	if (!fp) {
		perror("fopen");
		return;
	}

	unsigned int i = 0;
	for (; i < width*height; ++i) {
		rgb.r = *r++;
		rgb.g = *g++;
		rgb.b = *b++;
		fwrite((void*)&rgb, sizeof(rgb), 1, fp);
	}

	fclose(fp);
}

int Convert(char *file,int width,int height, char *outfilename)
{
	int temp= 0;
	int x= 0;
	int y= 0;
	int fReadSize= 0;
	int ImgSize= width*height;
	FILE*fp= NULL;
	unsigned char* yuv= NULL;
	unsigned char* rgb= NULL;
	unsigned char* cTemp[6];

	int FrameSize= ImgSize+ (ImgSize>> 1);
	yuv= (unsigned char *)malloc(FrameSize);
	rgb= (unsigned char *)malloc(ImgSize*3);

	if((fp= fopen(file,"rb"))== NULL)
		return 0;

	fReadSize= fread(yuv,1, FrameSize, fp);
	fclose(fp);

	cTemp[0]= yuv;                       //y分量地址
	cTemp[1]= yuv+ ImgSize;           //u分量地址
	cTemp[2]= cTemp[1]+ (ImgSize>>2);   //v分量地址
	cTemp[3]= rgb;                       //r分量地址
	cTemp[4]= rgb+ ImgSize;           //g分量地址
	cTemp[5]= cTemp[4]+ ImgSize;       //b分量地址
	for(y=0; y< height; y++) {
		for(x=0; x< width; x++)
		{
			//r分量
			temp= cTemp[0][y*width+x]+ (cTemp[2][(y/2)*(width/2)+x/2]-128)* YuvToRgb[0][2];
			cTemp[3][y*width+x]= temp<0 ? 0 : (temp>255 ? 255 : temp);
			//g分量
			temp= cTemp[0][y*width+x]+ (cTemp[1][(y/2)*(width/2)+x/2]-128)* YuvToRgb[1][1]
			  + (cTemp[2][(y/2)*(width/2)+x/2]-128)* YuvToRgb[1][2];
			cTemp[4][y*width+x]= temp<0 ? 0 : (temp>255 ? 255 : temp);
			//b分量
			temp= cTemp[0][y*width+x]+ (cTemp[1][(y/2)*(width/2)+x/2]-128)* YuvToRgb[2][1];
			cTemp[5][y*width+x]= temp<0 ? 0 : (temp>255 ? 255 : temp);
		}
	}

	//写到BMP文件中
	WriteRaw(width, height, cTemp[3], cTemp[4], cTemp[5], outfilename);

	free(yuv);
	free(rgb);
	return 0;
}

int main(int argc, char **argv)
{
	char filename[80];
	char outfilename[80];
	if (argc != 3) {
		printf("Default input file: my.raw\n");
		printf("Default output file: out.raw\n");
		strcpy(filename, "my.raw");
		strcpy(outfilename, "out.raw");
	} else {
		strncpy(filename, argv[1], sizeof(filename));
		strncpy(outfilename, argv[2], sizeof(outfilename));
	}

        Convert(filename, WIDTH, HEIGHT, outfilename);
	printf("Convert [%s] -> [%s], width: %d, height: %d\n", filename, outfilename, WIDTH, HEIGHT);
	return 0;
}
