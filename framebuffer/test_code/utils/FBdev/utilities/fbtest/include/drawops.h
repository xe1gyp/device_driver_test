
/*
 *  Low-level drawing operations
 *
 *  These operations control the drawing of pixel values to the frame buffer
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


struct drawops {
    const char *name;
    int (*init)(void);
    void (*set_pixel)(u32 x, u32 y, pixel_t pixel);
    pixel_t (*get_pixel)(u32 x, u32 y);
    void (*draw_hline)(u32 x, u32 y, u32 length, pixel_t pixel);
    void (*draw_vline)(u32 x, u32 y, u32 length, pixel_t pixel);
    void (*draw_rect)(u32 x, u32 y, u32 width, u32 height, pixel_t pixel);
    void (*fill_rect)(u32 x, u32 y, u32 width, u32 height, pixel_t pixel);
    void (*draw_line)(u32 x1, u32 y1, u32 x2, u32 y2, pixel_t pixel);
    void (*expand_bitmap)(u32 x, u32 y, u32 width, u32 height, const u8 *data,
			  u32 pitch, pixel_t pixel0, pixel_t pixel1);
    void (*draw_pixmap)(u32 x, u32 y, u32 width, u32 height,
			  const pixel_t *pixmap);
    void (*draw_circle)(u32 x, u32 y, u32 r, pixel_t pixel);
    void (*fill_circle)(u32 x, u32 y, u32 r, pixel_t pixel);
    void (*draw_ellipse)(u32 x, u32 y, u32 a, u32 b, pixel_t pixel);
    void (*fill_ellipse)(u32 x, u32 y, u32 a, u32 b, pixel_t pixel);
    void (*copy_rect)(u32 dx, u32 dy, u32 width, u32 height, u32 sx, u32 sy);
    /* FIXME: text */
};

extern struct drawops drawops;


#define set_pixel(x, y, pixel)	drawops.set_pixel((x), (y), (pixel))
#define get_pixel(x, y)	drawops.get_pixel((x), (y))
#define draw_hline(x, y, length, pixel)	\
    drawops.draw_hline((x), (y), (length), (pixel))
#define draw_vline(x, y, length, pixel)	\
    drawops.draw_vline((x), (y), (length), (pixel))
#define draw_rect(x, y, width, height, pixel)	\
    drawops.draw_rect((x), (y), (width), (height), (pixel))
#define fill_rect(x, y, width, height, pixel)	\
    drawops.fill_rect((x), (y), (width), (height), (pixel))
#define draw_line(x1, y1, x2, y2)	\
    drawops.draw_line((x), (y), (x2), (y2))
#define expand_bitmap(x, y, width, height, data, pitch, pixel0, pixel1)	\
    drawops.expand_bitmap((x), (y), (width), (height), (data), (pitch),	\
			  (pixel0), (pixel1))
#define draw_pixmap(x, y, width, height, pixmap)	\
    drawops.draw_pixmap((x), (y), (width), (height), (pixmap))
#define draw_circle(x, y, r, pixel)	\
    drawops.draw_circle((x), (y), (r), (pixel))
#define fill_circle(x, y, r, pixel)	\
    drawops.fill_circle((x), (y), (r), (pixel))
#define draw_ellipse(x, y, a, b, pixel)	\
    drawops.draw_ellipse((x), (y), (a), (b), (pixel))
#define fill_ellipse(x, y, a, b, pixel)	\
    drawops.fill_ellipse((x), (y), (a), (b), (pixel))
#define copy_rect(dx, dy, width, height, sx, sy)	\
    drawops.copy_rect((dx), (dy), (width), (height), (sx), (sy))


    /*
     *  Frame buffer organization specific drawing operations
     */

extern const struct drawops cfb2_drawops;
extern const struct drawops cfb4_drawops;
extern const struct drawops cfb8_drawops;
extern const struct drawops cfb16_drawops;
extern const struct drawops cfb24_drawops;
extern const struct drawops cfb32_drawops;
extern const struct drawops planar_drawops;
extern const struct drawops iplan2_drawops;


    /*
     *  Generic drawing operations
     */

extern void generic_draw_hline(u32 x, u32 y, u32 length, pixel_t pixel);
extern void generic_draw_vline(u32 x, u32 y, u32 length, pixel_t pixel);
extern void generic_draw_rect(u32 x, u32 y, u32 width, u32 height,
			      pixel_t pixel);
extern void generic_fill_rect(u32 x, u32 y, u32 width, u32 height,
			      pixel_t pixel);
extern void generic_draw_line(u32 x1, u32 y1, u32 x2, u32 y2, pixel_t pixel);
extern void generic_expand_bitmap(u32 x, u32 y, u32 width, u32 height,
				  const u8 *data, u32 pitch, pixel_t pixel0,
				  pixel_t pixel1);
extern void generic_draw_pixmap(u32 x, u32 y, u32 width, u32 height,
				const pixel_t *data);
extern void generic_draw_circle(u32 x, u32 y, u32 r, pixel_t pixel);
extern void generic_fill_circle(u32 x, u32 y, u32 r, pixel_t pixel);
extern void generic_draw_ellipse(u32 x, u32 y, u32 a, u32 b, pixel_t pixel);
extern void generic_fill_ellipse(u32 x, u32 y, u32 a, u32 b, pixel_t pixel);
extern void generic_copy_rect(u32 dx, u32 dy, u32 width, u32 height, u32 sx,
			      u32 sy);


    /*
     *  Generic CFB drawing routines
     */

extern int cfb_init(void);
extern void cfb_draw_hline(u32 x, u32 y, u32 length, pixel_t pixel);
extern void cfb_fill_rect(u32 x, u32 y, u32 width, u32 height, pixel_t pixel);
extern void cfb_copy_rect(u32 dx, u32 dy, u32 width, u32 height, u32 sx,
			  u32 sy);


    /*
     *  Initialization
     */

extern void drawops_init(void);

