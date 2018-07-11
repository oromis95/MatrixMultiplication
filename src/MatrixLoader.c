/*
 * MatrixLoader.c
 *
 *  Created on: 31 mag 2018
 *      Author: oromis
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "MatrixLoader.h"
/**
 * Loading of Matrix
 */
void MatrixLoader(char* fileName, int n, int m, int **matrix, int mod) {
	FILE *f;
	int i = 0, j = 0;
	ssize_t read;
	char * line = NULL;
	size_t len = 0;
	char folder[50]="./data/";

	if (mod == 1) {
		f = fopen(strcat(folder,fileName), "r");
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
