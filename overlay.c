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

	unsigned char *buf = (unsigned char*)mmap(NULL, 480*272*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if ((int)buf == -1) {
		perror("mmap");
	}

	unsigned int i;
	unsigned char *ptr = buf;
	for (i = 0; i < 480*272; ++i) {
		*ptr++ = 0x55;
		*ptr++ = 0x55;
	}

	munmap(buf, 480*272*2);
	close(fd);

	return 0;
}
