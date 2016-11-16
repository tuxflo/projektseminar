#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <stddef.h>

#define BUFFER_SIZE 256

typedef struct Entry {
        unsigned int id;
        char name[BUFFER_SIZE];
} Entry;

int main(int argc, char **argv) {

    const int tag = 13;
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr,"Requires at least two processes.\n");
        exit(-1);
    }

    /* create a type for struct Entry */
    const int nitems=2;
    /* pass the size of the struct entries: 1 int, BUFFER_SIZE chars */
    int          blocklengths[2] = {1,BUFFER_SIZE};
    MPI_Datatype types[2] = {MPI_INT, MPI_CHAR};
    MPI_Datatype mpi_Entry_type;
    MPI_Aint     offsets[2];

    offsets[0] = offsetof(Entry, id);
    offsets[1] = offsetof(Entry, name);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_Entry_type);
    MPI_Type_commit(&mpi_Entry_type);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    /* first node (0) sends the data */
    if (rank == 0) {
        Entry send;
        send.id = 42;
        strcpy(send.name, "test");

        const int dest = 1;
        MPI_Send(&send,   1, mpi_Entry_type, dest, tag, MPI_COMM_WORLD);

        printf("Rank %d: sent structure Entry\n", rank);
    }
    /* second node (1) receives... */
    if (rank == 1) {
        MPI_Status status;
        const int src=0;

        Entry recv;

        MPI_Recv(&recv,   1, mpi_Entry_type, src, tag, MPI_COMM_WORLD, &status);
        char tmp[BUFFER_SIZE];
        strcpy(tmp, recv.name);
        printf("Rank %d: Received: id = %d name = %s\n", rank,
                 recv.id, tmp);
    }

    MPI_Type_free(&mpi_Entry_type);
    MPI_Finalize();

    return 0;
}
