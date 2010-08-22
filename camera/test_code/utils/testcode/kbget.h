/* These two pieces of code originally come from:
 * Lee Hollingworth:
 *   http://c-faq.com/osdep/kbhit.txt
 * Floyd L. Davidson:
 *   http://kasperd.net/~kasperd/comp.os.linux.development/ncurses2.txt
 *
 * Both authors have expressed their consent to use this code freely.
 */

#include <unistd.h>	/* read() */
#include <fcntl.h>	/* setting keyboard flags */
#include <sys/ioctl.h>
#include <termio.h>	/* used to set terminal modes */
#include <termios.h>

/*
 * two global variables for tty and keybrd control
 */
static struct termio term_orig;
static int kbdflgs;

/*
 * function :   system_mode
 * purpose  :   reset the system to what it was before input_mode was
 *              called
 */
void system_mode(void)
{
	if (ioctl(0, TCSETA, &term_orig) == -1)
		return;

	fcntl(0, F_SETFL, kbdflgs);
}

/*
 * function :   input_mode
 * purpose  :   set the system into raw mode for keyboard i/o
 * returns  :   0 - error
 *              1 - no error
 */
int input_mode(void)
{
	struct termio term;    /* to avoid ^S ^Q processing */

	/*
	* get rid of XON/XOFF handling, echo, and other input processing
	*/
	if (ioctl(0, TCGETA, &term) == -1)
		return 0;

	(void)ioctl(0, TCGETA, &term_orig);
	term.c_iflag = 0;
	term.c_oflag = 0;
	term.c_lflag = 0;
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	if (ioctl(0, TCSETA, &term) == -1)
		return 0;
	kbdflgs = fcntl(0, F_GETFL, 0);
	/*
	* no delay on reading stdin
	*/
	int flags = fcntl(0, F_GETFL);
	flags &= ~O_NDELAY;
	fcntl(0, F_SETFL, flags);
	return 1;
}

/*
 * function :   getch
 * purpose  :   read a single character from the keyboard without echo
 * returns  :   the keybress character
 */
int getch(void)
{
	unsigned char ch;
	int ret;

	input_mode();

	do
		ret = read(0, &ch, 1);
	while (ret != 1);

	system_mode();

	return ch;
}

int kbhit(void)
{
	int cnt = 0;
	int error;
	static struct termios otty, ntty;

	tcgetattr(0, &otty);
	ntty = otty;

	ntty.c_iflag = 0;       /* input mode */
	ntty.c_oflag = 0;       /* output mode */
	ntty.c_lflag &= ~ICANON; /* raw mode */
	ntty.c_cc[VMIN] = CMIN;    /* minimum time to wait */
	ntty.c_cc[VTIME] = CTIME;   /* minimum characters to wait for */

	error = tcsetattr(0, TCSANOW, &ntty);
	if (error == 0) {
		error += ioctl(0, FIONREAD, &cnt);
		error += tcsetattr(0, TCSANOW, &otty);
	}

	return (error == 0 ? cnt : -1);
}
