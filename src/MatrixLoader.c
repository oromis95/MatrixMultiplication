/*
 * MatrixLoader.c
 *
 *  Created on: 31 mag 2018
 *      Author: oromis
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MatrixLoader.h"
void MatrixLoader(char* fileName, int n, int m, int **matrix, int mod) {
	FILE *f;
	int i = 0, j = 0;
	ssize_t read;
	char * line = NULL;
	size_t len = 0;

	if (mod == 1) {
		f = fopen(fileName, "r");
	}
	while ((read = getline(&line, &len, f)) != -1) {
		strtok(line, ";");
		while (line) {
			matrix[i][j++] = atoi(line);
			line = strtok(NULL, ";");
		}
		++i;
		j = 0;

	}
}
