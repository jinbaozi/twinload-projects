#include "xmap.h"
#include "user_cache_opt.h"

extern  void tit_debug()
{
	int 		j;
	unsigned int	tit_state[20];
	unsigned int	tit_address[20];

	printf("print tit debug registers now!!!!!\n");

	// invalidate cacheline
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7ac0));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b00));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b40));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7ac0));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b00));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b40));
	//__sync();

	// read tit registers, cached
	for(j = 0; j < 20; ++j){
		tit_state[j] = *(unsigned int*)(base_addr + 0x7b18 - 4*j);
		tit_address[j] = *(unsigned int*)(base_addr + 0x7b68 - 4*j);
		printf("TIT message: tit_state[%d] = 0x%x\t, tit_address[%d] = 0x%x\n", j, tit_state[j], j, tit_address[j]);
	}

	printf("print tit debug registers over!!!\n");
	return;
}

extern  void ddr_buffer_init()
{

	int 		k;
	unsigned long	*init_addr;
	unsigned long	*config_addr;
	unsigned long	config_data;

	printf("start test!!!!!!!!!!!!!!!!!!!!!!!\n");

	// ddr buffer init start
	config_addr	= (unsigned long*)(base_addr + 0x7000);
	init_addr	= (unsigned long*)(base_addr);
	printf("config_addr=%lu\n", config_addr);
	printf("init_addr=%lu\n", init_addr);

	*(unsigned long*)(config_addr + 0x10) = 0x0;
	*(unsigned long*)(config_addr + 0x11) = 0x0;
	*(unsigned long*)(config_addr + 0x12) = 0x0;
	*(unsigned long*)(config_addr + 0x13) = 0x0;
	*(unsigned long*)(config_addr + 0x14) = 0x0;
	*(unsigned long*)(config_addr + 0x15) = 0x0;
	*(unsigned long*)(config_addr + 0x16) = 0x0;
	*(unsigned long*)(config_addr + 0x17) = 0x0;
	//flush_dcache_line_usr((unsigned long)(config_addr + 0x10));
	flush_scache_line_usr((unsigned long)(config_addr + 0x10));
	//__sync();
	*(unsigned long*)(init_addr + 0x0) = 0x6666666666666666;
	*(unsigned long*)(init_addr + 0x1) = 0x0;
	*(unsigned long*)(init_addr + 0x2) = 0x0;
	*(unsigned long*)(init_addr + 0x3) = 0x0;
	*(unsigned long*)(init_addr + 0x4) = 0x0;
	*(unsigned long*)(init_addr + 0x5) = 0x0;
	*(unsigned long*)(init_addr + 0x6) = 0x0;
	*(unsigned long*)(init_addr + 0x7) = 0x0;
	//flush_dcache_line_usr((unsigned long)(init_addr));
	flush_scache_line_usr((unsigned long)(init_addr));
	//__sync();
	k = 0;
	do{
		++k;
		//invalidate_dcache_line((unsigned long)(config_addr + 0x10));
		//flush_dcache_line_usr((unsigned long)(config_addr + 0x10));
		flush_scache_line_usr((unsigned long)(config_addr + 0x10));
		//__sync();
		config_data = *(unsigned long*)(config_addr + 0x10);
		if((config_data & 0x1ULL) == 0x1ULL)
			break;
	}while(k < 10000000);
	printf("ddr_buffer init completed!!!!!!!!!!!!!!!!!\n");
	// ddr buffer init end
	
	// ddr buffer config start
	*(unsigned long*)(config_addr + 0x8) 	= 0x1;
	*(unsigned long*)(config_addr + 0x9) 	= 0x0;
	*(unsigned long*)(config_addr + 0xa) 	= 0x0;
	*(unsigned long*)(config_addr + 0xb) 	= 0x0;
	*(unsigned long*)(config_addr + 0xc) 	= 0x0;
	*(unsigned long*)(config_addr + 0xd) 	= 0x0;
	*(unsigned long*)(config_addr + 0xe) 	= 0x0;
	*(unsigned long*)(config_addr + 0xf) 	= 0x0;
  	//flush_dcache_line_usr((unsigned long)(config_addr + 0x8));
  	flush_scache_line_usr((unsigned long)(config_addr + 0x8));
	//__sync();
	*(unsigned long*)(config_addr + 0x18) 	= 0xaaaaaaaaffffffff;
	*(unsigned long*)(config_addr + 0x19) 	= 0xccccccccbbbbbbbb;
	*(unsigned long*)(config_addr + 0x1a) 	= 0x0;
	*(unsigned long*)(config_addr + 0x1b) 	= 0x0;
	*(unsigned long*)(config_addr + 0x1c) 	= 0x0;
	*(unsigned long*)(config_addr + 0x1d) 	= 0x0;
	*(unsigned long*)(config_addr + 0x1e) 	= 0x0;
	*(unsigned long*)(config_addr + 0x1f) 	= 0x0;
	//flush_dcache_line_usr((unsigned long)(config_addr + 0x18));
	flush_scache_line_usr((unsigned long)(config_addr + 0x18));
	//__sync();
	printf("config_addr = 0x%lx, data = 0x%lx\n", (unsigned long)(config_addr + 0x18), *(unsigned long*)(config_addr + 0x18));
	printf("config_addr = 0x%lx, data = 0x%lx\n", (unsigned long)(config_addr + 0x19), *(unsigned long*)(config_addr + 0x19));
	//__sync();
	// ddr buffer config end

	return;
}

extern  unsigned int tit2virtual_convert(unsigned int tit_addr)
{
	unsigned int	tit_bank, tit_row, tit_col;
	unsigned int	virtual_bank, virtual_row, virtual_col, virtual_addr;

	tit_bank = (tit_addr & 0x1c000000) >> 26;
	tit_row  = (tit_addr & 0x03fffc00) >> 10;
	tit_col  = (tit_addr & 0x000003ff);

	virtual_bank = ((tit_row & 0x00000070) >> 4) ^ tit_bank;
	virtual_row  = tit_row;
	virtual_col  = tit_col;

	virtual_addr = (virtual_row << 16) | (virtual_bank << 13) | (virtual_col << 3);

	return(virtual_addr);
}

extern  unsigned int virtual2phy_convert(unsigned long virtual_addr)
{
	unsigned int	virtual_row, virtual_bank, virtual_col;
	unsigned int	phy_row, phy_bank, phy_col, phy_addr;

	virtual_row  =	(virtual_addr & 0xffff0000) >> 16;
	virtual_bank =	(virtual_addr & 0x0000e000) >> 13;
	virtual_col  =	(virtual_addr & 0x00001ff8) >> 3;

	phy_row      =  virtual_row;
	phy_bank     =	((virtual_row & 0x00000070) >> 4) ^ virtual_bank;
	phy_col      =	virtual_col;

	phy_addr     =	(phy_row << 16) | (phy_bank << 13) | (phy_col << 3);

	return(phy_addr);
}

extern  unsigned long* virtual_addr_derive(unsigned long* base_addr, unsigned int phy_row, unsigned int phy_bank, unsigned int phy_col)
{
	unsigned int 	virtual_row, virtual_bank, virtual_col, virtual_offset;
	unsigned long	*virtual_addr;

	virtual_row 	= phy_row;
	virtual_bank 	= ((phy_row & 0x00000070) >> 4) ^ phy_bank;
	virtual_col 	= phy_col;

	virtual_offset 	= (virtual_row << 13) | (virtual_bank << 10) | virtual_col;
	virtual_addr	= (unsigned long*)(base_addr + virtual_offset);

	return(virtual_addr);
}	

extern  void tit_clear_0_7()
{
	int 			j;
	unsigned int		tit_state[8];
	unsigned int		tit_address[8];
	volatile unsigned char	tit_data[8];
//	unsigned int		cnt_before_clear;
//	unsigned int		cnt_after_clear;

//	cnt_before_clear = read_c0_count();

	//invalidate_dcache_line((unsigned long)(base_addr + 0x7ac0));
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7b00));
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7b40));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7ac0));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b00));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b40));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7ac0));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b00));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b40));
	//__sync();

	// read tit registers
	for(j = 0; j < 8; ++j){
		tit_state[j] = *(unsigned int*)(base_addr + 0x7b18 - 4*j);
		tit_address[j] = *(unsigned int*)(base_addr + 0x7b68 - 4*j);
//		printf("before clear: tit_state[%d] = 0x%x\t, tit_address[%d] = 0x%x\n", j, tit_state[j], j, tit_address[j]);
	}

	// clear tit 0-7
	for(j = 0; j < 8; ++j){
		if((tit_state[j] & 0xff) == 0x26){
			tit_data[j] = *(base_addr_nc + tit2virtual_convert(tit_address[j]));
		}
	}

//	cnt_after_clear = read_c0_count();
//	printf("count during clear = %u\n", (unsigned int)(cnt_after_clear - cnt_before_clear));

	return;
}

extern  void tit_clear_8_11()
{
	int 			j, k;
	unsigned int		tit_state[4];
	unsigned int		tit_address[4];
	volatile unsigned long	val = 0;

	// invalidate cacheline
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7ac0));
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7b00));
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7b40));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7ac0));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b00));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b40));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7ac0));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b00));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b40));
	//__sync();

	// read tit registers, cached
	for(j = 0; j < 4; ++j){
		tit_state[j] = *(unsigned int*)(base_addr + 0x7af8 - 4*j);
		tit_address[j] = *(unsigned int*)(base_addr + 0x7b48 - 4*j);
//		printf("before clear: tit_state[%d] = 0x%x\t, tit_address[%d] = 0x%x\n", j, tit_state[j], j, tit_address[j]);
	}

	// clear tit 8-11, cached
	for(j = 0; j < 4; ++j){
		if((tit_state[j] & 0xff) == 0x28){ 
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x0)  = 0x10000;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x8)  = 0x1;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x10) = 0x10010;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x18) = 0x10018;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x20) = 0x10020;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x28) = 0x10028;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x30) = 0x10030;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x38) = 0x10038;
			//flush_dcache_line_usr((unsigned long)(base_addr + tit2virtual_convert(tit_address[j])));
			flush_scache_line_usr((unsigned long)(base_addr + tit2virtual_convert(tit_address[j])));
			//__sync();
			k = 0;
			do{
				++k;
				val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
				if(val == FAKE_DATA1){
					continue;
				}
				else if(val != FAKE_DATA2){
					break;
				}
				else{
					++k;
					val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
					break;
				}
			}while(k < RETRY_TIMES_LONG);
		}
		else if((tit_state[j] & 0xff) == 0x4a){
			val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
		}
		else if((tit_state[j] & 0xff) == 0x48){
			k = 0;
			do{
				++k;
				val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
				if(val == FAKE_DATA1){
					continue;
				}
				else if(val != FAKE_DATA2){
					break;
				}
				else{
					++k;
					val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
					break;
				}
			}while(k < RETRY_TIMES_LONG);
		}
	}

	return;
}

extern  void tit_clear_12_19()
{
	int 			j, k;
	unsigned int		tit_state[8];
	unsigned int		tit_address[8];
	volatile unsigned long	val = 0;

	// invalidate cacheline
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7ac0));
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7b00));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7ac0));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b00));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7ac0));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b00));
	//__sync();

	// read tit registers, cached
	for(j = 0; j < 8; ++j){
		tit_state[j] = *(unsigned int*)(base_addr + 0x7ae8 - 4*j);
		tit_address[j] = *(unsigned int*)(base_addr + 0x7b38 - 4*j);
//		printf("before clear: tit_state[%d] = 0x%x\t, tit_address[%d] = 0x%x\n", j, tit_state[j], j, tit_address[j]);
	}

	// clear tit 12-19, cached
	for(j = 0; j < 8; ++j){
		if((tit_state[j] & 0xff) == 0x2c){
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x0)  = (unsigned long)virtual_addr_derive(((unsigned long*)0x0), 0x1, j, 0x8);
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x8)  = (unsigned long)virtual_addr_derive(((unsigned long*)0x0), 0x2, j, 0x8);
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x10) = 0x1;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x18) = 0x0;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x20) = 0x0;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x28) = 0x0;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x30) = 0x0;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x38) = 0x0;
			//flush_dcache_line_usr((unsigned long)(base_addr + tit2virtual_convert(tit_address[j])));
			flush_scache_line_usr((unsigned long)(base_addr + tit2virtual_convert(tit_address[j])));
			//__sync();
			k = 0;
			do{
				++k;
				val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
				if(val != FAKE_DATA1)
					break;
			}while(k < RETRY_TIMES_LONG);
		}
		else if((tit_state[j] & 0xff) == 0x4e){
			val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
		}
		else if((tit_state[j] & 0xff) == 0x4c){
			k = 0;
			do{
				++k;
				val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
				if(val != FAKE_DATA1)
					break;
			}while(k < RETRY_TIMES_LONG);
		}
	}

	return;
}

extern  void tit_clear()
{
	int 			j, k;
	unsigned int		tit_state[20];
	unsigned int		tit_address[20];
	volatile unsigned char	tit_data[8];
	volatile unsigned long	val = 0;

	// invalidate cacheline
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7ac0));
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7b00));
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7b40));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7ac0));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b00));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b40));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7ac0));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b00));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b40));
	//__sync();

	// read tit registers, cached
	for(j = 0; j < 20; ++j){
		tit_state[j] = *(unsigned int*)(base_addr + 0x7b18 - 4*j);
		tit_address[j] = *(unsigned int*)(base_addr + 0x7b68 - 4*j);
		//__sync();
		printf("before clear: tit_state[%d] = 0x%x\t, tit_address[%d] = 0x%x\n", j, tit_state[j], j, tit_address[j]);
	}

	// clear tit 0-7, no cache
	printf("clear tit 0-7\n");
	for(j = 0; j < 8; ++j){
		if((tit_state[j] & 0xff) == 0x26){
			tit_data[j] = *(base_addr_nc + tit2virtual_convert(tit_address[j]));
			//__sync();
		}
	}

	// clear tit 8-11, cached
	printf("clear tit 8-11\n");
	for(j = 8; j < 12; ++j){
		if((tit_state[j] & 0xff) == 0x28){ 
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x0)  = 0x10000;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x8)  = 0x1;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x10) = 0x10010;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x18) = 0x10018;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x20) = 0x10020;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x28) = 0x10028;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x30) = 0x10030;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x38) = 0x10038;
			//flush_dcache_line_usr((unsigned long)(base_addr + tit2virtual_convert(tit_address[j])));
			flush_scache_line_usr((unsigned long)(base_addr + tit2virtual_convert(tit_address[j])));
			//__sync();
			k = 0;
			do{
				++k;
				val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
				if(val == FAKE_DATA1){
					continue;
				}
				else if(val != FAKE_DATA2){
					break;
				}
				else{
					++k;
					val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
					break;
				}
			}while(k < RETRY_TIMES_LONG);
		}
		else if((tit_state[j] & 0xff) == 0x4a){
			val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
		}
		else if((tit_state[j] & 0xff) == 0x48){
			k = 0;
			do{
				++k;
				val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
				if(val == FAKE_DATA1){
					continue;
				}
				else if(val != FAKE_DATA2){
					break;
				}
				else{
					++k;
					val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
					break;
				}
			}while(k < RETRY_TIMES_LONG);
		}
	}

	// clear tit 12-19, cached
	printf("clear tit 12-19\n");
	for(j = 12; j < 20; ++j){
		if((tit_state[j] & 0xff) == 0x2c){
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x0)  = (unsigned long)virtual_addr_derive(((unsigned long*)0x0), 0x1, (j - 12), 0x8);
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x8)  = (unsigned long)virtual_addr_derive(((unsigned long*)0x0), 0x2, (j - 12), 0x8);
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x10) = 0x1;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x18) = 0x0;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x20) = 0x0;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x28) = 0x0;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x30) = 0x0;
			*(unsigned long*)(base_addr + tit2virtual_convert(tit_address[j]) + 0x38) = 0x0;
			//flush_dcache_line_usr((unsigned long)(base_addr + tit2virtual_convert(tit_address[j])));
			flush_scache_line_usr((unsigned long)(base_addr + tit2virtual_convert(tit_address[j])));
			//__sync();
			k = 0;
			do{
				++k;
				val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
				if(val != FAKE_DATA1)
					break;
			}while(k < RETRY_TIMES_LONG);
		}
		else if((tit_state[j] & 0xff) == 0x4e){
			val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
		}
		else if((tit_state[j] & 0xff) == 0x4c){
			k = 0;
			do{
				++k;
				val = *(unsigned long*)(base_addr_nc + tit2virtual_convert(tit_address[j]));
				if(val != FAKE_DATA1)
					break;
			}while(k < RETRY_TIMES_LONG);
		}
	}

	// invalidate cacheline
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7ac0));
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7b00));
	//invalidate_dcache_line((unsigned long)(base_addr + 0x7b40));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7ac0));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b00));
	//flush_dcache_line_usr((unsigned long)(base_addr + 0x7b40));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7ac0));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b00));
	flush_scache_line_usr((unsigned long)(base_addr + 0x7b40));
	//__sync();

	// verify tit registers, cached
	for(j = 0; j < 20; ++j){
		tit_state[j] = *(unsigned int*)(base_addr + 0x7b18 - 4*j);
		tit_address[j] = *(unsigned int*)(base_addr + 0x7b68 - 4*j);
		//__sync();
		printf("after clear: tit_state[%d] = 0x%x\t, tit_address[%d] = 0x%x\n", j, tit_state[j], j, tit_address[j]);
	}

	return;
}

/*if use flush_dcache_line_usr(), it will return -1*/
extern int tw_load(int64_t *addr, int i)
{
	//register int val;
	register int k = 0;
		do{	
			++k;
			//val = *(int*)(addr + i);
			if(*(int*)(addr + i) == FAKE_DATA1){
				++retry_times_count;
				flush_scache_line_usr((unsigned long)(addr + i));
				if(k % TIT_CLEAR_NLD == 0){
					tit_clear_0_7();
					++tit_clear_count;
				}
				continue;
			}
			else if(*(int*)(addr + i) != FAKE_DATA2){
				break;
			}
			else{
				++retry_times_count;
				flush_scache_line_usr((unsigned long)(addr + i));
				//flush_scache_line_usr(((unsigned long)(addr + i))&(~63));
				++k;
				//val = *(int*)((int *)addr + i);
				break;
			}
		}while(k < RETRY_TIMES);
#if 1
		if(k == RETRY_TIMES)
			printf("Warning!tw_load:addr = 0x%llx, i = %d, val = %d\n", addr, i, *(int*)(addr + i));
#endif
	return *(int*)(addr + i);
}

extern void tw_store(int64_t *addr, int i, int v)
{
	register int k = 0;
	//register int val;
	do{
		++k;
#if 1
		if(k % 50 == 0)
		{
			tit_clear_0_7();
			++tit_clear_count;
		}
#endif
		//val = tw_load(addr, i);
		//if(val != FAKE_DATA1)
		if(tw_load(addr, i) != FAKE_DATA1)
			break;
		flush_scache_line_usr((unsigned long)(addr + i));
		++retry_times_count;
	}while(k < RETRY_TIMES);
#if 1
	if(k == RETRY_TIMES)
		printf("Warning!tw_store:addr = 0x%llx, i = %d, val = %d\n", addr, i, tw_load(addr, i));
#endif

	*(int64_t *)(addr + i) = v;

	flush_scache_line_usr((unsigned long)(addr + i));
}

extern int tw_load_nc(int64_t *addr, int i)
{
	//register int val;
	int k = 0;
		do{	
			k++;
			//val = *(int*)(addr + i);
			if(*(int*)(addr + i) == FAKE_DATA1){
				retry_times_count++;
				continue;
			}
			else if(*(int*)(addr + i) != FAKE_DATA2){
				break;
			}
			else{
				retry_times_count++;
				k++;
				//val = *(int*)(addr + i);
				break;
			}
		}while(k < RETRY_TIMES);
#if 1
		if(k == RETRY_TIMES)
			printf("Warning!tw_load_nc:addr = 0x%llx, i = %d, val = %d\n", addr, i, *(int*)(addr + i));
#endif
	return *(int*)(addr + i);
}

extern void tw_store_nc(int64_t *addr, int i, int v)
{
#if 0
	register int k = 0;
	//register int val;
	do{
		k++;
#if 1
		if(k % 50 == 0)
		{
			tit_clear_0_7();
			tit_clear_count++;
		}
#endif
		//val = tw_load_nc(addr, i);
		//if(val != FAKE_DATA1)
		if(tw_load_nc(addr, i) != FAKE_DATA1)
			break;
		__sync();	
		retry_times_count++;
	}while(k < RETRY_TIMES);
#endif

	*(int64_t *)(addr + i) = v;
}

extern double tw_load_d(double *addr, int i)
{
	//register int val;
	//long* temp;
	register int k = 0;
		do{	
			++k;
			//temp = (long*)(addr + i);
			//val = *(int*)(addr + i);
			if(*(int*)(addr + i) == FAKE_DATA1){
				++retry_times_count;
				flush_scache_line_usr((unsigned long)(addr + i));
				if(k % TIT_CLEAR_NLD == 0){
					tit_clear_0_7();
					++tit_clear_count;
				}
				continue;
			}
			else if(*(int*)(addr + i) != FAKE_DATA2){
				break;
			}
			else{
				++retry_times_count;
				flush_scache_line_usr((unsigned long)(addr + i));
				++k;
				break;
			}
		}while(k < RETRY_TIMES);
#if 1
		if(k == RETRY_TIMES)
			//printf("Warning!tw_load:addr = 0x%llx, i = %d, val = %ld\n", addr, i, val);
			printf("Warning!tw_load_d:addr = 0x%llx, i = %d, val = %ld\n", addr, i, *(double*)(addr + i));
#endif
	return *(double*)(addr + i);
}

extern void tw_store_d(double *addr, int i, double v)
{
	register int k = 0;
	//register int val;
	do{
		++k;
#if 1
		if(k % 50 == 0)
		{
			tit_clear_0_7();
			++tit_clear_count;
		}
#endif
		//val = tw_load((int64_t*)addr, i);
		//if(val != FAKE_DATA1)
		if(tw_load((int64_t*)addr, i) != FAKE_DATA1)
		{
			break;
		}
		flush_scache_line_usr((unsigned long)(addr + i));
		++retry_times_count;
	}while(k < RETRY_TIMES);
#if 1
	if(k == RETRY_TIMES)
		printf("Warning!tw_store_d:addr = 0x%llx, i = %d, val = %lf\n", addr, i, tw_load((int64_t*)addr, i));
#endif

	*(double *)(addr + i) = v;
	flush_scache_line_usr((unsigned long)(addr + i));
}
