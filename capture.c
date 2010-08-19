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

struct buffer {
  void *                  start;
  size_t                  length;
};

struct buffer *buffers = NULL;
int fd;

#define WIDTH 720
#define HEIGHT 240
#define BUFFER_COUNT 4

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
	open_device();

	request_buffer();

	query_buffer();

	queue_buffer();

	stream_on();

	read_frame();

	stream_off();

	uninit();

	close(fd);

	return 0;
}

