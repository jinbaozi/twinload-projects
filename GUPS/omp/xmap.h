#ifndef __COMMON__
#define __COMMON__

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#define FAKE_DATA1              0xffffffffffffffff                                                                    
#define FAKE_DATA2              0xaaaaaaaaaaaaaaaa                                                                    
#define FAKE_DATA3              0x7fffffffffffffff                                                                    
#define SET_DATA                0xbbbbbbbbbbbbbbbb                                                                    
#define SUCCESS_DATA            0xcccccccccccccccc                                                                    
#define RETRY_TIMES             1000                                                                   
#define RETRY_TIMES_LONG        10000
#define READ_TIMES_PER_CL       1                                                                                     
#define TIT_CLEAR_NCL           200                                                                                   
#define TIT_CLEAR_NLD           10
#define CPU_FREQ_MHZ            950

#define	NZ	NA*(NONZER+1)*(NONZER+1)+NA*(NONZER+2)

#define	CFG_SIZE	0x80000000

int tit_clear_count = 1;
int retry_times_count = 0;

typedef unsigned long u64Int;
typedef long s64Int;

int fd, fd_nc;

unsigned char *base_addr;
unsigned char *base_addr_nc;

int64_t   *cached_addr;
int64_t   *uncached_addr;

extern void tit_debug(void);
extern void ddr_buffer_init(void);
extern unsigned int tit2virtual_convert(unsigned int tit_addr);
extern unsigned int virtual2phy_convert(unsigned long virtual_addr);
extern unsigned long* virtual_addr_derive(unsigned long* base_addr, unsigned int phy_row, unsigned int phy_bank, unsigned int phy_col);
        
extern void tit_clear_0_7_nocache(void);                                                                              
extern void tit_clear_0_7(void);                                                                                      
extern void tit_clear_8_11(void);                                                                                     
extern void tit_clear_12_19(void);                                                                                    
extern void tit_clear(void);

extern int tw_load(int64_t *addr, int i);
extern void tw_store(int64_t *addr, int i, int j);

extern int tw_load_nc(int64_t *addr, int i);
extern void tw_store_nc(int64_t *addr, int i, int j);

extern double tw_load_d(double *addr, int i);
extern void tw_store_d(double *addr, int i, double j);
#endif
