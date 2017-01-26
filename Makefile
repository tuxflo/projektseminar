all: main_test

clean:
	rm -f bin/*

run_main_test:
	HASH=0 mpiexec -n 3 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 4 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 5 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 6 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 7 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 8 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 9 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 10 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 11 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 12 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 13 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 14 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 15 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 16 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 17 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 18 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 19 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 20 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 21 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 22 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 23 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 24 -f machines.txt bin/main_test
	HASH=0 mpiexec -n 25 -f machines.txt bin/main_test

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
