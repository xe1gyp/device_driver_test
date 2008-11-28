
/*
 *  Frame buffer control
 *
 *  (C) Copyright 2001-2003 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "page.h"

#include "types.h"
#include "fb.h"
#include "util.h"
#include "colormap.h"

#define PAGE_SHIFT              12
#define PAGE_SIZE               (1UL << PAGE_SHIFT)
#define PAGE_MASK               (~(PAGE_SIZE-1))

static int fb_fd = -1;

struct fb_fix_screeninfo fb_fix;
struct fb_var_screeninfo fb_var;
struct fb_cmap fb_cmap;

static unsigned long fb_start;
static u32 fb_len, fb_offset;
u8 *fb;


    /*
     *   Saved frame buffer device state
     */

static struct fb_var_screeninfo saved_var;
static struct fb_fix_screeninfo saved_fix;
static struct fb_cmap saved_cmap;
static u16 *saved_red, *saved_green, *saved_blue, *saved_transp;
static u8 *saved_fb;


static void fix_validate(void);
static void var_validate(void);
static void var_validate_change(const struct fb_var_screeninfo *old,
				int error);
static void cmap_validate(void);
static void cmap_validate_change(const struct fb_cmap *old, int error);
static void fb_dump_cmap(void);


    /*
     *  Open the frame buffer device
     */

void fb_open(void)
{
    Debug("fb_open()\n");
    if ((fb_fd = open(Opt_Fbdev, O_RDWR)) == -1) {
	Fatal("open %s: %s\n", Opt_Fbdev, strerror(errno));
    }
}


    /*
     *  Close the frame buffer device
     */

void fb_close(void)
{
    Debug("fb_close()\n");
    if (fb_fd != -1) {
	close(fb_fd);
	fb_fd = -1;
    }
}


    /*
     *  Get the fixed information about a frame buffer
     */

int fb_get_fix(void)
{
    Debug("fb_get_fix()\n");
    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_fix) == -1) {
	Fatal("ioctl FBIOGET_FSCREENINFO: %s\n", strerror(errno));
    }
    fix_validate();
    return 1;
}


    /*
     *  Get the variable information about a frame buffer
     */

int fb_get_var(void)
{
    Debug("fb_get_var()\n");
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fb_var) == -1) {
	Fatal("ioctl FBIOGET_VSCREENINFO: %s\n", strerror(errno));
    }
    var_validate();
    return 1;
}


    /*
     *  Set the variable information about a frame buffer
     */

int fb_set_var(void)
{
    struct fb_var_screeninfo var = fb_var;
    int error;

    Debug("fb_set_var()\n");
    error = ioctl(fb_fd, FBIOPUT_VSCREENINFO, &fb_var);
    var_validate_change(&var, error);
    if (error == -1) {
	Fatal("ioctl FBIOPUT_VSCREENINFO: %s\n", strerror(errno));
    }
    return 1;
}


    /*
     *  Get the colormap
     */

int fb_get_cmap(void)
{
    Debug("fb_get_cmap()\n");
    if (ioctl(fb_fd, FBIOGETCMAP, &fb_cmap) == -1) {
	Fatal("ioctl FBIOGETCMAP: %s\n", strerror(errno));
    }
    cmap_validate();
    if (Opt_Debug)
	fb_dump_cmap();
    return 1;
}


    /*
     *  Set the colormap
     */

int fb_set_cmap(void)
{
    struct fb_cmap cmap = fb_cmap;
    int error;

    Debug("fb_set_cmap()\n");
    if (Opt_Debug)
	fb_dump_cmap();
    error = ioctl(fb_fd, FBIOPUTCMAP, &fb_cmap);
    cmap_validate_change(&cmap, error);
    if (error == -1) {
	Fatal("ioctl FBIOPUTCMAP: %s\n", strerror(errno));
    }
    return 1;
}


    /*
     *  Map the frame buffer
     */

void fb_map(void)
{
    caddr_t addr;

    Debug("fb_map()\n");
    fb_start = (unsigned long)fb_fix.smem_start & PAGE_MASK;
    fb_offset = (unsigned long)fb_fix.smem_start & ~PAGE_MASK;
    fb_len = (fb_offset+fb_fix.smem_len+~PAGE_MASK) & PAGE_MASK;
    Debug("fb_start = %lx, fb_offset = %x, fb_len = %x\n", fb_start, fb_offset,
	  fb_len);
    addr = mmap(NULL, fb_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (addr == (caddr_t)-1)
	Fatal("mmap smem: %s\n", strerror(errno));
    fb = addr+fb_offset;
}


    /*
     *  Unmap the frame buffer
     */

void fb_unmap(void)
{
    Debug("fb_unmap()\n");
    if (munmap((caddr_t)((unsigned long)fb & PAGE_MASK), fb_len) == -1)
	Fatal("munmap smem: %s\n", strerror(errno));
}


    /*
     *  Save the frame buffer contents
     */

void fb_save(void)
{
    Debug("fb_save()\n");
    if (!(saved_fb = malloc(fb_fix.smem_len)))
	Fatal("malloc %d: %s\n", fb_fix.smem_len, strerror(errno));
    memcpy(saved_fb, fb, fb_fix.smem_len);
}


    /*
     *  Restore the frame buffer contents
     */

void fb_restore(void)
{
    Debug("fb_restore()\n");
    memcpy(fb, saved_fb, fb_fix.smem_len);
    free(saved_fb);
}


    /*
     *  Clear the frame buffer
     *
     *  We can't use memset(), because on PPC it uses dcbz, which is not
     *  allowed on non-cacheable memory :-(
     */

void fb_clear(void)
{
    int size = fb_fix.smem_len/sizeof(u32);
    u32 *p = (u32 *)fb;

    Debug("fb_clear()\n");
    while (size--)
	*p++ = 0;
}


    /*
     *  Validate the fixed information about a frame buffer
     */

static void fix_validate(void)
{
    /* FIXME: check for impossible values */
}


    /*
     *  Validate the variable information about a frame buffer
     */

static void var_validate(void)
{
    /* FIXME: check for impossible values */
}


    /*
     *  Validate a change of the variable information about a frame buffer
     */

#define CHECK_CHANGE(x)							\
    do {								\
	if (fb_var.x != old->x) {					\
	    if (error == -1)						\
		Error(#x "changed from %u to %u\n", old->x, fb_var.x);	\
	    else							\
		Warning(#x "changed from %u to %u\n", old->x, fb_var.x);\
	}								\
    } while (0)

#define CHECK_ROUNDING(x)			\
    do {					\
	if (fb_var.x < old->x)			\
	    Error(#x " was rounded down\n");	\
    } while (0)

#define CHECK_CHANGE_AND_ROUNDING(x)	\
    do {				\
	CHECK_CHANGE(x);		\
	if (error != -1)		\
	    CHECK_ROUNDING(x);		\
    } while (0)

static void var_validate_change(const struct fb_var_screeninfo *old, int error)
{
    CHECK_CHANGE_AND_ROUNDING(xres);
    CHECK_CHANGE_AND_ROUNDING(yres);
    CHECK_CHANGE_AND_ROUNDING(xres_virtual);
    CHECK_CHANGE_AND_ROUNDING(yres_virtual);
    CHECK_CHANGE_AND_ROUNDING(xoffset);
    CHECK_CHANGE_AND_ROUNDING(yoffset);
    CHECK_CHANGE_AND_ROUNDING(bits_per_pixel);
    CHECK_CHANGE(grayscale);
    CHECK_CHANGE_AND_ROUNDING(red.offset);
    CHECK_CHANGE_AND_ROUNDING(red.length);
    CHECK_CHANGE(red.msb_right);
    CHECK_CHANGE_AND_ROUNDING(green.offset);
    CHECK_CHANGE_AND_ROUNDING(green.length);
    CHECK_CHANGE_AND_ROUNDING(green.length);
    CHECK_CHANGE(green.msb_right);
    CHECK_CHANGE_AND_ROUNDING(blue.offset);
    CHECK_CHANGE_AND_ROUNDING(blue.length);
    CHECK_CHANGE_AND_ROUNDING(blue.length);
    CHECK_CHANGE(blue.msb_right);
    CHECK_CHANGE_AND_ROUNDING(transp.offset);
    CHECK_CHANGE_AND_ROUNDING(transp.length);
    CHECK_CHANGE_AND_ROUNDING(transp.length);
    CHECK_CHANGE(transp.msb_right);
    CHECK_CHANGE(nonstd);
    CHECK_CHANGE(activate);
    CHECK_CHANGE_AND_ROUNDING(height);
    CHECK_CHANGE_AND_ROUNDING(width);
    CHECK_CHANGE(accel_flags);
    CHECK_CHANGE_AND_ROUNDING(pixclock);
    CHECK_CHANGE_AND_ROUNDING(left_margin);
    CHECK_CHANGE_AND_ROUNDING(right_margin);
    CHECK_CHANGE_AND_ROUNDING(upper_margin);
    CHECK_CHANGE_AND_ROUNDING(lower_margin);
    CHECK_CHANGE_AND_ROUNDING(hsync_len);
    CHECK_CHANGE_AND_ROUNDING(vsync_len);
    CHECK_CHANGE(sync);
    CHECK_CHANGE(vmode);
    CHECK_CHANGE(reserved[0]);
    CHECK_CHANGE(reserved[1]);
    CHECK_CHANGE(reserved[2]);
    CHECK_CHANGE(reserved[3]);
    CHECK_CHANGE(reserved[4]);
    CHECK_CHANGE(reserved[5]);

    var_validate();
}

#undef CHECK_CHANGE
#undef CHECK_ROUNDING
#undef CHECK_CHANGE_AND_ROUNDING


    /*
     *  Validate the colormap
     */

static void cmap_validate(void)
{
    /* FIXME */
}


    /*
     *  Validate a change of the colormap
     */

static void cmap_validate_change(const struct fb_cmap *old, int error)
{
    /* FIXME */

    cmap_validate();
}


    /*
     *  Validate the variable and fixed information about a frame buffer
     */

static void var_fix_validate(void)
{
    /* FIXME: check for impossible combinations */
}


    /*
     *  Initialization
     */

#define ALLOC_AND_SAVE_COMPONENT(name)					\
    do {								\
	if (!(saved_ ## name = malloc(fb_cmap.len*sizeof(u16))))	\
	    Fatal("malloc %d: %s\n", fb_cmap.len*sizeof(u16),		\
		  strerror(errno));					\
	memcpy(saved_ ## name, fb_cmap.name, fb_cmap.len*sizeof(u16));	\
    } while (0)

void fb_init(void)
{
    Debug("fb_init()\n");
    fb_open();
    fb_get_var();
    memcpy(&saved_var, &fb_var, sizeof(struct fb_var_screeninfo));
    //saved_var = fb_var;
    if (fb_var.xoffset || fb_var.yoffset || fb_var.accel_flags) {
	fb_var.xoffset = 0;
	fb_var.yoffset = 0;
	fb_var.accel_flags = 0;
	fb_set_var();
    }
    fb_get_fix();
    var_fix_validate();
    memcpy(&saved_fix, &fb_fix, sizeof(struct fb_var_screeninfo));
    //saved_fix = fb_fix;
    switch (fb_fix.visual) {
	case FB_VISUAL_MONO01:
	case FB_VISUAL_MONO10:
	case FB_VISUAL_TRUECOLOR:
	    /* no colormap */
	    break;

	case FB_VISUAL_PSEUDOCOLOR:
	case FB_VISUAL_STATIC_PSEUDOCOLOR:
	    cmap_init(1<<fb_var.bits_per_pixel);
	    break;

	case FB_VISUAL_DIRECTCOLOR:
	    cmap_init(1<<(max(max(fb_var.red.length, fb_var.green.length),
			      max(fb_var.blue.length, fb_var.transp.length))));
	    break;
    }
    if (fb_cmap.len) {
	fb_get_cmap();
	saved_cmap = fb_cmap;
	ALLOC_AND_SAVE_COMPONENT(red);
	ALLOC_AND_SAVE_COMPONENT(green);
	ALLOC_AND_SAVE_COMPONENT(blue);
	if (fb_cmap.transp)
	    ALLOC_AND_SAVE_COMPONENT(transp);
    }
    fb_map();
    fb_save();
    fb_clear();
}

#undef ALLOC_AND_SAVE_COMPONENT


    /*
     *   Clean up
     */

#define RESTORE_AND_FREE_COMPONENT(name)				\
    do {								\
	memcpy(fb_cmap.name, saved_ ## name, fb_cmap.len*sizeof(u16));	\
	free(saved_ ## name);						\
    } while (0)

void fb_cleanup(void)
{
    Debug("fb_cleanup()\n");
    if (saved_fb)
	fb_restore();
    if (fb)
	fb_unmap();
    if (fb_fd != -1) {
	if (saved_cmap.len) {
	    fb_cmap = saved_cmap;
	    RESTORE_AND_FREE_COMPONENT(red);
	    RESTORE_AND_FREE_COMPONENT(green);
	    RESTORE_AND_FREE_COMPONENT(blue);
	    if (fb_cmap.transp)
		RESTORE_AND_FREE_COMPONENT(transp);
	    fb_set_cmap();
	}
	//fb_var = saved_var;
	memcpy(&fb_var, &saved_var, sizeof(struct fb_var_screeninfo));
	fb_set_var();
	fb_get_var();
	/* FIXME: compare fb_var with saved_var */
	fb_get_fix();
	/* FIXME: compare fb_fix with saved_fix */
	fb_close();
    }
}

#undef RESTORE_AND_FREE_COMPONENT


    /*
     *  Dump the colormap
     */

static void fb_dump_cmap(void)
{
    int i;

    Debug("Colormap start = %d len = %d\n", fb_cmap.start, fb_cmap.len);
    for (i = 0; i < fb_cmap.len; i++)
	Debug("%4d: R %04x G %04x B %04x A %04x\n", i, fb_cmap.red[i],
	      fb_cmap.green[i], fb_cmap.blue[i], fb_cmap.transp[i]);
}

