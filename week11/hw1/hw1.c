#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define devName "hw1"
#define devNum 300

char initFlag = 0;
char data[4] = {'0',};

int hw1_open(struct inode *inode, struct file *flie)
{
	printk("hw1 open func called!\n");
	
	return 0;
}

int hw1_read(struct file *pfile, char *pbuff, size_t size, loff_t *filepos)
{
	printk("hw1_read func called!\n");

	if (size >= 4)
	{
		copy_to_user(pbuff, data, 4);
		return 4;
	}
	
	return 0;
}

int hw1_release(struct inode *inode, struct file *flip)
{
	printk("hw1_release func called!\n");
	return 0;
}

int hw1_write(struct file *pfile, const char *pBuff, size_t size, loff_t *filepos)
{
	printk("hw1_write func called!\n");
	if(size >= 4)
	{
		copy_from_user(data, pBuff, 4);
		printk("user to driver is: %c%c%c%c\n", data[0],data[1],data[2],data[3]);
		return 4;
	}

	return 0;
}

struct file_operations fops= {
	.owner = THIS_MODULE,
	.open = hw1_open,
	.read = hw1_read,
	.write = hw1_write,
	.release = hw1_release,
};

int test_init(void)
{
	if((register_chrdev(devNum, devName, &fops))<0)
	{
		printk("Driver register failed!\n");
	}
	initFlag = 1;
	printk("Driver register succed!\n");
	return 0;
}

void test_exit(void)
{
	if(initFlag == 1)
	{
		printk("Driver unregister succed!\n");
		unregister_chrdev(devNum, devName);
	}
	else printk("Driver unregister failed!\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
