/*
 * FreivaldsCheck.c
 *
 *  Created on: 30 mag 2018
 *      Author: oromis
 */
#include <stdio.h>
#include <stdlib.h>
#include "FreivaldsCheck.h"
#define MAX_COUNT 50
int checkVector(int *, int);
int *matrXvec(int **, int*, int, int);
int *vectDiff(int *, int *, int, int);
void printfVector(int*, int);
void FreivaldsCheck(int **matrixA, int **matrixB, int ** matrixC, int height,
		int width, int testCase) {

	int flag = 1;
	int *randomVector, *br, *cr, *p1, *finalP;
	randomVector = malloc(height * sizeof(int));
	int counter, max = testCase;
	if (max < 1 || max > 50) {
		max = MAX_COUNT;
	}
	while (counter < max) {
		for (int i = 0; i < height; i++) {
			randomVector[i] = rand() % 2;
		}

		br = matrXvec(matrixB, randomVector, height, width);
		cr = matrXvec(matrixC, randomVector, height, width);
		p1 = matrXvec(matrixA, br, height, width);
		finalP = vectDiff(p1, cr, height, width);
		if (!checkVector(finalP, height)) {
			flag = 0;
		}
		counter++;
	}
	if (flag) {
		printf(
				"Correttezza assicurata con algoritmo di Freivalds con probabilità d'errore minore di 1/2^%d\n",
				max);
	} else {
		printf("Moltiplicazione errata\n");
	}

}
int checkVector(int *vector, int lenght) {
	int flag = 1;
	for (int i = 0; i < lenght; i++) {
		if (vector[i] != 0) {
			flag = 0;
		}
	}
	return flag;
}
int* matrXvec(int **matrixA, int* vectorB, int height, int width) {
	int *vectorResult = malloc(height * sizeof(int));

	for (int i = 0; i < height; i++) {
		vectorResult[i] = 0;
		for (int j = 0; j < width; j++) {
			vectorResult[i] += matrixA[i][j] * vectorB[j];
		}

	}
	return vectorResult;
}
void printfVector(int* vector, int lenght) {
	for (int i = 0; i < lenght; i++) {
		printf("%d ", vector[i]);
	}
	printf("\n");
}
int *vectDiff(int *vectorA, int *vectorB, int height, int width) {
	int *vectorC = malloc(height * sizeof(int));

	for (int i = 0; i < height; i++) {

		vectorC[i] = vectorA[i] - vectorB[i];

	}
	return vectorC;
}

