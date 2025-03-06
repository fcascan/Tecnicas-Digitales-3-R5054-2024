#include <asm-generic/errno.h>
#include <asm-generic/errno-base.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/of_platform.h>
#include <linux/wait.h>
#include <linux/slab.h>

/*****************************************************************************/

/* my libraries */
#include "./I2CRegisters.h"
#include "./MPU6050.h"

/*****************************************************************************/

/* defines */
#define COMPATIBLE  "td3_i2c_fcc"
#define CLASS_NAME  "td3_i2c_fcc_class"
#define NUM_MENOR   0
#define CANT_DISP   1
#define READ_RETRIES    10
#define WRITE_RETRIES   10
#define OWN_ADDR    0xAA

/*****************************************************************************/

/* module information */
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Fernando Castro Canosa");
MODULE_VERSION("0.0.1");
MODULE_DESCRIPTION("Driver_MPU6050_i2c");

/*****************************************************************************/

/* functions prototypes */
static int __init driverInit(void);
static void __exit driverExit(void);
static int change_permission_cdev(struct device *dev,struct kobj_uevent_env *env);

/* file operations */
static int FCCopen(struct inode *inode, struct file *file);
static int FCCclose(struct inode *inode, struct file *file);
static ssize_t FCCread(struct file *device_descriptor, char *buff, size_t dataLength, loff_t *offset);
static ssize_t FCCwrite(struct file *device_descriptor, const char *buff, size_t data_length, loff_t *offset);
static long FCCioctl(struct file *device_descriptor, unsigned int command, unsigned long arguments);

/* platform device */
static int FCCi2cProbe(struct platform_device* MyI2cPD);
static int FCCi2cRemove(struct platform_device* MyI2cPD);
irqreturn_t FCCi2cHandlerIRQ(int IRQ, void *ID, struct pt_regs *REG);

/* i2c prototipes */
static uint8_t FCCi2cRead(uint8_t *dataRx, uint32_t len);
static void FCCi2cWrite(uint8_t *dataTx, uint8_t len);

/* MPU6050 prototipes */
static uint16_t MPUReadFifo(void);
static void MPUInit(void);

/*****************************************************************************/

/* data structure for char_device */
static struct
{
  dev_t myI2C;

  struct cdev *myI2CCdev;
  struct device *myI2CDevice;
  struct class *myI2CClass;

} struct_state;


static struct of_device_id i2c_of_device_ids[] =
{
  {
      .compatible = COMPATIBLE,
  },
  {}
};


static struct file_operations i2c_my_ops =
{
  .owner = THIS_MODULE,
  .open = FCCopen,
  .read = FCCread,
  .write = FCCwrite,
  .release = FCCclose,
  .unlocked_ioctl = FCCioctl
};

static struct platform_driver MyPlatformDriver =
{
  .probe  = FCCi2cProbe,
  .remove = FCCi2cRemove,
  .driver =
    {
        .name = COMPATIBLE,
        .of_match_table = of_match_ptr(i2c_of_device_ids),
    },
};

static struct
{
  /*Variables de Rx*/
  uint8_t * I2CRxData;
  uint32_t  I2CRxDataLen;
  uint32_t  I2CRxDataIndex;

  /*Variables de Tx*/
  uint8_t * I2CTxData;
  uint32_t   I2CTxDataLen;
  uint32_t   I2CTxDataIndex;
} I2CData;
