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

int main()
{
	int fd = open("/dev/video0", O_RDONLY);
	if (fd == -1)
		perror("open");

	struct v4l2_format fmt;
	memset((void*)&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret == -1) {
		perror("ioctl");
	}

	char *buf = (char*)&(fmt.fmt.pix.pixelformat);
	printf("%c%c%c%c", buf[0], buf[1], buf[2], buf[3]);

	close(fd);
	return 0;
}
