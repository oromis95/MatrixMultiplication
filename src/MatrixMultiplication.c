/*
 ============================================================================
 Name        : MatrixMultiplication.c
 Author      : Domenico
 Version     :
 Copyright   : copyright by Domenico
 Description : Main for Matrix Multiplication
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include "MatrixGenerator.h"
#include "MatrixLoader.h"

#define M_LENGHT 12
#define N_HEIGHT 12
int main(int argc, char* argv[]) {
	/* VARIABLE DECLARATION */
	int my_rank; /* rank of process */
	int p; /* number of processes */
	int source; /* rank of sender */
	int dest; /* rank of receiver */
	int tag = 0; /* tag for messages */
	char message[100]; /* storage for message */
	MPI_Status status; /* return status for receive */
	int **matrix;
	int *matrixInArray;
	int portionSize, remain;
	int row;
	MatrixGenerator("a.csv", N_HEIGHT, M_LENGHT);
	MatrixGenerator("b.csv", N_HEIGHT, M_LENGHT);

	/* MATRIX IN ARRAY OPTIMIZATION */
	matrixInArray = malloc(N_HEIGHT * M_LENGHT * sizeof(int));
	matrix = malloc(M_LENGHT * sizeof(int*));
	for (int p = 0; p < M_LENGHT; p++) {
		matrix[p] = &matrixInArray[p * N_HEIGHT];
	}

	/* LOAD THE MATRIX */
	MatrixLoader("a.csv", N_HEIGHT, M_LENGHT, matrix, 1);
	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int sizes[2] = { N_HEIGHT, M_LENGHT };

	portionSize = N_HEIGHT / (p - 1);
	remain = N_HEIGHT % (p - 1);

	for (int k = 1; k < p; k++) {
		dest = k;
		if (k <= remain) {
			int addedSize = portionSize + 1;
			/*send this amount of rows two possibilies:
			 * - increment pointer
			 * - use start index & end
			 *
			 * Check slides for best mode to pack & send data
			 * */
		} else {

		}
	}

	//must use mpi_free
	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
