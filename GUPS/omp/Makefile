CC=gcc
#CFLAGS=-O3 -fopenmp
CFLAGS=-g

all:single_node_lcg_1_omp
#single_node_lcg_omp single_node_lcg_1_omp
#single_node_lcg_omp:single_node_lcg.c
#	$(CC) $(CFLAGS) -o single_node_lcg_omp single_node_lcg.c  
single_node_lcg_1_omp:single_node_lcg_1.c
	$(CC) $(CFLAGS) -o single_node_lcg_1_omp single_node_lcg_1.c  
clean:
	rm -f single_node_lcg_omp single_node_lcg_1_omp
