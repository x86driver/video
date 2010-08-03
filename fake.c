#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *fp = fopen("fake.raw", "wb");
	if (!fp) {
		perror("fopen");
		exit(1);
	}

	int x = 480, y = 272;
	unsigned int i = 0;
	unsigned char data[] = {0x10, 0x80};

	for (i = 0; i < x*y; ++i) {
		fwrite((void*)data, sizeof(data), 1, fp);
	}
	fclose(fp);
	return 0;
}
