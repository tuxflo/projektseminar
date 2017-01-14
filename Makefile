all: main_test

clean:
	rm -f bin/*

run_main_test:
	mpiexec -n 4 -f machines.txt bin/main_test

main_test: compile_main_test run_main_test

compile_main_test:
	mpicc -o bin/main_test src/main_test.c src/la.c src/mutex.c src/hash.c src/separate.c src/mpi_error.c

