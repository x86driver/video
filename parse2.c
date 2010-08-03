#include <stdio.h>
#include <stdlib.h>

#define SIZE 256
unsigned char *data;
unsigned char state_table[] = {0x9d, 0xda, 0xf1, 0xb6};
unsigned char lookup_table[] = {0x80, 0xc7, 0xec, 0xab};
int count = 0;

void lookup(int state, int *n)
{
	int i = *n;
	int number = 0;
	while (i < count) {
		if (data[i+1] != lookup_table[state]) {
			printf("It is 0x%X, next must be 0x%X, but next is: 0x%X\n", data[i], lookup_table[state], data[i+1]);
			++i;
		} else {
//			printf("0x%X 268 0x%X 1440\n", data[i], data[i+1]);
			i += 2;
			++number;
		}
		if (data[i] != state_table[state]) {
			printf("\033[1;33m0x%X\033[0;38m 268 \033[1;34m0x%X\033[0;38m 1440\n", state_table[state], lookup_table[state]);
			printf("====== %d times ======\n", number);
			break;
		}
	}
	*n = i;
}

void parse()
{
	int i = 0;
	while (i <= count) {
		switch (data[i]) {
		case 0x9d:
			lookup(0, &i);
			break;
		case 0xda:
			lookup(1, &i);
			break;
		case 0xf1:
			lookup(2, &i);
			break;
		case 0xb6:
			lookup(3, &i);
			break;
		case 0x80:
		case 0xc7:
		case 0xec:
		case 0xab:
			printf("Found 0x%X...\n", data[i]);
		default:
			++i;
			break;
		}
	}
}

int main()
{
	FILE *fp = fopen("b.txt", "r");
	if (!fp) {
		perror("fopen");
		exit(1);
	}

	data = (unsigned char*)malloc(SIZE);

	fscanf(fp, "%x", (data+count));
	while (!feof(fp)) {
		++count;
		fscanf(fp, "%x", (data+count));
	}

	--count;
	parse();

	free(data);
	fclose(fp);
	return 0;
}
