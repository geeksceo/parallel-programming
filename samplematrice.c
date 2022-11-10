#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>

MPI_Status status;
int N = 3;
double a[3][3] = {{1, 4, 7}, {2, 5, 8}, {3, 6, 9} }, b[3][3] = {{-1, 8, 11}, {-2, 9, 10}, {3, 10, 9}}, c[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

int main() {
	int numtasks, taskid, numworkers, source, dest, rows, offset, i, j, k;
	
	struct timeval start, stop;
	
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	numworkers = numtasks - 1;
	/* MASTER */
	if(taskid == 0) {
		gettimeofday(&start, 0);
		/* SEND MQTRIX DATA */
		rows = N/numworkers;
		offset = 0;
		// Boucle
		for(dest = 1; dest <= numworkers; dest++) 
		{
			MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
			MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
			MPI_Send(&a[offset][0], rows*N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
			MPI_Send(&a, N*N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
			offset = offset + rows;
		}
		
		//
		for(i = 1; i <= numworkers; i++) 
		{
			source = i;
			MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&c[offset][0], rows*N, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &status);
		}
		gettimeofday(&stop, 0);
		
		printf("Ici le resultat de la MATRICE : \n");
		for(i = 0; i < N; i++) {
			for(j = 0; j < N; j++) {
				printf("%602f  ", c[i][j]);
				printf("\n");
			}
			
			fprintf(stdout, "Time = %6.2f\n\n", (stop.tv_sec+stop.tv_usec*1e-6)-(start.tv_sec+start.tv_usec*1e-6));
		}
	}
	/* WORKER */
	if(taskid > 0) {
		source = 0;
		
		MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&a, rows*N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&b, N*N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
		
		/* MULTIPLICATION */
		for(k = 0; k < N; k++)
			for(i = 0; i < N; i++){
				c[i][k] = 0.0;
				for(j = 0; j < N; j++)
					c[i][k] += a[i][j] * b[j][k];
			}
		MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
		MPI_Send(&c, rows*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
	}
} 



