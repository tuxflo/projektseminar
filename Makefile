all: put run
put: mpi_types/put.c
	~/Proseminar/mpich-install/bin/mpicc -o mpi_types/put_neu mpi_types/put_neu.c

clean:
	rm -f mpi_type/put

run: ${PROGRAM_NAME}
	../mpich-install/bin/mpiexec -n 2 mpi_types/put_neu
