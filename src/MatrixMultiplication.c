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

#define M_WIDTH 12
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
	int **matrixA;
	int *matrixInArrayA;
	int **matrixB;
	int *matrixInArrayB;
	int portionSize, remain;
	int rowCount = 0, countRowsSend = 0;
	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	portionSize = N_HEIGHT / (p - 1);
	remain = N_HEIGHT % (p - 1);
	if (my_rank == 0) {
		MatrixGenerator("a.csv", N_HEIGHT, M_WIDTH);
		MatrixGenerator("b.csv", N_HEIGHT, M_WIDTH);

		/* MATRIX IN ARRAY OPTIMIZATION */
		matrixInArrayA = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixA = malloc(N_HEIGHT * sizeof(int*));
		matrixInArrayB = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixB = malloc(N_HEIGHT * sizeof(int*));
		for (int y = 0; y < N_HEIGHT; y++) {
			matrixA[y] = &matrixInArrayA[y * M_WIDTH];
			matrixB[y] = &matrixInArrayB[y * M_WIDTH];
		}

		/* LOAD THE MATRIX */
		MatrixLoader("a.csv", N_HEIGHT, M_WIDTH, matrixA, 1);

		for (int k = 1; k < p; k++) {
			dest = k;
			countRowsSend = 0;
			if (k != 1) {
				if (k <= remain) {
					countRowsSend = portionSize + 1;

				} else {
					countRowsSend = portionSize;
				}
			}
			rowCount += countRowsSend;
			MPI_Send(&countRowsSend, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			for (int o = 0; 0 < countRowsSend; 0++) {
				MPI_Send(&matrixA[o][0], M_WIDTH, dest, tag, MPI_COMM_WORLD);
			}
		}
	} else {
		MPI_Recv(countRowsSend, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
		matrixInArrayA = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixA = malloc(N_HEIGHT * sizeof(int*));
		for (int y = 0; y < countRowsSend; y++) {
			matrixA[y] = &matrixInArrayA[y * M_WIDTH];
		}
		for (int o = 0; 0 < countRowsSend; 0++) {
			MPI_Recv(&matrixA[o][0], M_WIDTH, source, tag, MPI_COMM_WORLD);
		}
	}

//must use mpi_free
	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
