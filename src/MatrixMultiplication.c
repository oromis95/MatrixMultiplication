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

#define M_LENGHT 10
#define N_HEIGHT 10
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
	MatrixGenerator("a.csv", 10, 10);
	MatrixGenerator("b.csv", 10, 10);

	/* MATRIX IN ARRAY OPTIMIZATION */
	matrixInArray = malloc(N_HEIGHT * M_LENGHT * sizeof(int));
	matrix = malloc(M_LENGHT * sizeof(int*));
	for (int p = 0; p < M_LENGHT; p++) {
		matrix[p] = &matrixInArray[p * N_HEIGHT];
	}

	MatrixLoader("a.csv", 10, 10, matrix, 1);
	for (int i = 0; i < 10; i++) {
		for (int k = 0; k < 10; k++) {
			printf("%d ", matrix[i][k]);
		}
		printf("\n");
	}
//	/* start up MPI */
//
//	MPI_Init(&argc, &argv);
//
//	/* find out process rank */
//	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
//
//	/* find out number of processes */
//	MPI_Comm_size(MPI_COMM_WORLD, &p);
//
//
//	if (my_rank !=0){
//		/* create message */
//		sprintf(message, " from process %d!", my_rank);
//		dest = 0;
//		/* use strlen+1 so that '\0' get transmitted */
//		MPI_Send(message, strlen(message)+1, MPI_CHAR,
//		   dest, tag, MPI_COMM_WORLD);
//	}
//	else{
//		printf(" From process 0: Num processes: %d\n",p);
//		for (source = 1; source < p; source++) {
//			MPI_Recv(message, 100, MPI_CHAR, source, tag,
//			      MPI_COMM_WORLD, &status);
//			printf("%s\n",message);
//		}
//	}
//	/* shut down MPI */
//	MPI_Finalize();

	return 0;
}
