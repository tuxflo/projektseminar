all: global run

clean:
	rm -f bin/ga_test

run:
	mpiexec -n 4 bin/ga_test

global:
	mpicc -o bin/ga_test src/ga_test.c src/ga_create.c src/ga_mutex1.c src/mutex-create.c src/ga_put.c src/mutex-acquire.c src/mutex-release.c src/ga_get.c src/mcs-lock.c src/hash.c src/separate.c
