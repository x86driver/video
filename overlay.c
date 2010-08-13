#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct {
	unsigned int sx, sy, width, height, format;
} overlay_config_t;

int main()
{
	int fd = open("/dev/overlay", O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(1);
	}

	overlay_config_t fmt;
	memset((void*)&fmt, 0, sizeof(fmt));
	fmt.width = 480;
	fmt.height = 272;

	int ret = ioctl(fd, 4, &fmt);
	if (ret == -1) {
		perror("ioctl");
	}

	unsigned char *buf = malloc(480*272*2);
	if ((int)buf == -1)
		perror("malloc");

	memset(buf, 0x55, 480*272*2);

	ret = ioctl(fd, 3, buf);

	free(buf);
	close(fd);

	return 0;
}
