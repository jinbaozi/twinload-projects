#ifndef _TEST_H_
#define _TEST_H_

#define cache_op_usr(op,addr)                                               \
        __asm__ __volatile__(                                           \
        "       .set    push                                    \n"     \
        "       .set    noreorder                               \n"     \
        "       .set    mips3\n\t                               \n"     \
        "       cache   %0, %1                                  \n"     \
        "       .set    pop                                     \n"     \
        :                                                               \
        : "i" (op), "R" (*(unsigned char *)(addr)))


#define __sync()				\
	__asm__ __volatile__(			\
		".set	push\n\t"		\
		".set	noreorder\n\t"		\
		".set	mips2\n\t"		\
		"sync\n\t"			\
		".set	pop"			\
		: /* no output */		\
		: /* no input */		\
		: "memory")

static inline void flush_dcache_line_usr(unsigned long addr)
{
        cache_op_usr(0x15, addr);
}

static inline void flush_scache_line_usr(unsigned long addr)
{
        cache_op_usr(0x17, addr);
}

#endif
