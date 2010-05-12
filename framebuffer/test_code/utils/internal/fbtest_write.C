#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <math.h>

void myerror(char * message) {
	fprintf(stderr,"%s\n",message);
	exit(1);
}

void myperror(char * message) {
	perror(message);
	exit(1);
}

unsigned int * framebuffer;
int fbfd;
int c,d;

const int width=1024;
const int height=768;

void line(int x1,int y1,int x2,int y2,int color) {
	int x,y,dx,dy,halfdx;
	int inc;
	unsigned int * pos;
	int halfd;
	int d=0;

	dx = x2-x1; if(dx<0) dx = -dx;
	dy = y2-y1; if(dy<0) dy = -dy;
	if(dx>dy) {
		/* draw line horizontally */
		if(x2<x1) {
			/* ensure we're drawing left to right */
			int t;
			t = x1; x1 = x2; x2 = t;
			t = y1; y1 = y2; y2 = t;
		}
		halfd = dx>>1;
		x = x1; y = y1;
		pos = framebuffer + x + width*y;
		inc = (y2>y1)?width:-width;
		while(1) {
			*pos = color;
			if(x == x2) return;
			x++; pos++;
			d+=dy; if(d>halfd) {
				d-=dx;
				pos += inc;
			}
		}
	} else {
		/* draw line vertically */
		if(y2<y1) {
			/* ensure we're drawing downwards */
			int t;
			t = x1; x1 = x2; x2 = t;
			t = y1; y1 = y2; y2 = t;
		}
		halfd = dy>>1;
		x = x1; y = y1;
		pos = framebuffer + x + width*y;
		inc = (x2>x1)?1:-1;
		while(1) {
			*pos = color;
			if(y == y2) return;
			y++; pos+=width;
			d+=dx; if(d>halfd) {
				d-=dy;
				pos += inc;
			}
		}
	}
}

struct point {
	int x,y;
	point(int px,int py):x(px),y(py){}
	point(){}
	point operator+(point rhs) {return point(x+rhs.x,y+rhs.y);}
	point operator+=(point rhs) {*this = *this + rhs;}
};

inline void line(point p1,point p2,int color) {
	line(p1.x,p1.y,p2.x,p2.y,color);
}

float pi = 3.141592654;

void circle(int x,int y,int xrad,int yrad,int color) {
	int x1,y1,x2,y2;
	float angle=0;
	int step;

	x1=x;y1=y-yrad;

	do {
		angle += pi/180;
		x2 = lround(x+xrad*sin(angle));
		y2 = lround(y-yrad*cos(angle));
//		if(!(++step &0x7))
			line(x1,y1,x2,y2,color);
		x1 = x2; y1 = y2;
	} while(angle<2*pi);
}


#define TAIL_LENGTH 100

point p1tail[TAIL_LENGTH];
point p2tail[TAIL_LENGTH];

point p1,p2;
point p1delta,p2delta;

int main(void) {
	/* open the framebuffer device */
	if((fbfd = open("/dev/fb0",O_RDWR)) == -1) myperror("while opening /dev/fb0");
	/* map it to memory. */
	/* note for non-kernel hackers: you might thing that mapping a
	   memory-mapped block device to memory would actually just cause an extra
	   layer of frust to be added.  This is not true; linux allows this to be
	   optimised, and the framebuffer device does so.
	*/
	if((framebuffer = (unsigned int *)mmap(0,1024*768*4,PROT_READ|PROT_WRITE,MAP_SHARED,fbfd,0)) == MAP_FAILED)
		myperror("while mmapping framebuffer");

	srand(time(NULL));

	/* set up the tail and deltas and such. */
	for(int c=0;c<TAIL_LENGTH;c++) {
		p1tail[c] = point(0,0);
		p2tail[c] = point(0,0);
	}
	p1 = point(rand()%1024,rand()%768);
	p2 = point(rand()%1024,rand()%768);
	p1delta = point(rand()%19-10,rand()%19-10);
	p2delta = point(rand()%19-10,rand()%19-10);

	/* main loop */
	int tailpos = 0;
	while(1) {
		p1+=p1delta; p2+=p2delta;
		if(p1.x<0 || p1.x>=width ) {p1.x -= p1delta.x*2; p1delta.x =- p1delta.x;}
		if(p1.y<0 || p1.y>=height) {p1.y -= p1delta.y*2; p1delta.y =- p1delta.y;}
		if(p2.x<0 || p2.x>=width ) {p2.x -= p2delta.x*2; p2delta.x =- p2delta.x;}
		if(p2.y<0 || p2.y>=height) {p2.y -= p2delta.y*2; p2delta.y =- p2delta.y;}

		line(p1,p2,rand());
		line(p1tail[tailpos],p2tail[tailpos],0);
		p1tail[tailpos] = p1; p2tail[tailpos] = p2;
		tailpos = (tailpos+1)%TAIL_LENGTH;
		usleep(100);
	}
}
