#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_DESCRIPTION("fast_io_module");
MODULE_AUTHOR("Me");
MODULE_LICENSE("GPL");

extern int fast_io_dummy_init(void);
extern void fast_io_dummy_exit(void);

static int dummy_init(void)
{
        return fast_io_dummy_init();
}

static void dummy_exit(void)
{
	fast_io_dummy_exit();
}

module_init(dummy_init);
module_exit(dummy_exit);