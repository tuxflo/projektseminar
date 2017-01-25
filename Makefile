all: main_test

clean:
	rm -f bin/*

run_main_test:
	mpiexec -n 9 -f machines.txt bin/main_test

main_test: compile_main_test run_main_test

compile_main_test:
	mpicc -o bin/main_test src/main_test.c src/la.c src/mutex.c src/hash.c src/separate.c src/mpi_error.c src/check.c src/insert.c

dist: compile_dist run_dist

compile_dist:
	gcc -o bin/dist src/distribution.c src/hash.c src/separate.c

run_dist:
	./bin/dist 10 20
	
compile_januar:
	mpicc -o bin/januar src/januar.c src/la.c src/mutex.c src/hash.c src/separate.c src/mpi_error.c src/check.c src/insert.c

run_januar:
	mpiexec -n 9 -f machines.txt bin/januar
