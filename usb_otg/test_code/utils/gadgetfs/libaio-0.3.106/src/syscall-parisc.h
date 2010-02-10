/*
 * Linux system call numbers.
 *
 * Cary Coutant says that we should just use another syscall gateway
 * page to avoid clashing with the HPUX space, and I think he's right:
 * it will would keep a branch out of our syscall entry path, at the
 * very least.  If we decide to change it later, we can ``just'' tweak
 * the LINUX_GATEWAY_ADDR define at the bottom and make __NR_Linux be
 * 1024 or something.  Oh, and recompile libc. =)
 *
 * 64-bit HPUX binaries get the syscall gateway address passed in a register
 * from the kernel at startup, which seems a sane strategy.
 */

#define __NR_Linux                0
#define __NR_io_setup           (__NR_Linux + 215)
#define __NR_io_destroy         (__NR_Linux + 216)
#define __NR_io_getevents       (__NR_Linux + 217)
#define __NR_io_submit          (__NR_Linux + 218)
#define __NR_io_cancel          (__NR_Linux + 219)

#define SYS_ify(syscall_name)   __NR_##syscall_name

/* Assume all syscalls are done from PIC code just to be
 * safe. The worst case scenario is that you lose a register
 * and save/restore r19 across the syscall. */
#define PIC

/* Definition taken from glibc 2.3.3
 * sysdeps/unix/sysv/linux/hppa/sysdep.h
 */

#ifdef PIC
/* WARNING: CANNOT BE USED IN A NOP! */
# define K_STW_ASM_PIC	"       copy %%r19, %%r4\n"
# define K_LDW_ASM_PIC	"       copy %%r4, %%r19\n"
# define K_USING_GR4	"%r4",
#else
# define K_STW_ASM_PIC	" \n"
# define K_LDW_ASM_PIC	" \n"
# define K_USING_GR4
#endif

/* GCC has to be warned that a syscall may clobber all the ABI
   registers listed as "caller-saves", see page 8, Table 2
   in section 2.2.6 of the PA-RISC RUN-TIME architecture
   document. However! r28 is the result and will conflict with
   the clobber list so it is left out. Also the input arguments
   registers r20 -> r26 will conflict with the list so they
   are treated specially. Although r19 is clobbered by the syscall
   we cannot say this because it would violate ABI, thus we say
   r4 is clobbered and use that register to save/restore r19
   across the syscall. */

#define K_CALL_CLOB_REGS "%r1", "%r2", K_USING_GR4 \
			 "%r20", "%r29", "%r31"

#undef K_INLINE_SYSCALL
#define K_INLINE_SYSCALL(name, nr, args...)	({			\
	long __sys_res;							\
	{								\
		register unsigned long __res __asm__("r28");		\
		K_LOAD_ARGS_##nr(args)					\
		/* FIXME: HACK stw/ldw r19 around syscall */		\
		__asm__ volatile(					\
			K_STW_ASM_PIC					\
			"	ble  0x100(%%sr2, %%r0)\n"		\
			"	ldi %1, %%r20\n"			\
			K_LDW_ASM_PIC					\
			: "=r" (__res)					\
			: "i" (SYS_ify(name)) K_ASM_ARGS_##nr		\
			: "memory", K_CALL_CLOB_REGS K_CLOB_ARGS_##nr	\
		);							\
		__sys_res = (long)__res;				\
	}								\
	__sys_res;							\
})

#define K_LOAD_ARGS_0()
#define K_LOAD_ARGS_1(r26)					\
	register unsigned long __r26 __asm__("r26") = (unsigned long)(r26);   \
	K_LOAD_ARGS_0()
#define K_LOAD_ARGS_2(r26,r25)					\
	register unsigned long __r25 __asm__("r25") = (unsigned long)(r25);   \
	K_LOAD_ARGS_1(r26)
#define K_LOAD_ARGS_3(r26,r25,r24)				\
	register unsigned long __r24 __asm__("r24") = (unsigned long)(r24);   \
	K_LOAD_ARGS_2(r26,r25)
#define K_LOAD_ARGS_4(r26,r25,r24,r23)				\
	register unsigned long __r23 __asm__("r23") = (unsigned long)(r23);   \
	K_LOAD_ARGS_3(r26,r25,r24)
#define K_LOAD_ARGS_5(r26,r25,r24,r23,r22)			\
	register unsigned long __r22 __asm__("r22") = (unsigned long)(r22);   \
	K_LOAD_ARGS_4(r26,r25,r24,r23)
#define K_LOAD_ARGS_6(r26,r25,r24,r23,r22,r21)			\
	register unsigned long __r21 __asm__("r21") = (unsigned long)(r21);   \
	K_LOAD_ARGS_5(r26,r25,r24,r23,r22)

/* Even with zero args we use r20 for the syscall number */
#define K_ASM_ARGS_0
#define K_ASM_ARGS_1 K_ASM_ARGS_0, "r" (__r26)
#define K_ASM_ARGS_2 K_ASM_ARGS_1, "r" (__r25)
#define K_ASM_ARGS_3 K_ASM_ARGS_2, "r" (__r24)
#define K_ASM_ARGS_4 K_ASM_ARGS_3, "r" (__r23)
#define K_ASM_ARGS_5 K_ASM_ARGS_4, "r" (__r22)
#define K_ASM_ARGS_6 K_ASM_ARGS_5, "r" (__r21)

/* The registers not listed as inputs but clobbered */
#define K_CLOB_ARGS_6
#define K_CLOB_ARGS_5 K_CLOB_ARGS_6, "%r21"
#define K_CLOB_ARGS_4 K_CLOB_ARGS_5, "%r22"
#define K_CLOB_ARGS_3 K_CLOB_ARGS_4, "%r23"
#define K_CLOB_ARGS_2 K_CLOB_ARGS_3, "%r24"
#define K_CLOB_ARGS_1 K_CLOB_ARGS_2, "%r25"
#define K_CLOB_ARGS_0 K_CLOB_ARGS_1, "%r26"

#define io_syscall1(type,fname,sname,type1,arg1)			\
type fname(type1 arg1)							\
{									\
    return K_INLINE_SYSCALL(sname, 1, arg1);				\
}

#define io_syscall2(type,fname,sname,type1,arg1,type2,arg2)		\
type fname(type1 arg1, type2 arg2)					\
{									\
    return K_INLINE_SYSCALL(sname, 2, arg1, arg2);			\
}

#define io_syscall3(type,fname,sname,type1,arg1,type2,arg2,type3,arg3)	\
type fname(type1 arg1, type2 arg2, type3 arg3)				\
{									\
    return K_INLINE_SYSCALL(sname, 3, arg1, arg2, arg3);		\
}

#define io_syscall4(type,fname,sname,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type fname(type1 arg1, type2 arg2, type3 arg3, type4 arg4)		\
{									\
    return K_INLINE_SYSCALL(sname, 4, arg1, arg2, arg3, arg4);		\
}

#define io_syscall5(type,fname,sname,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
type fname(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5)	\
{									\
    return K_INLINE_SYSCALL(sname, 5, arg1, arg2, arg3, arg4, arg5);	\
}
