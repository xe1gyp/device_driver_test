#define __NR_io_setup		241
#define __NR_io_destroy		242
#define __NR_io_getevents	243
#define __NR_io_submit		244
#define __NR_io_cancel		245

#define io_syscall1(type,fname,sname,atype,a) \
type fname(atype a) \
{ \
register long __res __asm__ ("%d0") = __NR_##sname; \
register long __a __asm__ ("%d1") = (long)(a); \
__asm__ __volatile__ ("trap  #0" \
		      : "+d" (__res) \
		      : "d" (__a)  ); \
return (type) __res; \
}

#define io_syscall2(type,fname,sname,atype,a,btype,b) \
type fname(atype a,btype b) \
{ \
register long __res __asm__ ("%d0") = __NR_##sname; \
register long __a __asm__ ("%d1") = (long)(a); \
register long __b __asm__ ("%d2") = (long)(b); \
__asm__ __volatile__ ("trap  #0" \
		      : "+d" (__res) \
		      : "d" (__a), "d" (__b) \
		     ); \
return (type) __res; \
}

#define io_syscall3(type,fname,sname,atype,a,btype,b,ctype,c) \
type fname(atype a,btype b,ctype c) \
{ \
register long __res __asm__ ("%d0") = __NR_##sname; \
register long __a __asm__ ("%d1") = (long)(a); \
register long __b __asm__ ("%d2") = (long)(b); \
register long __c __asm__ ("%d3") = (long)(c); \
__asm__ __volatile__ ("trap  #0" \
		      : "+d" (__res) \
		      : "d" (__a), "d" (__b), \
			"d" (__c) \
		     ); \
return (type) __res; \
}

#define io_syscall4(type,fname,sname,atype,a,btype,b,ctype,c,dtype,d) \
type fname (atype a, btype b, ctype c, dtype d) \
{ \
register long __res __asm__ ("%d0") = __NR_##sname; \
register long __a __asm__ ("%d1") = (long)(a); \
register long __b __asm__ ("%d2") = (long)(b); \
register long __c __asm__ ("%d3") = (long)(c); \
register long __d __asm__ ("%d4") = (long)(d); \
__asm__ __volatile__ ("trap  #0" \
		      : "+d" (__res) \
		      : "d" (__a), "d" (__b), \
			"d" (__c), "d" (__d)  \
		     ); \
return (type) __res; \
}

#define io_syscall5(type,fname,sname,atype,a,btype,b,ctype,c,dtype,d,etype,e) \
type fname (atype a,btype b,ctype c,dtype d,etype e) \
{ \
register long __res __asm__ ("%d0") = __NR_##sname; \
register long __a __asm__ ("%d1") = (long)(a); \
register long __b __asm__ ("%d2") = (long)(b); \
register long __c __asm__ ("%d3") = (long)(c); \
register long __d __asm__ ("%d4") = (long)(d); \
register long __e __asm__ ("%d5") = (long)(e); \
__asm__ __volatile__ ("trap  #0" \
		      : "+d" (__res) \
		      : "d" (__a), "d" (__b), \
			"d" (__c), "d" (__d), "d" (__e)  \
		     ); \
return (type) __res; \
}
