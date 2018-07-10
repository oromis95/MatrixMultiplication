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
#include <time.h>
#include "mpi.h"
#include "MatrixGenerator.h"
#include "MatrixLoader.h"
#include "MatrixWriter.h"
#include "FreivaldsCheck.h"

#define M_WIDTH 800
#define N_HEIGHT 800
void compute(int **, int, int **, int **, int);
void printMatrix(int **, int, int);
int main(int argc, char* argv[]) {
	/* VARIABLE DECLARATION */
	int my_rank; /* rank of process */
	int p; /* number of processes */
	int source = 0; /* rank of sender */
	int dest; /* rank of receiver */
	int tag = 0; /* tag for messages */
	MPI_Request *requestes; /* requestes for receives for master */
	MPI_Status statusA; /* return status for receive in each thread for A matrix */
	MPI_Status statusB; /* return status for receive in each thread for B matrix */
	MPI_Request *requestesForCount; /* request status for send for master count */
	MPI_Request *requestesForA; /* request status for send for master A*/
	MPI_Request *requestesForB; /* request status for send for master B*/
	MPI_Status status; /* request status for send for in each thread  */
	int *flagSync, flagTest;
	int width, height;
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
	if (argc < 2) {
		width = M_WIDTH;
		height = N_HEIGHT;
	} else if (argc == 2) {
		if (strcmp(argv[1], "test")) {
			flagTest = 1;
		} else {
			flagTest = 2;
		}
	} else if (argc == 3) {
		height = atoi(argv[1]);
		width = atoi(argv[2]);
	} else if (argc == 4) {
		height = atoi(argv[1]);
		width = atoi(argv[2]);
		if (strcmp(argv[3], "test")) {
			flagTest = 1;
		} else {
			flagTest = 2;
		}
	} else {
		printf("ERROR");
		return -1;
	}
	srand(time(NULL));
	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	/*PortionSize and remanin calculation*/
	portionSize = height / (p);
	remain = height % (p);

	/*Dynamic Allocation of array 1d*/
	countRowsSend = malloc(p * sizeof(int));
	rowsStart = malloc(p * sizeof(int));
	rowsStart[0] = 0;
	requestes = malloc(p * sizeof(MPI_Status));
	requestesForCount = malloc(p * sizeof(MPI_Request));
	requestesForA = malloc(p * sizeof(MPI_Request));
	requestesForB = malloc(p * sizeof(MPI_Request));

	if (my_rank == 0) {
		/*MATRIX GENERATION*/
		MatrixGenerator("a.csv", height, width);
		MatrixGenerator("b.csv", height, width);

		/* MATRIX IN ARRAY OPTIMIZATION */
		matrixInArrayA = malloc(height * width * sizeof(int));
		matrixA = malloc(height * sizeof(int*));
		matrixInArrayB = malloc(height * width * sizeof(int));
		matrixB = malloc(height * sizeof(int*));
		matrixInArrayC = malloc(height * width * sizeof(int));
		matrixC = malloc(height * sizeof(int*));
		for (int y = 0; y < height; y++) {
			matrixA[y] = &matrixInArrayA[y * width];
			matrixB[y] = &matrixInArrayB[y * width];
			matrixC[y] = &matrixInArrayC[y * width];
		}

		/* LOAD THE MATRIX */
		MatrixLoader("a.csv", height, width, matrixA, 1);
		MatrixLoader("b.csv", height, width, matrixB, 1);
		startTime = MPI_Wtime();

		/*COMUNICATION*/
		for (int k = 1; k < p; k++) { //Qui far lavorare il master
			dest = k;
			countRowsSend[k] = 0;

			if (k <= remain) {
				countRowsSend[k] = portionSize + 1;
			} else {
				countRowsSend[k] = portionSize;
			}
			rowsStart[k] = rowsStart[k - 1] + countRowsSend[k];
			if (countRowsSend[k] > 0) {
				rowsEnd += countRowsSend[k];

				MPI_Isend(&countRowsSend[k], 1, MPI_INT, dest, tag,
				MPI_COMM_WORLD, &requestesForCount[k]);
				MPI_Isend(&matrixA[rowsStart[k]][0], width * countRowsSend[k],
				MPI_INT, dest, tag,
				MPI_COMM_WORLD, &requestesForA[k]);

				MPI_Isend(&matrixB[0][0], width * height, MPI_INT, dest, tag,
				MPI_COMM_WORLD, &requestesForB[k]);

			}
		}
		if (remain > 0) {
			compute(matrixA, (portionSize + 1), matrixB, matrixC, width);
		}else{
			compute(matrixA, portionSize , matrixB, matrixC, width);
		}

		for (int k = 1; k < p; k++) {
			source = k;

			MPI_Irecv(&matrixC[rowsStart[k]][0], width * countRowsSend[k],
			MPI_INT, source, tag, MPI_COMM_WORLD, &requestes[k]);
		}
		/*
		 * SYNCHRONIZATION with optimization based on array
		 *
		 */
		flagSync = malloc(p * sizeof(int));
		for (int k = 1; k < p; k++) {
			flagSync[k] = 0;
		}
		for (int k = 1; k < p; k++) {
			MPI_Test(&requestes[k], &flagSync[k], MPI_STATUS_IGNORE);
			while (!flagSync[k]) { //se false
				for (int y = 1; y < p; y++) {
					if (!flagSync[y]) {
						MPI_Test(&requestes[y], &flagSync[y],
						MPI_STATUS_IGNORE);
					}
				}
				// Do other stuff
				MPI_Test(&requestes[k], &flagSync[k], MPI_STATUS_IGNORE);
			}

		}
		endTime = MPI_Wtime();
		printf("Time is %f ms\n", (endTime - startTime) * 1000);
		/*SAVING OF MATRIX C*/
		MatrixWriter("c.csv", height, width, matrixC);
		/*CORRECTNESS CHECKING*/
		if (flagTest) {
			FreivaldsCheck(matrixA, matrixB, matrixC, height, width);
		}
	} else {
		/*RECEIVE NUMBER OF ROWS FOR MATRIX A*/
		MPI_Recv(&rowCount, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
		/*ARRAY 2D OPTIMIZATION*/
		matrixInArrayA = malloc(height * width * sizeof(int));
		matrixA = malloc(height * sizeof(int*));
		matrixInArrayB = malloc(height * width * sizeof(int));
		matrixB = malloc(height * sizeof(int*));
		matrixInArrayC = malloc(height * width * sizeof(int));
		matrixC = malloc(height * sizeof(int*));
		for (int y = 0; y < rowCount; y++) {
			matrixA[y] = &matrixInArrayA[y * width];
			matrixC[y] = &matrixInArrayC[y * width];
		}
		for (int y = 0; y < height; y++) {
			matrixB[y] = &matrixInArrayB[y * width];
		}

		/*RECEIVE OF MATRIX A*/
		MPI_Recv(&matrixA[0][0], width * rowCount, MPI_INT, source, tag,
		MPI_COMM_WORLD, &statusA);
		/*RECEIVE OF MATRIX B*/
		MPI_Recv(&matrixB[0][0], width * height, MPI_INT, source, tag,
		MPI_COMM_WORLD, &statusB);
		/*COMPUTATION*/
		compute(matrixA, rowCount, matrixB, matrixC, width);
		dest = source;
		/*SENDING OF RESULT*/
		MPI_Send(&matrixC[0][0], width * rowCount, MPI_INT, dest, tag,
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

void compute(int **matrixA, int localHeight, int **matrixB, int **matrixC,
		int width) {
	double start, end;
	start = MPI_Wtime();
	for (int a = 0; a < localHeight; a++) {
		for (int q = 0; q < width; q++) {
			matrixC[a][q] = 0;
			for (int u = 0; u < width; u++) {
				matrixC[a][q] += matrixA[a][u] * matrixB[u][q];
			}
		}
	}
	end = MPI_Wtime();
	printf("subtime is %f ms\n", (end - start) * 1000);
}
