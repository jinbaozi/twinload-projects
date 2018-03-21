#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>
#include "xmap.c"

#define LCG_MUL64 6364136223846793005ULL
#define LCG_ADD64 1

typedef unsigned long u64Int;
typedef long s64Int;

#define FSTR64 "%ld"
#define FSTRU64 "%lu"
#define ZERO64B 0L
//typedef unsigned long u64Int;  //in our 64 bits machine, the unsigned long is 64 bits

/* Number of updates to table (suggested: 4x number of table entries) */
#define NUPDATE (TableSize)

#define BLOCK_SIZE 1024

/* Utility routine to start LCG random number generator at Nth step */
u64Int
HPCC_starts_LCG(s64Int n)
{
  u64Int mul_k, add_k, ran, un;

  mul_k = LCG_MUL64;
  add_k = LCG_ADD64;

  ran = 1;
  for (un = (u64Int)n; un; un >>= 1) {
    if (un & 1)
      ran = mul_k * ran + add_k;
    add_k *= (mul_k + 1);
    mul_k *= mul_k;
  }

	return ran;
}

void
RandomAccessUpdate_LCG(u64Int TableSize) {
//RandomAccessUpdate_LCG(u64Int TableSize, u64Int *Table) {
	struct timeval tv_pre, tv_after;
        gettimeofday(&tv_pre,NULL);
	u64Int i;
  	u64Int ran[BLOCK_SIZE];              /* Current random numbers */
  	int j, logTableSize;


	for (j=0; j<BLOCK_SIZE; j++)
    		ran[j] = HPCC_starts_LCG((NUPDATE/BLOCK_SIZE) * j);

	gettimeofday(&tv_after,NULL);
	printf("4Elapsed time: %.4f s\n",(double)(tv_after.tv_sec-tv_pre.tv_sec)+(double)(tv_after.tv_usec-tv_pre.tv_usec)/1000000);

  	logTableSize = 0;
  	for (i = 1; i < TableSize; i <<= 1)
    		logTableSize += 1;
	
	gettimeofday(&tv_after,NULL);
	printf("5Elapsed time: %.4f s\n",(double)(tv_after.tv_sec-tv_pre.tv_sec)+(double)(tv_after.tv_usec-tv_pre.tv_usec)/1000000);

	for (i=0; i<NUPDATE/BLOCK_SIZE; i++) {
		/* #pragma ivdep */
#ifdef _OPENMP
		#pragma omp parallel for
#endif
		for (j=0; j<BLOCK_SIZE; j++) {
      			ran[j] = LCG_MUL64 * ran[j] + LCG_ADD64;
      			//Table[ran[j] >> (64 - logTableSize)] ^= ran[j];
      			if(j % 200 == 0)
				tit_clear_0_7();
			tw_store(cached_addr, ran[j] >> (64 - logTableSize), tw_load(cached_addr, ran[j] >> (64 - logTableSize)) ^ ran[j]);
			//tls_u64Int(&Table[ran[j] >> (64-logTableSize)], tl_u64Int(&Table[ran[j]>>(64-logTableSize)])^ran[j]);
    		}
	//gettimeofday(&tv_after,NULL);
	//printf("6--%d, Elapsed time: %.4f s\n",BLOCK_SIZE,(double)(tv_after.tv_sec-tv_pre.tv_sec)+(double)(tv_after.tv_usec-tv_pre.tv_usec)/1000000);
  	}
	gettimeofday(&tv_after,NULL);
	printf("7--%d--Elapsed time: %.4f s\n",BLOCK_SIZE, (double)(tv_after.tv_sec-tv_pre.tv_sec)+(double)(tv_after.tv_usec-tv_pre.tv_usec)/1000000);
}


int
RandomAccess_LCG(unsigned long totalMemSize, int doIO, char *outFname, double *GUPs, int *failure) {
	struct timeval tv_pre, tv_after;
        gettimeofday(&tv_pre,NULL);
	u64Int i;
  	u64Int temp;
  	//double cputime;               /* CPU time to update table */
  	double realtime;              /* Real time to update table */
	struct timeval gups_start, gups_end;
  	double totalMem;
  	//u64Int *Table;
//#endif
  	u64Int logTableSize, TableSize;
  	FILE *outFile = NULL;

  	if (doIO) {
    		outFile = fopen( outFname, "a" );
    		if (! outFile) {
      			outFile = stderr;
      			fprintf( outFile, "#### Cannot open output file.\n" );
      			return 1;
    		}
  	}

	/* calculate local memory per node for the update table */
  	totalMem = (double)totalMemSize;
  	totalMem /= sizeof(u64Int);


  	/* calculate the size of update array (must be a power of 2) */
  	for (totalMem *= 0.5, logTableSize = 0, TableSize = 1;
       		totalMem >= 1.0;
       		totalMem *= 0.5, logTableSize++, TableSize <<= 1)
	;
	
	//uncached_addr	= (int64_t*)(base_addr_nc + 0x10000);
	cached_addr	= (int64_t*)(base_addr + 0x10000);

	printf("$$$$ Malloc Table: %lu MB , size u64Int %d\n", ((TableSize * sizeof(u64Int)) >> 20), sizeof(u64Int));

#if 0
	if (!Table) {
		fprintf(stderr, "#### Failed to malloc space for Table\n");
		exit(-8);
    		if (doIO) {
      			fprintf( outFile, "Failed to allocate memory for the update table (" FSTR64 ").\n", TableSize);
      			fclose( outFile );
    		}
    		return 1;
  	}
#endif

	/* Print parameters for run */
 	if (doIO) {
  		fprintf( outFile, "Main table size   = 2^" FSTR64 " = " FSTR64 " words\n", logTableSize,TableSize);
  		fprintf( outFile, "Number of updates = " FSTR64 "\n", NUPDATE);
  	}
	
	gettimeofday(&tv_after,NULL);
	printf("Elapsed time: %.4f s\n",(double)(tv_after.tv_sec-tv_pre.tv_sec)+(double)(tv_after.tv_usec-tv_pre.tv_usec)/1000000);

#ifdef _OPENMP
#pragma omp parallel 
    {
#pragma omp master
	{
		printf("$$ In omp, Num of omp threads:%d\n", omp_get_num_threads());
		if(doIO)
		{
			fprintf( outFile, "Num of threads = %d\n", omp_get_num_threads());
		}
	}
    }

#pragma omp parallel
    {
    	printf ("Printing one line per active thread....\n");
    }

#endif


	/* Initialize main table */
	for (i=0; i<TableSize; i++) {
		//Table[i] = i;
		tw_store(cached_addr, i, i);
		//*(int64_t*)(uncached_addr + i);
	}

        printf("$$$$ NUpdate %lu\n", NUPDATE);

	/* Begin timing here */
	gettimeofday(&gups_start, NULL);

	gettimeofday(&tv_after,NULL);
	printf("1Elapsed time: %.4f s\n",(double)(tv_after.tv_sec-tv_pre.tv_sec)+(double)(tv_after.tv_usec-tv_pre.tv_usec)/1000000);
  	//RandomAccessUpdate_LCG( TableSize, Table );
  	RandomAccessUpdate_LCG( TableSize);

	gettimeofday(&tv_after,NULL);
	printf("2Elapsed time: %.4f s\n",(double)(tv_after.tv_sec-tv_pre.tv_sec)+(double)(tv_after.tv_usec-tv_pre.tv_usec)/1000000);
  	/* End timed section */
	gettimeofday(&gups_end, NULL);
	
	realtime = (gups_end.tv_sec - gups_start.tv_sec) + ((double)(gups_end.tv_usec - gups_start.tv_usec)) / 1.0E6;
  	printf( "Real time used = %.6f seconds\n", realtime);

  	/* make sure no division by zero */
  	*GUPs = (realtime > 0.0 ? 1.0 / realtime : -1.0);
  	*GUPs *= 1e-9*NUPDATE;
	
	/* Print timing results */
  	if (doIO) {
  		//fprintf( outFile, "CPU time used  = %.6f seconds\n", cputime);
  		fprintf( outFile, "Real time used = %.6f seconds\n", realtime);
  		fprintf( outFile, "%.9f Billion(10^9) Updates    per second [GUP/s]\n", *GUPs );
  	}

	temp = 0;
#ifdef UPDATE_CHECK
	 /* Verification of results (in serial or "safe" mode; optional) */
  	temp = 0x1;
  	for (i=0; i<NUPDATE; i++) {
    		temp = LCG_MUL64 * temp + LCG_ADD64;
    		//Table[temp >> (64 - (int)logTableSize)] ^= temp;
		//tw_store_nc(uncached_addr, temp >> (64 - logTableSize), tw_load_nc(uncached_addr, temp >> (64 - logTableSize)) ^ temp);
      		if(j % 200 == 0)
			tit_clear_0_7();
		tw_store(cached_addr, temp >> (64 - logTableSize), tw_load(cached_addr, temp >> (64 - logTableSize)) ^ temp);
  	}

  	temp = 0;
  	for (i=0; i<TableSize; i++)
    		//if (Table[i] != i)
    		if(tw_load(cached_addr, i) != i)
      			temp++;

  	if (doIO) {
  		fprintf( outFile, "Found " FSTR64 " errors in " FSTR64 " locations (%s).\n",
           		temp, TableSize, (temp <= 0.01*TableSize) ? "passed" : "failed");
  	}
#endif
  	if (temp <= 0.01*TableSize) *failure = 0;
  	else *failure = 1;
//#ifndef TWO_ACCESS
//	free(Table);
//#endif
	if (doIO) {
		fprintf( outFile, "-------------------------------------------------------------\n\n");
    		fflush( outFile );
    		fclose( outFile );
  	}
	

  	return 0;

}

void print_help_msg()
{
	printf("Usage:./RandomAccess TotalMemSize(MB) [numThreads] [outFileName=gupsout]\n");
}

int main(int argc, char *argv[])
{
	struct timeval tv_pre, tv_after;
	gettimeofday(&tv_pre,NULL);
	unsigned long totalMemSize = 0;
	char *outFileName = "gupsout";
	int numThreads = 1;

	double GUPS;
	int faillure;

	if(argc != 2 && argc != 3 && argc != 4)
	{
		fprintf(stderr, "Error: invalid input parameters\n");
		print_help_msg();
		exit(-8);
	}

	totalMemSize = atoi(argv[1]);
	totalMemSize <<= 20;
	
	printf("$$ Get total memory size:%lu MB\n",totalMemSize>>20);

	if(argc == 3)
	{
		numThreads = atoi(argv[2]);
		printf("$$ Get num of threads:%d\n",numThreads);
#ifdef _OPENMP
		omp_set_num_threads(numThreads);
#endif
	}
	
	if(argc == 4)
	{
		outFileName = argv[3];
		printf("$$ Get Out File Name:%s\n", outFileName);
	}	

	fd = open("/dev/top_mc",O_RDWR);
        if(fd < 0)
        {
                printf("can not open /dev/top_mc, errno = %d!\n", errno);
                return 0;
        }

        base_addr = mmap(0, CFG_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        printf("base_addr=%lx\n", base_addr);
        if(base_addr < 0){
                printf("can not mmap /dev/top_mc, errno=%d\n", errno);
                return -1;
        }

        fd_nc = open("/dev/top_mc_nc",O_RDWR);
        if(fd_nc < 0)
        {
                printf("can not open /dev/top_mc_nc, errno = %d!\n", errno);
                return 0;
        }

        base_addr_nc = mmap(0, CFG_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_nc, 0);
        printf("base_addr_nc=%lx\n", base_addr_nc);
        if(base_addr_nc < 0){
                printf("can not mmap /dev/top_mc, errno=%d\n", errno);
                return -1;
        }
	ddr_buffer_init();
        tit_debug();
        tit_clear();

        tit_clear_0_7();
	//u64Int *test = malloc(totalMemSize * sizeof(u64Int));
	//memset(test, 0, totalMemSize * sizeof(u64Int));
	//free(test);
	RandomAccess_LCG(totalMemSize, 1, outFileName, &GUPS, &faillure);

	if(!faillure)
	{
		printf("$$ Run RandomAccess Successfully, GUPS=%.6f\n", GUPS);
	}	
	else
	{
		fprintf(stderr, "## Run RandomAccess Failed\n");
	}
	gettimeofday(&tv_after,NULL);
	printf("3Elapsed time: %.4f s\n",(double)(tv_after.tv_sec-tv_pre.tv_sec)+(double)(tv_after.tv_usec-tv_pre.tv_usec)/1000000);
}
