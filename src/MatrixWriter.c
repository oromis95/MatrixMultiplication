/*
 * MatrixLoader.c
 *
 *  Created on: 09 luglio 2018
 *      Author: oromis
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MatrixWriter.h"
void MatrixWriter(char* fileName, int n, int m, int **matrix) {
	FILE *f = fopen(fileName, "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	for (int i = 0; i < n; i++) {
		for (int k = 0; k < m; k++) {
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
}
