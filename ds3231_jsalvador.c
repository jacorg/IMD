#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <asm/div64.h>

#define  DEVICE_NAME "ds3231" 
#define  N 18
#define SECOND             0x00
/*
 * Seccion sobre Informacion del modulo
 */
MODULE_AUTHOR("Jacobo O. Salvador <jacosalvador@gmail.com>");
MODULE_LICENSE("GPL");                                                                                              
MODULE_DESCRIPTION("Char device driver para demostracion de lectura en RTC DS3231"); 
MODULE_VERSION("0.01");  

int major;
struct class *cls;
static int nOpen = 0;              // Count  times the device is opened


static struct i2c_client *slaveClient;
static char   cmd[256] = {0};           /*cmd from user space used in dev_write*/


int ds3231_open(struct inode *inode,struct file *file)
{
   nOpen++;
   printk("i2c_ds3231: device opened %d times\n", nOpen);
   return 0;
 
}
 
int ds3231_close(struct inode *inode,struct file *file)
{
    printk("%s->%d\n",__func__,__LINE__);
	return 0;
}
 
long ds3231_ioctl(struct file * file, unsigned int cmd, unsigned long args)
{
	printk("%s->%d\n",__func__,__LINE__);
 	return 0;
}

static ssize_t ds3231_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   
   uint8_t year,month,day,date,hour,minute,second;
   uint8_t aux;
   
   int Ret;
   //char buf[BUFFER_SIZE];
   int i;
   char buffer_out[N];
   char buf[] = {SECOND};

   copy_from_user(cmd,buffer, len);
   
   Ret = i2c_master_send(slaveClient, buf,1);
   if (Ret < 0)
	{
		printk("return value error: %d\r\n",Ret);
		return -1;
	}
        
	i2c_master_recv(slaveClient,buffer_out,N);      
             
    aux = (uint8_t)buffer_out[0];
    second = ((aux >> 4) * 10) + (aux & 0x0F);

    aux = (uint8_t)buffer_out[1];
    minute = ((aux >> 4) * 10) + (aux & 0x0F);

    aux = (uint8_t)buffer_out[2];
    hour = (((aux & 0x10) >> 4) * 10) + (aux & 0x0F);

    aux = (uint8_t)buffer_out[3];
    day = aux;

    aux = (uint8_t)buffer_out[4];
    date = ((aux >> 4) * 10) + (aux & 0x0F);

    aux = (uint8_t)buffer_out[5];
    month = ((aux & 0x10)>>4)*10+(aux & 0x0F);

    aux = (uint8_t)buffer_out[6];
    year = ((aux >> 4)*10)+ (aux & 0x0F);
     
	printk("==== Lectura RTC DD:MM:YY & HH:MM:SS==== "); 
	printk("DD:MM:YY--->%d:%d:%d\n",day,month,year); 
	printk("HH:MM:SS--->%d:%d:%d\n",hour,minute,second);
    printk("=========================================");
   return len;
}


static const struct file_operations fops_ds3231={
	.owner = THIS_MODULE,
	.open = ds3231_open,
	.write = ds3231_write,
	.release =ds3231_close,
	.unlocked_ioctl = ds3231_ioctl,
};
 
/* Declaracion e inicializacion de una estructura miscdevice */
static struct miscdevice ds3231_miscdevice = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = DEVICE_NAME,
		.fops = &fops_ds3231,
};

 
static int ds3231_remove(struct i2c_client *cli)
{
	pr_info("Executing remove()\n");
	pr_info("EXIT DS3231 i2c\n");	
	/* Unregister del miscDevice del kernel */
	misc_deregister(&ds3231_miscdevice);
	pr_info("Modulo descargado.........");
	return 0;
}
 
struct i2c_device_id id_table[] = {
};
 
int ds3231_probe(struct i2c_client *slave,const struct i2c_device_id *id)
{
	int ret_misc=0;
	//struct  device_node *of_node=slave->dev.of_node;
	//struct mcp4725_dev_t *mcp4725_pri=NULL;

    printk("Executing probe()\n");
	printk("Init DS3231 i2c\n");	
	printk("Informacion de dispositivo conectado (struct i2c_client):\n");
	printk("\tDireccion: %#x\n",slave->addr);
	printk("\tNombre: %s\n",slave->name);
	printk("\tDriver: %s\n",(slave->dev).driver->name);


	/* Registro del dispositivo con el kernel */
	ret_misc = misc_register(&ds3231_miscdevice);
	if (ret_misc != 0) {
		pr_err("No se pudo registrar el dispositivo %s\n", DEVICE_NAME);
		return ret_misc;
	}
	pr_info("Dispositivo %s: minor asignado: %i\n", DEVICE_NAME, ds3231_miscdevice.minor);

	/*
	 * Una vez que tenemos numeros Major y Minor, exponemos al modulo completo
	 * el puntero recibido como dispositivo esclavo conectado (cliente)
	 */
	slaveClient = slave;	//guardamos este puntero para el uso de read y write
	return 0;
}
 

static const struct of_device_id ds3231_dt_ids[]={
	{.compatible="mse,ds3231",},
	{}
};
 
MODULE_DEVICE_TABLE(of, ds3231_dt_ids);


 int __init ds3231_init(void)
{
	   return 0;
}
 
 
 
 
void __exit ds3231_exit(void)
{ 
	return 0;
}
 
static struct i2c_driver ds3231_driver = {

	.probe= ds3231_probe,
	.remove= ds3231_remove,
	.driver = {
		.name = "ds3231",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(ds3231_dt_ids),
	},
};

module_i2c_driver(ds3231_driver);


