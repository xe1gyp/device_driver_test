/* 
 * common.h
 * 
 * This program plays to the audio device with variable options
 * This requires a RAW input file.
 *
 * Copyright (C) 2006 Texas Instruments, Inc. 
 */
#ifndef __COMMON_HEADER__
#define __COMMON_HEADER__

/* Borrowed from http://www.dtek.chalmers.se/groups/dvd/dist/oss_audio.c */
/* AFMT_AC3 is really IEC61937 / IEC60958, mpeg/ac3/dts over spdif */
#ifndef AFMT_AC3
#define AFMT_AC3        0x00000400      /* Dolby Digital AC3 */
#endif
#ifndef AFMT_S32_LE
#define AFMT_S32_LE     0x00001000  /* 32/24-bits, in 24bit use the msbs */ 
#endif
#ifndef AFMT_S32_BE
#define AFMT_S32_BE     0x00002000  /* 32/24-bits, in 24bit use the msbs */ 
#endif

#endif /* #ifndef __COMMON_HEADER__ */
