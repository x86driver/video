#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>

#define WIDTH 720
#define HEIGHT 240
#define FRAMEBUFFER "/dev/graphics/fb0"

double YuvToRgb[3][3]= {{1,      0, 1.4022},
                        {1,   -0.3456,-0.7145},
                        {1,  1.771,      0}};

struct RGB {
	unsigned char r,g,b;
} __attribute__((packed));

unsigned short *PICTURE_BUFFER;
unsigned char *RGB888_BUFFER;
unsigned char *RESIZE_BUFFER;
unsigned char* yuv= NULL;
unsigned char* rgb= NULL;

void rgb888to565(unsigned char *rgb888, unsigned short *rgb565)
{
        unsigned int i = 0;
        unsigned char r,g,b;
        for (; i < 480*240; ++i) {
                r = *rgb888++;
                g = *rgb888++;
                b = *rgb888++;
                r /= 8;
                g /= 4;
                b /= 8;
                *rgb565++ = (r << 11) | (g << 5) | b;
        }
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

int Convert(int width, int height)
{
	int temp= 0;
	int x= 0;
	int y= 0;
	int ImgSize= width*height;
	unsigned char* cTemp[6];

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

	//free(yuv);
	//free(rgb);
	return 0;
}

void init_framebuffer()
{
        int fd = open(FRAMEBUFFER, O_RDWR);
        if (fd == -1) {
                perror("open");
                exit(1);
        }
        struct fb_var_screeninfo screeninfo;
        ioctl(fd, FBIOGET_VSCREENINFO, &screeninfo);
        int width = screeninfo.xres;
        int height = screeninfo.yres;

        PICTURE_BUFFER = (unsigned short *)mmap(NULL, width*height*(screeninfo.bits_per_pixel/8),
                PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

        if (PICTURE_BUFFER == MAP_FAILED) {
                perror("mmap");
                exit(1);
        }
	close(fd);
}

void init_memory()
{
        int ImgSize= WIDTH*HEIGHT;
        int FrameSize= ImgSize+ (ImgSize>> 1);
        yuv= (unsigned char *)malloc(FrameSize);
        rgb= (unsigned char *)malloc(ImgSize*3);
        RGB888_BUFFER = (unsigned char*)malloc(WIDTH*HEIGHT*3);
        RESIZE_BUFFER = (unsigned char*)malloc(480*240*3);
	if (yuv == NULL || rgb == NULL || RGB888_BUFFER == NULL || RESIZE_BUFFER == NULL)
		perror("malloc");
}

void free_memory()
{
        free(RGB888_BUFFER);
        free(RESIZE_BUFFER);
        free(yuv);
        free(rgb);
}

void init_file_read(const char *filename)
{
        FILE *fp = fopen(filename, "rb");
        if (!fp)
                perror("fopen");
        int ImgSize= WIDTH*HEIGHT;
        int FrameSize= ImgSize+ (ImgSize>> 1);
        fread(yuv,1, FrameSize, fp);
        fclose(fp);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s [input file]\n", argv[0]);
		exit(1);
	}

	init_memory();

	init_file_read(argv[1]);

	init_framebuffer();

	Convert(WIDTH, HEIGHT);

	free_memory();
	return 0;
}
