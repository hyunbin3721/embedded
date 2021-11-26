#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define devName "hw2"
#define devNum 400
#define MagicNum 0x55

char initFlag = 0;
static char array[2000] ={0,};

int hw2_open(struct inode *inode, struct file *flie)
{
	printk("hw2_open func called!\n");
	
	return 0;
}

int hw2_read(struct file *pfile, char *pbuff, size_t size, loff_t *filepos)
{
	printk("hw2_read func called!\n");
	copy_to_user(pbuff, array, size);
	return size;
	
	return 0;
}

int hw2_release(struct inode *inode, struct file *flip)
{
	printk("hw2_release func called!\n");
	return 0;
}

int hw2_write(struct file *pfile, const char *pBuff, size_t size, loff_t *filepos)
{
	int count = 0;
	printk("hw2_write func called!\n");

	if(size <= 2000)
	{
		copy_from_user(array, pBuff, size);
		return size;
	}
	else printk("writing buffs size is not enough!");
	
	return 0;
}

static long hw2_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg)
{
	int cnt = 0; int k = 0; unsigned int typeNr = 0; unsigned int type = 0; unsigned int nr = 0;
	
	if(_IOC_TYPE(cmd)!=MagicNum)
	{
		printk("Wrong MagicNumber!\n");
		return -1;
	}
	
	switch(_IOC_NR(cmd))
	{
		case 99:type = _IOC_TYPE(cmd); nr = _IOC_NR(cmd);
				type = type << 8;
				typeNr = type | nr;
				printk("Received CMD: %d(%x)\n",typeNr, typeNr);
				printk("IOCTL size: %d\n",_IOC_SIZE(cmd));
				break;

		case 98:type = _IOC_TYPE(cmd); nr = _IOC_NR(cmd);
				type = type << 8;
				typeNr = type | nr;
				printk("Received CMD: %d(%x)\n",typeNr, typeNr);
				printk("IOCTL size: %d\n",_IOC_SIZE(cmd));
				break;
				
		case 97:copy_from_user(&cnt, arg, _IOC_SIZE(cmd));
				for(k=0; k<cnt;k++)
				{
					printk("%d", array[k]-'0');
				}
				printk("\n");
				break;
				
		default:type = _IOC_TYPE(cmd); nr = _IOC_NR(cmd);
				 type = type << 8;
				 typeNr = type | nr;
				 printk("Received CMD: %d(%x)\n",typeNr, typeNr);
				 break;
	}
	
}
struct file_operations fops= {
	.owner = THIS_MODULE,
	.open = hw2_open,
	.read = hw2_read,
	.write = hw2_write,
	.release = hw2_release,
	.unlocked_ioctl = hw2_ioctl,
};

int hw2_init(void)
{
	if((register_chrdev(devNum, devName, &fops))<0)
	{
		printk("Driver register failed!\n");
	}
	initFlag = 1;
	printk("Driver register succed!\n");
	return 0;
}

void hw2_exit(void)
{
	if(initFlag == 1)
	{
		printk("Driver unregister succed!\n");
		unregister_chrdev(devNum, devName);
	}
	else printk("Driver unregister failed!\n");
}

module_init(hw2_init);
module_exit(hw2_exit);

MODULE_LICENSE("GPL");
