/**
 * Kernel module that communicates with /proc file system.
 * Modified by Camryn Friedman 11/30/2021
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128
#define LAB_NAME "LAB4"
#define PROC_NAME "myname"
#define your_name "Camryn Friedman"
#define FIRST_NAME "Camryn"
#define LAST_NAME "Friedman"

/*initialize global variable for call count*/
int call_num = 1;

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

/* Try to handle multiple kernels for students on older Ubuntu images, or M1 Macs.
 * If more or less up to date, use the  proc_ops         parameter to proc_create.
 * Otherwise, try to use the older      file_operations  parameter to proc_create. */

#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
const static struct proc_ops my_ops = {
    .proc_read = proc_read,
};
#else
static struct file_operations my_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
};
#endif

/* This function is called when the module is loaded. */
int proc_init(void)
{
        // YOUR CODE GOES HERE to create the "/proc/myname" entry
        proc_create(PROC_NAME, 0666, NULL, &my_ops);

        // YOUR CODE GOES HERE to print kernel message like: "LAB4:  Loading  module by Keith Shafer"
        printk(KERN_INFO "%s: Loading module by %s\n", LAB_NAME, your_name);

        return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void)
{

        // YOUR CODE GOES HERE to remove the "/proc/myname" entry
        remove_proc_entry(PROC_NAME, NULL);

        // YOUR CODE GOES HERE to print kernel message like: "LAB4:  Removing module by Keith Shafer"
        printk(KERN_INFO "%s: Removing module by %s\n", LAB_NAME, your_name);
}

/**
 * This function is called each time the /proc/myname is read.
 *
 * When a user triggers this call, the function is called repeatedly by the system until you return 0, so
 * be careful to return 0 when you know you're done.  Note that you have to return at least
 * once with a non-zero value to get your data back out to the user.
 *
 * If you get a loop at the command line when you 'cat /proc/myname', just hit Ctrl-C to break it.
 *
 * See the lab write-up for details on what your proc_read is supposed to output.
 */

ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{

        // YOUR CODE GOES HERE to implement read calls like 'cat /proc/myname'
        /* initialize variables */
        int rv = 0;
        char buffer[BUFFER_SIZE]; //buffer to hold printed message
        static int completed = 0; //acts as a boolean to denote when run is completed
        unsigned long raw_copy; //holds output of raw_copy_to_user

        if (completed)
        {
                completed = 0;
                return 0;
        }

        completed = 1;

        /*int sprintf(char *str, const char *format, ...)
        -returns  total num of characters written excluding the null-character
        appended at the end of the string
        -if failure, negative number returned*/

        /*when call number is even, return last name. else return first name.*/
        if (call_num % 2 == 0)
        {
                rv = sprintf(buffer, "%s: %s[%d] = %s\n", LAB_NAME, PROC_NAME, call_num, LAST_NAME);
        }
        else{
                rv = sprintf(buffer, "%s: %s[%d] = %s\n", LAB_NAME, PROC_NAME, call_num, FIRST_NAME);
        }

        /*copy kernel space buffer to user space buf*/
        raw_copy = raw_copy_to_user(buf, buffer, rv);

        /*increment call count*/
        call_num++;

        return rv;

}

/* Macros for registering module entry and exit points. */
module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module");
MODULE_AUTHOR("SGG-MORE");
