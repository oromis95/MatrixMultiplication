/*
 * FreivaldsCheck.c
 *
 *  Created on: 30 mag 2018
 *      Author: oromis
 */
#include <stdio.h>
#include <stdlib.h>
#define MAX_COUNT 1000
void FreivaldsCheck(int **matrixA, int **matrixB, int ** matrixC, int height,
		int width) {
	char nl;
	int *randomVector;
	randomVector = malloc(height * sizeof(int));
	int counter = 0;
	while (counter < MAX_COUNT && scanf("%c", &nl) != EOF) {
		if (nl == '\n') {
			break;
		}
		for (int i = 0; i < height; i++) {
			randomVector[i] = rand() % 2;
		}

	}

}
void matrXvec(){

}


