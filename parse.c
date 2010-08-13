#include <stdio.h>
#include <stdlib.h>

#define SIZE 1048576
unsigned char *data;
enum State {STATUS, COUNT} state;

void parse1(int count)
{
	int i = 0;
	int number = 0;
	while (i <= count) {
		switch (state) {
		case STATUS:
			if (data[i] == 0xff && data[i+1] == 0 && data[i+2] == 0) {
				printf("0x%X 0x%X 0x%X \033[1;32m0x%X\033[0;38m ", data[i], data[i+1], data[i+2], data[i+3]);
				fflush(NULL);
				i += 4;
				state = COUNT;
			} else {
				++i;
			}
		break;
		case COUNT:
			number = 0;
			while (i <= count) {
				if (data[i] != 0xff)
					++number;
				else
					break;
				++i;
			}
			printf("\033[1;31m%d\033[0;38m\n", number);
			fflush(NULL);
			state = STATUS;
		break;
		}

	}
}

void parse(int count)
{
	int i = 0;
	while (i <= count) {
		if (data[i] == 0xff && data[i+1] == 0 && data[i+2] == 0) {
			printf("0x%X\n", data[i+3]);
				fflush(NULL);
				i += 4;
		} else
			++i;
	}
}

int main()
{
	FILE *fp = fopen("a.txt", "r");
	if (!fp) {
		perror("fopen");
		exit(1);
	}

	int i = 0;
	data = (unsigned char*)malloc(SIZE);

	while (!feof(fp)) {
		fscanf(fp, "%x", (data+i));
//		printf("0x%x\n", data[i]);
		++i;
	}
//	printf("count: %d\n", i);
	fclose(fp);
	parse(i);
	free(data);
	return 0;
}
