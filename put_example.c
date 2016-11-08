#include	"mpi.h"
#include "stdio.h"
/*	 This	 does	 a	 transpose	 with	 a	 get	 operation,	 fence,	 and	 derived
		 datatypes.	 Uses	 vector	 and	 hvector.	 	 Run	 on	 2	 processes	 */
#define	 NROWS	 100
#define	 NCOLS	 100
int	 main(int	 argc,	 char	 *argv[])
{
	int	 rank,	 nprocs,	 A[NROWS][NCOLS],	 i,	 j;
	MPI_Win	 win;
	MPI_Datatype	 column,	 xpose;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	if	 (nprocs	 !=	 2)	 {
		printf("Run	 this	 program	 with	 2	 processes\n");fflush(stdout);
		MPI_Abort(MPI_COMM_WORLD,	 1);
	}
	if	 (rank	 ==	 0)
	{
		for	 (i=0;	 i<NROWS;	 i++)
			for	 (j=0;	 j<NCOLS;	 j++)
				A[i][j]	 =	 -­‐1;
		/*	 create	 datatype	 for	 one	 column	 */
		MPI_Type_vector(NROWS,	 1,	 NCOLS,	 MPI_INT,	 &column);
		/*	 create	 datatype	 for	 matrix	 in	 column-­‐major	 order	 */
		MPI_Type_hvector(NCOLS,	 1,	 sizeof(int),	 column,	 &xpose);
		MPI_Type_commit(&xpose);
		MPI_Win_create(NULL,	 0,	 1,	 MPI_INFO_NULL,	 MPI_COMM_WORLD,	 &win);
		MPI_Win_fence(0,	 win);
		MPI_Get(A,	 NROWS*NCOLS,	 MPI_INT,	 1,	 0,	 1,	 xpose,	 win);
		MPI_Type_free(&column);
		MPI_Type_free(&xpose);
		MPI_Win_fence(0,	 win);

		/*	 check	 data	 transferred	 correctly	 */
		for	 (j=0;	 j<NCOLS;	 j++){
			for	 (i=0;	 i<NROWS;	 i++){
				if	 (A[j][i]	 !=	 i*NCOLS	 +	 j){

					printf("Error:	 A[%d][%d]=%d	 ne	 %d\n",	 j,	 i,	 A[j][i],	 i*NCOLS	 +	 j);
					fflush(stdout);
				}
			}
		}
	}
	else
	{	 /*	 rank	 =	 1	 */
		for	 (i=0;	 i<NROWS;	 i++)
			for	 (j=0;	 j<NCOLS;	 j++)
				A[i][j]	 =	 i*NCOLS	 +	 j;
		MPI_Win_create(A,	 NROWS*NCOLS*sizeof(int),	 sizeof(int),	 MPI_INFO_NULL,	MPI_COMM_WORLD,	 &win);
		MPI_Win_fence(0,	 win);
		MPI_Win_fence(0,	 win);
	}

	MPI_Win_free(&win);
	MPI_Finalize();
	return	 0;
}
