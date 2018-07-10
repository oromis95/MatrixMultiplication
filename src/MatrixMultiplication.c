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
#include "MatrixWriter.h"
#include "FreivaldsCheck.h"

#define M_WIDTH 1000
#define N_HEIGHT 1000
void compute(int **, int, int **, int **);
void printMatrix(int **, int, int);
int main(int argc, char* argv[]) {
	/* VARIABLE DECLARATION */
	int my_rank; /* rank of process */
	int p; /* number of processes */
	int source = 0; /* rank of sender */
	int dest; /* rank of receiver */
	int tag = 0; /* tag for messages */
	MPI_Request *requestes; /* return status for receives for master */
	//MPI_Status request; /* return status for receive in each thread */
	MPI_Status statusA; /* return status for receive in each thread */
	MPI_Status statusB; /* return status for receive in each thread */
	MPI_Request *requestesForCount; /* request status for send for master */
	MPI_Request *requestesForA; /* request status for send for master */
	MPI_Request *requestesForB; /* request status for send for master */
	MPI_Status status; /* request status for send for in each thread  */
	int *flag;
	int **matrixA;
	int *matrixInArrayA;
	int **matrixB;
	int *matrixInArrayB;
	int **matrixC;
	int *matrixInArrayC;
	int portionSize, remain;
	int *rowsStart, rowsEnd = 0, rowCount = 0;
	int *countRowsSend;
	double startTime, endTime;

	srand(time(NULL));
	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	portionSize = N_HEIGHT / (p - 1);
	remain = N_HEIGHT % (p - 1);
	countRowsSend = malloc(p * sizeof(int));
	rowsStart = malloc(p * sizeof(int));
	rowsStart[1] = 0;
	requestes = malloc(p * sizeof(MPI_Status));
	requestesForCount = malloc(p * sizeof(MPI_Request));
	requestesForA = malloc(p * sizeof(MPI_Request));
	requestesForB = malloc(p * sizeof(MPI_Request));
	if (my_rank == 0) {
		MatrixGenerator("a.csv", N_HEIGHT, M_WIDTH);
		MatrixGenerator("b.csv", N_HEIGHT, M_WIDTH);

		/* MATRIX IN ARRAY OPTIMIZATION */
		matrixInArrayA = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixA = malloc(N_HEIGHT * sizeof(int*));
		matrixInArrayB = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixB = malloc(N_HEIGHT * sizeof(int*));
		matrixInArrayC = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixC = malloc(N_HEIGHT * sizeof(int*));
		for (int y = 0; y < N_HEIGHT; y++) {
			matrixA[y] = &matrixInArrayA[y * M_WIDTH];
			matrixB[y] = &matrixInArrayB[y * M_WIDTH];
			matrixC[y] = &matrixInArrayC[y * M_WIDTH];
		}

		/* LOAD THE MATRIX */
		MatrixLoader("a.csv", N_HEIGHT, M_WIDTH, matrixA, 1);
		MatrixLoader("b.csv", N_HEIGHT, M_WIDTH, matrixB, 1);
		startTime = MPI_Wtime();
		/*COMUNICATION*/
		for (int k = 1; k < p; k++) {
			dest = k;
			countRowsSend[k] = 0;

			if (k <= remain) {
				countRowsSend[k] = portionSize + 1;
			} else {
				countRowsSend[k] = portionSize;
			}
			if (countRowsSend[k] > 0) {
				rowsEnd += countRowsSend[k];

				MPI_Isend(&countRowsSend[k], 1, MPI_INT, dest, tag,
				MPI_COMM_WORLD, &requestesForCount[k]);
				MPI_Isend(&matrixA[rowsStart[k]][0], M_WIDTH * countRowsSend[k],
				MPI_INT, dest, tag,
				MPI_COMM_WORLD, &requestesForA[k]);

				MPI_Isend(&matrixB[0][0], M_WIDTH * N_HEIGHT, MPI_INT, dest,
						tag,
						MPI_COMM_WORLD, &requestesForB[k]);
				if (k == (p - 1)) {

				} else {
					rowsStart[k + 1] = rowsStart[k] + countRowsSend[k];
				}
			}
			/*
			 * EVENTUALI CONTROLLI DI SINCRONIA
			 *
			 */
		}

		for (int k = 1; k < p; k++) {
			source = k;

			MPI_Irecv(&matrixC[rowsStart[k]][0], M_WIDTH * countRowsSend[k],
			MPI_INT, source, tag, MPI_COMM_WORLD, &requestes[k]);
		}
		/*
		 * CONTROLLI OBBLIGATORI PRIMA DELLA FINE
		 *
		 */
		flag = malloc(p * sizeof(int));
		for (int k = 1; k < p; k++) {
			flag[k] = 0;
		}
		for (int k = 1; k < p; k++) {
			MPI_Test(&requestes[k], &flag[k], MPI_STATUS_IGNORE);
			while (!flag[k]) { //se false
				for (int y = 1; y < p; y++) {
					if (!flag[y]) {
						MPI_Test(&requestes[y], &flag[y], MPI_STATUS_IGNORE);
					}
				}
				// Do other stuff
				MPI_Test(&requestes[k], &flag[k], MPI_STATUS_IGNORE);
			}

		}
		endTime = MPI_Wtime();
		printf("Time is %f ms\n", (endTime - startTime) * 1000);
		MatrixWriter("c.csv", N_HEIGHT, M_WIDTH, matrixC);
		FreivaldsCheck(matrixA, matrixB, matrixC, N_HEIGHT, M_WIDTH);
	} else {

		MPI_Recv(&rowCount, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

		matrixInArrayA = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixA = malloc(N_HEIGHT * sizeof(int*));
		matrixInArrayB = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixB = malloc(N_HEIGHT * sizeof(int*));
		matrixInArrayC = malloc(N_HEIGHT * M_WIDTH * sizeof(int));
		matrixC = malloc(N_HEIGHT * sizeof(int*));
		for (int y = 0; y < rowCount; y++) {
			matrixA[y] = &matrixInArrayA[y * M_WIDTH];
			matrixC[y] = &matrixInArrayC[y * M_WIDTH];
		}
		for (int y = 0; y < N_HEIGHT; y++) {
			matrixB[y] = &matrixInArrayB[y * M_WIDTH];
		}

		MPI_Recv(&matrixA[0][0], M_WIDTH * rowCount, MPI_INT, source, tag,
		MPI_COMM_WORLD, &statusA);
		MPI_Recv(&matrixB[0][0], M_WIDTH * N_HEIGHT, MPI_INT, source, tag,
		MPI_COMM_WORLD, &statusB);

		compute(matrixA, rowCount, matrixB, matrixC);
		dest = source;
		MPI_Send(&matrixC[0][0], M_WIDTH * rowCount, MPI_INT, dest, tag,
		MPI_COMM_WORLD);

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
	double start, end;
	start = MPI_Wtime();
	for (int a = 0; a < localHeight; a++) {
		for (int q = 0; q < M_WIDTH; q++) {
			matrixC[a][q] = 0;
			for (int u = 0; u < N_HEIGHT; u++) {
				matrixC[a][q] += matrixA[a][u] * matrixB[u][q];
			}
		}
	}
	end = MPI_Wtime();
	printf("subtime is %f ms\n", (end - start) * 1000);
}
