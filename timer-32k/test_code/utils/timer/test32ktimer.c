


#include <linux/module.h>
#include <linux/clocksource.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <asm/uaccess.h>        // get_fs(), set_fs(), KERNEL_DS
#include <linux/file.h>         // fput()
#include <linux/mm.h>           // GFP_KERNEL

#include <linux/fcntl.h>
#include <linux/fs.h>


//#include <drivers/misc/omap_timer32k.h>
#define TIMER32K_COUNTER_READ omap_32k_read()
//#define TIMER32K_COUNTER_READ

int write_to_file(struct file *f, char *buffer, int pos);
extern void omap_wait_for_32kclock_edge(void);
extern unsigned int omap_32k_read(void);
static int __init timertest_init(void)
{
    int i;
	struct file *f = NULL;
	mm_segment_t orig_fs;
	int pos;
	char buffer[100];
	/* Prepare for file operation */
        f = filp_open("/timer.msg", O_WRONLY | O_CREAT, 0600);
        if (!f || !f->f_op || !f->f_op->write){
                printk("WARNING: File (write) object is a null pointer!!!\n");
        }
        f->f_pos = 0;
        pos = 0;
        orig_fs = get_fs();
	set_fs(KERNEL_DS);
    for(i=0;i<20;i++){
        omap_wait_for_32kclock_edge();
        printk("%d. CR=%x\n",i,TIMER32K_COUNTER_READ);
		sprintf(buffer, "%x\n", TIMER32K_COUNTER_READ);
		pos = write_to_file(f, buffer, pos);
    }
	return 0;
}
static void __exit timertest_exit(void)
{
}
// Write to file
int write_to_file(struct file *f, char *buffer, int pos) {
        int i, len, bufsize=0;
        const int MAXBUFSIZE=100;

        for (i=0; i<MAXBUFSIZE; i++) {
                if (buffer[i]=='\0')
                        break;
                bufsize++;
        }

        f->f_pos = pos;
        len = f->f_op->write(f, buffer, bufsize, &f->f_pos);

        return pos+len;
}
module_init(timertest_init);
module_exit(timertest_exit);

MODULE_LICENSE("GPL");

