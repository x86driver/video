#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	int fd = open("my.raw", O_RDONLY);
	if (fd == -1) {
		perror("open");
		exit(1);
	}

	unsigned char *ptr = mmap(NULL, 262144, PROT_READ, MAP_SHARED, fd, 0);
	if ((char*)ptr == -1) {
		perror("mmap");
		exit(1);
	}
	unsigned char *buf = ptr+0x020;
	unsigned int i = 0;

	for (i = 0; i < 262144; ++i) {
		if ((*(buf+1) - *buf) == 3) {
			printf("Error on %d (0x%x), [%d] ", buf-ptr, buf-ptr, i);
			printf("0x%x, 0x%x\n", *(buf+1), *buf);
		}
		++buf;
	}

	munmap(ptr, 262144);
	close(fd);
	return 0;
}

