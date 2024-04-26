#include "Error.h"

void eThrowError(char* error) {
	printf("ERROR: %s\n", error);
	exit(EXIT_FAILURE);
}