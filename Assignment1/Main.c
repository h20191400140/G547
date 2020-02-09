#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/random.h>
#include<linux/uaccess.h>
#include"adc8.h"
			
#define MAJOR_NO 200  
#define MINOR_NO 1


static dev_t dev_nmbr;
static struct class *cls_cr;
static struct cdev c_dev;

int adc_val(void)
{
   int adc2;
   adc2=get_random_int()%1024;
   return adc2;
}
ssize_t my_read (struct file *pfile, char __user *ioctl_param, size_t length, loff_t *poffset)
{
   int val;
   args p;
   printk(KERN_INFO"File opened for Read \n");
   if(copy_from_user(&p,(args *)ioctl_param,sizeof(args)))
      return -1;
   switch(p.val1)
   {
      case 1:
         val=adc_val();
         printk(KERN_INFO" value from ADC[1] = %d\n",val);
         break;
      case 2:
         val=adc_val();
         printk(KERN_INFO" value from ADC[2] = %d\n",val);
         break;
      case 3:
          val=adc_val();
         printk(KERN_INFO" value from ADC[3] = %d\n",val);
         break;
      case 4:
          val=adc_val();
         printk(KERN_INFO" value from ADC[4] = %d\n",val);
         break;
      case 5:
          val=adc_val();
         printk(KERN_INFO" value from ADC[5] = %d\n",val);
         break;
      case 6:
          val=adc_val();
         printk(KERN_INFO" value from ADC[6] = %d\n",val);
         break;
      case 7:
         val=adc_val();
         printk(KERN_INFO" value from ADC[7] = %d\n",val);
         break;
      case 8:
          val=adc_val();
         printk(KERN_INFO" value from ADC[8] = %d\n",val);
         break;
      default:
         pr_err("Invalid channel ID\n");
         return -1;
   }
   
   p.val2=val;
   if(copy_to_user((args *)ioctl_param,&p,sizeof(args)))
   return -1;

   return 0;
}
ssize_t my_write (struct file *pfile, const char __user *puser, size_t length, loff_t *poffset)
{
   printk(KERN_INFO"File Write() \n");
   return length;
}
int my_open (struct inode *pinode, struct file *pfile)
{
   printk(KERN_INFO"File is Opened \n");
   return 0;
}
int my_release (struct inode *pinode, struct file *pfile)
{
   printk(KERN_INFO" File is Closed  \n");
   return 0;
}

long my_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
   int dummy_read;
   printk(KERN_INFO"Inside IOCTL Function \n");
   switch (ioctl_num)
   {
      case ADC_CHANNEL:
         dummy_read =  my_read (file, (char *)ioctl_param, 100, 0);
         break;

      default:
         pr_err("entered command is Invalid  \n");
         return -1;
   }
   return 0;
}

static struct file_operations fops = {
   .owner          = THIS_MODULE,
   .open           = my_open,
   .release        = my_release,
   .read           = my_read,
   .write          = my_write,
   .unlocked_ioctl = my_ioctl
};

int char_driver_init(void)
{
   printk(KERN_INFO"Inside char_driver_init function \n");

   dev_nmbr=MKDEV(MAJOR_NO,MINOR_NO);
   
   printk(KERN_INFO"Major and Minor device numbers has been generated successfully.\n");
   printk(KERN_INFO"Major Device no. = %d, Minor Device no. = %d \n",MAJOR(dev_nmbr),MINOR(dev_nmbr));

   if((cls_cr=class_create(THIS_MODULE,"char2_driver"))==NULL)
   {
      unregister_chrdev_region(dev_nmbr,1);
      return -1;  
   }

   if(device_create(cls_cr,NULL,dev_nmbr,NULL,"adc_channel")==NULL)
   {
      class_destroy(cls_cr);
      unregister_chrdev_region(dev_nmbr,1);
      return -1;
   }

   cdev_init(&c_dev,&fops);
   if(cdev_add(&c_dev,dev_nmbr,1) == -1)
   {
      device_destroy(cls_cr,dev_nmbr);
      class_destroy(cls_cr);
      unregister_chrdev_region(dev_nmbr,1);
      return -1;
   }

   return 0;
}

void char_driver_exit(void)
{
   cdev_del(&c_dev);
   device_destroy(cls_cr,dev_nmbr);
   class_destroy(cls_cr);
   unregister_chrdev_region(dev_nmbr,1);
   printk(KERN_INFO"Char Driver unregistered successfully \n");
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_AUTHOR("JOHN");
MODULE_DESCRIPTION("CHAR DRIVER");
MODULE_LICENSE("GPL");
