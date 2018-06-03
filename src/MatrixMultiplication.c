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
	MPI_Datatype newtype, resizedType;

	MatrixGenerator("a.csv", N_HEIGHT, M_LENGHT);
	MatrixGenerator("b.csv", N_HEIGHT, M_LENGHT);

	/* MATRIX IN ARRAY OPTIMIZATION */
	matrixInArray = malloc(N_HEIGHT * M_LENGHT * sizeof(int));
	matrix = malloc(M_LENGHT * sizeof(int*));
	for (int p = 0; p < M_LENGHT; p++) {
		matrix[p] = &matrixInArray[p * N_HEIGHT];
	}

	MatrixLoader("a.csv", N_HEIGHT, M_LENGHT, matrix, 1);
	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int sizes[2] = { N_HEIGHT, M_LENGHT };
	if (N_HEIGHT % 4 != 0 || M_LENGHT % 4 != 0) {
		printf("Error matrix size different not divisible by 4!\n");
		exit(1);
	}
	int subsizes[2] = { N_HEIGHT / p, M_LENGHT / p };
	int starts[2] = { 0, 0 };
	int counts[4] = {1,1,1,1};   /* how many pieces of data everyone has, in units of blocks */
	int displs[4] = {0,1,6,7};   /* the starting point of everyone's data */
	                             /* in the global array, in block extents */

	if (my_rank == 0) {
		MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C,
		MPI_INT, &newtype);
		MPI_Type_create_resized(newtype, 0, subsizes[0] * sizeof(int),
				&resizedType); //0 indica che non ci sono holes

		MPI_Type_commit(&newtype);
		for (int u=0;u<p;u++){

		}
	} else {

	}
	//must use mpi_free
	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
