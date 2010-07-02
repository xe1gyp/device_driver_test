#ifndef _APP_OMAP3_TYPES_H
#define _APP_OMAP3_TYPES_H

#ifndef _packed
 #define __packed __attribute__((__packed__))
#endif

/*
 * ISP memory page size
 */
#ifndef PAGE_SIZE
 #define PAGE_SIZE 4096
#endif

/*
 * get CPU native databus width
 */
#ifndef CPUNATIVE
 #define CPUNATIVE (sizeof(int) * 8)
#endif

#ifndef COUNT_OF
 #define COUNT_OF(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

/*
 * fast alignement when alignement size is power of 2
 */
#ifndef ALIGN
 #define ALIGN(val, asize) \
	(asize + (((unsigned long)val - 1) & ~(asize - 1)))
#endif

#ifndef typeof
 #define typeof __typeof__
#endif

#ifndef min
 #define min(x, y) ({			\
	typeof(x) _min1 = (x);		\
	typeof(y) _min2 = (y);		\
	(void) (&_min1 == &_min2);	\
	_min1 < _min2 ? _min1 : _min2; })
#endif

#ifndef max
 #define max(x, y) ({			\
	typeof(x) _max1 = (x);		\
	typeof(y) _max2 = (y);		\
	(void) (&_max1 == &_max2);	\
	_max1 > _max2 ? _max1 : _max2; })
#endif

#endif
