#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc != 5) {
		printf("Usage: %s [bit7 ... bit4]\n", argv[0]);
		exit(1);
	}

	int bit[8] = {0};
	int i;
	for (i = 0; i < 4; ++i) {
		bit[i] = atoi(argv[i+1]);
	}

	bit[4] = bit[2] ^ bit[3];
	bit[5] = bit[1] ^ bit[3];
	bit[6] = bit[1] ^ bit[2];
	bit[7] = bit[1] ^ bit[2] ^ bit[3];

	for (i = 0; i < 8; ++i) {
		if (i == 4)
			printf(" ");
		printf("%d", bit[i]);
	}

	printf("\n");
	return 0;
}
