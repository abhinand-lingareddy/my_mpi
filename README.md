
compile functions

gcc -c my_mpi.c

compile testing code

gcc h.c -o output my_mpi.c

Run:

./output rank no_of_processors filename

eg:

./output 0 2 host


