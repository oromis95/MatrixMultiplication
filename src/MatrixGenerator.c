/*
 * MatrixGenerator.c
 *
 *  Created on: 30 mag 2018
 *      Author: oromis
 */
#include <stdio.h>
#include <stdlib.h>
#include "MatrixGenerator.h"
int MatrixGenerator(char* name, int nSize, int mSize) {

	int n, m;
	n = nSize;
	m = mSize;
	FILE *f = fopen(name, "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	int matrix[n][m];
	for (int i = 0; i < n; i++) {
		for (int k = 0; k < m; k++) {
			matrix[i][k] = rand() % 100;
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

