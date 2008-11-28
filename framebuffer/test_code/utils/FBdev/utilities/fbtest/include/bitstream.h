
/*
 *  Bitstream operations
 *
 *  (C) Copyright 2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


    /*
     *  Forward and reverse copy
     */

extern void bitcpy(unsigned long *dst, int dst_idx, const unsigned long *src,
		   int src_idx, u32 n);
extern void bitcpy_rev(unsigned long *dst, int dst_idx,
		       const unsigned long *src, int src_idx, u32 n);


    /*
     *  Forward inverting copy
     */

extern void bitcpy_not(unsigned long *dst, int dst_idx,
		       const unsigned long *src, int src_idx, u32 n);


    /*
     *   Pattern fill
     */

extern void bitfill32(unsigned long *dst, int dst_idx, u32 pat, u32 n);
extern void bitfill(unsigned long *dst, int dst_idx, unsigned long pat,
		    int left, int right, u32 n);

