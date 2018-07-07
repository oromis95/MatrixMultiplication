/*
 ============================================================================
 Name        : MatrixMultiplication.c
 Author      : Domenico Antonio Tropeano
 Version     :
 Copyright   : copyright by Domenico Antonio Tropeano
 Description : Main for Matrix Multiplication
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include "MatrixGenerator.h"
#include "MatrixLoader.h"

#define M_WIDTH 3
#define N_HEIGHT 3
void compute(int **, int, int **, int **);
void printMatrix(int **, int, int);
int main(int argc, char* argv[]) {
	/* VARIABLE DECLARATION */
	int my_rank; /* rank of process */
	int p; /* number of processes */
	int source = 0; /* rank of sender */
	int dest; /* rank of receiver */
	int tag = 0; /* tag for messages */
	MPI_Status status; /* return status for receive */
	int **matrixA;
	int *matrixInArrayA;
	int **matrixB;
	int *matrixInArrayB;
	int **matrixC;
	int *matrixInArrayC;
	int portionSize, remain;
	int rowsStart = 0, rowsEnd = 0, countRowsSend = 0;
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
		MatrixLoader("b.csv", N_HEIGHT, M_WIDTH, matrixB, 1);

		for (int k = 1; k < p; k++) {
			dest = k;
			countRowsSend = 0;

			if (k <= remain) {
				countRowsSend = portionSize + 1;
			} else {
				countRowsSend = portionSize;
			}
			rowsEnd += countRowsSend;

			MPI_Send(&countRowsSend, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			//remove this shitty for and use 1d array
			for (int o = rowsStart; o < rowsEnd; o++) {
				MPI_Send(&matrixA[o][0], M_WIDTH, MPI_INT, dest, tag,
				MPI_COMM_WORLD);
			}

			rowsStart += countRowsSend;
			MPI_Send(&matrixB[0][0], M_WIDTH * N_HEIGHT, MPI_INT, dest, tag,
			MPI_COMM_WORLD);
		}
	} else {

		MPI_Recv(&countRowsSend, 1, MPI_INT, source, tag, MPI_COMM_WORLD,
				&status);

		matrixInArrayA = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixA = malloc(N_HEIGHT * sizeof(int*));
		matrixInArrayB = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixB = malloc(N_HEIGHT * sizeof(int*));
		matrixInArrayC = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixC = malloc(N_HEIGHT * sizeof(int*));
		for (int y = 0; y < countRowsSend; y++) {
			matrixA[y] = &matrixInArrayA[y * M_WIDTH];
			matrixC[y] = &matrixInArrayC[y * M_WIDTH];
		}
		for (int y = 0; y < N_HEIGHT; y++) {
			matrixB[y] = &matrixInArrayB[y * M_WIDTH];
		}
		for (int o = 0; o < countRowsSend; o++) {

			MPI_Recv(&matrixA[o][0], M_WIDTH, MPI_INT, source, tag,
			MPI_COMM_WORLD, &status);

		}

		MPI_Recv(&matrixB[0][0], M_WIDTH * N_HEIGHT, MPI_INT, source, tag,
		MPI_COMM_WORLD, &status);
		printMatrix(matrixA,countRowsSend,M_WIDTH);
		printMatrix(matrixB,N_HEIGHT,M_WIDTH);
		compute(matrixA,countRowsSend,matrixB,matrixC);
		printMatrix(matrixC,countRowsSend,M_WIDTH);
	}

	//must use mpi_free
	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
void printMatrix(int **matrix, int mRow, int mColumn) {
	for (int row = 0; row < mRow; row++) {
		for (int columns = 0; columns < mColumn; columns++) {
			printf("%d  ", matrix[row][columns]);
		}
		printf("\n");
	}
	printf("\n\n\n");
}

void compute(int **matrixA, int localHeight, int **matrixB, int **matrixC) {
	for (int a = 0; a < localHeight; a++) {
		for(int q=0;q<M_WIDTH;q++){
			matrixC[a][q]=0;
			for(int u=0;u<N_HEIGHT;u++){
				matrixC[a][q]+=matrixA[a][u]*matrixB[u][q];
			}
		}
	}
}
