all: global run

clean:
	rm -f bin/ga_test

run:
	mpirun -np 4 --hostfile machines.txt bin/ga_test

global:
	mpicc -std=c99 -o bin/ga_test src/ga_test.c src/ga_create.c src/ga_mutex1.c src/mutex-create.c src/ga_put.c src/mutex-acquire.c src/mutex-release.c src/ga_get.c src/mcs-lock.c src/hash.c src/separate.c

leia:
	mpicc -std=c99 -o bin/leia src/experiment_leia.c src/ga_create.c src/ga_mutex1.c src/mutex-create.c src/ga_put.c src/mutex-acquire.c src/mutex-release.c src/ga_get.c src/mcs-lock.c src/hash.c src/separate.c
