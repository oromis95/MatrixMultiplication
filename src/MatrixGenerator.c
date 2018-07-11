/*
 * MatrixGenerator.c
 *
 *  Created on: 30 mag 2018
 *      Author: oromis
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MatrixGenerator.h"
/**
 * Generation of Matrix
 */
int MatrixGenerator(char* name, int nSize, int mSize) {

	int n, m;
	n = nSize;
	m = mSize;
	char folder[50]="./data/";
	FILE *f = fopen( strcat(folder,name), "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	int matrix[n][m];
	for (int i = 0; i < n; i++) {
		for (int k = 0; k < m; k++) {
			matrix[i][k] = rand() % 10;
			if (k != 0) {
				fprintf(f, ";");
			}
			fprintf(f, "%d", matrix[i][k]);
		}
		if (i != n - 1) {
			fprintf(f, "\n");
		}
	}

	fclose(f);
	return 0;
}

