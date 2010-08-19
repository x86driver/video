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

unsigned short *PICTURE_BUFFER;
unsigned char *RGB888_BUFFER;
unsigned char *RESIZE_BUFFER;

void rgb888to565(unsigned char *rgb888, unsigned short *rgb565)
{
        unsigned int i = 0;
        unsigned char r,g,b;
	unsigned short *picture = rgb565;
        for (; i < 480*240; ++i) {
                r = *rgb888++;
                g = *rgb888++;
                b = *rgb888++;
                r /= 8;
                g /= 4;
                b /= 8;
                *rgb565++ = (r << 11) | (g << 5) | b;
        }
	FILE *fp = fopen("out565.raw", "wb");
	if (!fp) {
		perror("fopen");
	}
	fwrite(picture, 480*240*2, 1, fp);
	printf("Output file: out565.raw\n");
	fclose(fp);
}

void resize(unsigned char *inbuf, unsigned char *outbuf)
{
        unsigned int i = 0;
	unsigned char *resize = outbuf;
        for (; i < 240*240; ++i) {
                *outbuf++ = *inbuf++;
                *outbuf++ = *inbuf++;
                *outbuf++ = *inbuf++;

                *outbuf++ = *inbuf++;
                *outbuf++ = *inbuf++;
                *outbuf++ = *inbuf++;

                inbuf += 3;
        }
	rgb888to565(resize, PICTURE_BUFFER);
}

void YUV2RGB(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *outbuf)
{
	unsigned int i = 0;
	unsigned char *rgb888 = outbuf;
	for (; i < WIDTH*HEIGHT; ++i) {
		*outbuf++ = *r++;
		*outbuf++ = *g++;
		*outbuf++ = *b++;
	}
	resize(rgb888, RESIZE_BUFFER);
}

int Convert(char *file,int width,int height)
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
	YUV2RGB(cTemp[3], cTemp[4], cTemp[5], RGB888_BUFFER);

	free(yuv);
	free(rgb);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s [input file]\n", argv[0]);
		exit(1);
	}

	RGB888_BUFFER = (unsigned char*)malloc(WIDTH*HEIGHT*3);
	RESIZE_BUFFER = (unsigned char*)malloc(480*240*3);
	PICTURE_BUFFER = (unsigned short*)malloc(480*240*2);

	Convert(argv[1], WIDTH, HEIGHT);

	free(RGB888_BUFFER);
	free(RESIZE_BUFFER);
	free(PICTURE_BUFFER);
	return 0;
}
