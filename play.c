#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>
#include <linux/fb.h>

struct buffer {
  void *                  start;
  size_t                  length;
};

struct buffer *buffers = NULL;
int fd;

#define WIDTH 720
#define HEIGHT 240
#define BUFFER_COUNT 4

/* from capture */
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
//        int FrameSize= ImgSize+ (ImgSize>> 1);
//        yuv= (unsigned char *)malloc(FrameSize);
        rgb= (unsigned char *)malloc(ImgSize*3);
        RGB888_BUFFER = (unsigned char*)malloc(WIDTH*HEIGHT*3);
        RESIZE_BUFFER = (unsigned char*)malloc(480*240*3);
//	if (yuv == NULL || rgb == NULL || RGB888_BUFFER == NULL || RESIZE_BUFFER == NULL)
//		perror("malloc");
}

void free_memory()
{
        free(RGB888_BUFFER);
        free(RESIZE_BUFFER);
//        free(yuv);
        free(rgb);
}

/*
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
*/

void play()
{
	Convert(WIDTH, HEIGHT);
}
/* end capture */

void uninit()
{
	unsigned int i;
        for (i = 0; i < BUFFER_COUNT; ++i) {
                if (-1 == munmap (buffers[i].start, buffers[i].length))
                        perror ("munmap");
        }
	free(buffers);
}

void stream_on()
{
	enum v4l2_buf_type type;
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == ioctl (fd, VIDIOC_STREAMON, &type))
          perror ("VIDIOC_STREAMON");
}

void stream_off()
{
	enum v4l2_buf_type type;
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == ioctl (fd, VIDIOC_STREAMOFF, &type))
          perror ("VIDIOC_STREAMOFF");
}

static void
process_image                   (const void *           p, int index)
{
/*
        char filename[80];
	static int outindex = 0;
        snprintf(filename, sizeof(filename), "my%d.raw", outindex++);
        int outfd = open(filename, O_CREAT|O_WRONLY|O_TRUNC, 00644);
        if (outfd == -1)
                perror("open");
        printf("\nSize: %d\n", buffers[index].length);
        write(outfd, buffers[index].start, buffers[index].length);
        printf("Output file: %s\n", filename);
        close(outfd);
*/
	yuv = buffers[index].start;
	play();
}

int read_frame()
{
        struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(buf));

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	printf("before DQBUF\n");
	if (-1 == ioctl (fd, VIDIOC_DQBUF, &buf)) {
		printf("DQBUF error!!\n");
		switch (errno) {
		case EAGAIN:
		  printf("DQBUF EAGAIN\n");
		  return 0;

		case EIO:
		  printf("DQBUF EIO\n");
		  /* Could ignore EIO, see spec. */

		  /* fall through */

		default:
		  perror ("VIDIOC_DQBUF");
		}
	}
	printf("DQBUF ok! I get [%d] buffer\n", buf.index);

	process_image (buffers[buf.index].start, buf.index);

	printf("before QBUF\n");
	if (-1 == ioctl (fd, VIDIOC_QBUF, &buf))
	  perror ("VIDIOC_QBUF");
	printf("QBUF ok!\n");


        return 1;

}

void request_buffer()
{
        struct v4l2_requestbuffers req;
        struct v4l2_format fmt;

        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = WIDTH;
        fmt.fmt.pix.height      = HEIGHT;
//        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

        if (-1 == ioctl (fd, VIDIOC_S_FMT, &fmt))
          perror ("VIDIOC_S_FMT");

        memset(&req, 0, sizeof(req));
        req.count               = BUFFER_COUNT;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_MMAP;

        if (-1 == ioctl (fd, VIDIOC_REQBUFS, &req)) {
                perror("reqbufs");
        }

        printf("req.count: %d\n", req.count);
        buffers = calloc(req.count, sizeof(*buffers));
        if (!buffers) {
                perror("calloc");
                exit(1);
        }
}

void query_buffer()
{
	unsigned int n_buffers;

        for (n_buffers = 0; n_buffers < BUFFER_COUNT; ++n_buffers) {
                struct v4l2_buffer buf;

                memset(&buf, 0, sizeof(buf));

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if (-1 == ioctl (fd, VIDIOC_QUERYBUF, &buf))
                  perror ("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                  mmap (NULL /* start anywhere */,
                        buf.length,
                        PROT_READ | PROT_WRITE /* required */,
                        MAP_SHARED /* recommended */,
                        fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                  perror ("mmap");
        }
}

void queue_buffer()
{
	unsigned int i;
        for (i = 0; i < BUFFER_COUNT; ++i) {
                struct v4l2_buffer buf;

                memset(&buf, 0, sizeof(buf));

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = i;

                printf("VIDIOC_QBUF: %d\n", i);
                if (-1 == ioctl (fd, VIDIOC_QBUF, &buf))
                  perror ("VIDIOC_QBUF");
        }
}

void open_device()
{
        fd = open("/dev/video0", O_RDWR);
        if (fd == -1) {
                perror("open");
                exit(1);
        }
}

int main()
{
        init_memory();

        init_framebuffer();

	open_device();

	request_buffer();

	query_buffer();

	queue_buffer();

	stream_on();

	while (1)
		read_frame();

	stream_off();

	uninit();

	close(fd);

	free_memory();
	return 0;
}

