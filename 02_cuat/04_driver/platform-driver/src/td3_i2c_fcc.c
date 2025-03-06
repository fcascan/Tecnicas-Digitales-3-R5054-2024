#include "../inc/td3_i2c_fcc.h"

/*****************************************************************************/

/* global variables */
volatile int I2CWakeUpCondition = 0;
wait_queue_head_t I2CWakeUp = __WAIT_QUEUE_HEAD_INITIALIZER(I2CWakeUp);
int virq;
static void __iomem *I2C_ModuleInit, *CLK_ModuleInit, *CTRL_ModuleInit; // I/O linux kernel memory atributes

/*****************************************************************************/

static int __init driverInit(void)
{
   int status = 0;

   pr_info("[driverInit]: START -> Module init\n");

   /* Allocate memory for my char device */
   if ((struct_state.myI2CCdev = cdev_alloc()) == NULL)
   {
      pr_err("[driverInit]: ERROR cdev_alloc() (%s %d)\n", __FUNCTION__, __LINE__);
      status = -1;
      return status;
   }

   pr_info("[driverInit]: cdev_alloc() OK\n");

   /* Reserve a range of char devices. The major number is assigned dynamically */   
   if ( ( status = alloc_chrdev_region(&struct_state.myI2C, NUM_MENOR, CANT_DISP, COMPATIBLE) ) < 0)
   {
      pr_err("[driverInit]: ERROR alloc_chrdev_region() %d (%s %d)\n", status, __FUNCTION__, __LINE__);
      return status;
   }

   pr_info("[driverInit]: alloc_chrdev_region() OK -> numero mayor asignado: %d = 0x%x\n", MAJOR(struct_state.myI2C), MAJOR(struct_state.myI2C));

   /* Initialize char device */
   cdev_init(struct_state.myI2CCdev, &i2c_my_ops);

   pr_info("[driverInit]: cdev_init() OK\n");

   /* Add the char device to the system */
   if ( ( status = cdev_add(struct_state.myI2CCdev, struct_state.myI2C, CANT_DISP) )  < 0 ) 
   {
      unregister_chrdev_region(struct_state.myI2C, CANT_DISP);
      
      pr_err("[driverInit]: ERROR fallo cdev_add() %d (%s %d)\n", status, __FUNCTION__, __LINE__);
      
      return status;
   }

   pr_info("[driverInit]: cdev_add() OK\n");

   /* Create the class */
   if ( (  struct_state.myI2CClass = class_create(THIS_MODULE, CLASS_NAME) ) == NULL)
   {
      /* Remove the char device from the system */
      cdev_del(struct_state.myI2CCdev);

      /* Unregister the number of devices registered in the system */
      unregister_chrdev_region(struct_state.myI2C, CANT_DISP);
      pr_err("[driverInit]: ERROR in class_create() (%s %d)\n", __FUNCTION__, __LINE__);
      
      return -1;
   }

   pr_info("[driverInit]: class_create() OK\n");

   /* Change permissions for /dev/td3_i2c_fcc */  
   struct_state.myI2CClass->dev_uevent = change_permission_cdev;

   /* Create device */
   if ((device_create(struct_state.myI2CClass, NULL,struct_state.myI2C, NULL, COMPATIBLE)) == NULL)
   {
      /* Remove the char device from the system */
      cdev_del(struct_state.myI2CCdev);

      /* Unregister the number of devices registered in the system */
      unregister_chrdev_region(struct_state.myI2C, CANT_DISP);
      
      /* Remove the registered device from the system */
      device_destroy(struct_state.myI2CClass, struct_state.myI2C);

      pr_err("[driverInit]: ERROR in device_create() (%s %d)\n", __FUNCTION__, __LINE__);
      
      return -1;
   }

   pr_info("[driverInit]: device_create() OK\n");

   /* Register platform driver */
   status = platform_driver_register(&MyPlatformDriver);
   if (status <  0 )
   {
      /* Remove the char device from the system */
      cdev_del(struct_state.myI2CCdev);
      
      /* Unregister the number of devices registered in the system */
      unregister_chrdev_region(struct_state.myI2C, CANT_DISP);
      
      /* Remove the registered device from the system */
      device_destroy(struct_state.myI2CClass, struct_state.myI2C);
      
      /* Remove the class */
      class_destroy(struct_state.myI2CClass);
      
      pr_err("[driverInit]: ERROR in platform_driver_register() %d (%s %d)\n", status, __FUNCTION__, __LINE__);
      
      return status;
   }

   pr_info("[driverInit]: platform_driver_register() OK -> status = %d\n", status);

   pr_info("[driverInit]: END -> Module initialized\n");

   return 0;
}

static int change_permission_cdev(struct device *dev, struct kobj_uevent_env *env)
{
   pr_info("[change_permission_cdev]: START -> Changing permission\n");

   add_uevent_var(env, "DEVMODE=%#o", 0666); 

   pr_info("[change_permission_cdev]: END -> Permission changed\n");

   return 0;
}

static void __exit driverExit(void)
{
   pr_info("[driverExit]: START -> Removing module\n");

   cdev_del(struct_state.myI2CCdev);

   pr_info("[driverExit]: cdev_del(myI2CCdev) OK\n");

   unregister_chrdev_region(struct_state.myI2C, CANT_DISP);

   pr_info("[driverExit]: unregister_chrdev_region(myI2C) OK\n");

   device_destroy(struct_state.myI2CClass, struct_state.myI2C);

   pr_info("[driverExit]: device_destroy(myI2CClass, myI2C) OK\n");

   class_destroy(struct_state.myI2CClass);

   pr_info("[driverExit]: class_destroy(myI2CClass) OK\n");

   platform_driver_unregister(&MyPlatformDriver);

   pr_info("[driverExit]: platform_driver_unregister(&MyPlatformDriver) OK\n");

   pr_info("[driverExit]: END -> Module removed\n");
}

irqreturn_t FCCi2cHandlerIRQ (int IRQ, void *ID, struct pt_regs *REG)
{
   static uint32_t aux_val, reg_aux;
   /* 
      Read if an interrupt has arrived... for reception (Page 4610)
      Bit 3 RRDY:
         0h = No data available
         1h = Receive data available
   */
   aux_val = ioread32(I2C_ModuleInit + I2C_IRQSTATUS);

   /* Rx Interrupt */
   if ((aux_val & RRDY_IE) == RRDY_IE) 
   {
      /* Read the data received by the I2C module */
      I2CData.I2CRxData[ I2CData.I2CRxDataIndex] = ioread8(I2C_ModuleInit + I2C_DATA);
      I2CData.I2CRxDataIndex++;

      if ( I2CData.I2CRxDataIndex ==  I2CData.I2CRxDataLen)
      {
         /* Clear flags */
         reg_aux = ioread32(I2C_ModuleInit + I2C_IRQSTATUS);
         reg_aux |= 0x2E;
         iowrite32(reg_aux, I2C_ModuleInit + I2C_IRQSTATUS);

         /* Clear the interrupt by writing to the register */
         reg_aux = ioread32(I2C_ModuleInit + I2C_IRQENABLE_CLR);
         reg_aux |= RRDY_IE;
         iowrite32(reg_aux, I2C_ModuleInit + I2C_IRQENABLE_CLR);

         /* Wake up the process so the scheduler can run it */
         I2CWakeUpCondition = 1;
         wake_up_interruptible(&I2CWakeUp);
         I2CData.I2CRxDataIndex = 0;
      }
   }

   /* Read if an interrupt has arrived... for transmission (Page 4610)
      Bit 3 XRDY:
         0h = Transmission ongoing
         1h = Transmit data ready
      Set to '1' by core when transmitter and when new data is requested.
      When set to '1' by core, an interrupt is signaled to MPUSS.
      Write '1' to clear.
   */
   if ((aux_val & XRDY_IE) == XRDY_IE)
   {
      /* Send the data to the I2C module */
      iowrite8( I2CData.I2CTxData[ I2CData.I2CTxDataIndex], I2C_ModuleInit + I2C_DATA);
     
      I2CData.I2CTxDataIndex++;

      if (I2CData.I2CTxDataIndex == I2CData.I2CTxDataLen)
      {

         /* Clear flags */
         reg_aux = ioread32(I2C_ModuleInit + I2C_IRQSTATUS);
         reg_aux |= 0x36;
         iowrite32(reg_aux, I2C_ModuleInit + I2C_IRQSTATUS);

         /* Clear the interrupt by writing to the register */
         reg_aux = ioread32(I2C_ModuleInit + I2C_IRQENABLE_CLR);
         reg_aux |= XRDY_IE;
         iowrite32(reg_aux, I2C_ModuleInit + I2C_IRQENABLE_CLR);

         /* Wake up the process so the scheduler can run it */
         I2CWakeUpCondition = 1;
         wake_up(&I2CWakeUp);

         I2CData.I2CTxDataIndex = 0;
      }
   }

   /* Clear all flags */
   aux_val = ioread32(I2C_ModuleInit + I2C_IRQSTATUS);
   aux_val |= 0x6FFF;
   iowrite32(aux_val, I2C_ModuleInit + I2C_IRQSTATUS);

   return IRQ_HANDLED;
}

static int FCCi2cProbe(struct platform_device* MyI2cPD)
{
   uint32_t aux_val;

   pr_info("[FCCi2cProbe]: START -> Probe\n");
   /* Map the registers before they cause a data abort or prefetch abort */

   /* Map the base address of the i2c module */
   if ((I2C_ModuleInit = ioremap(I2C2_REG, I2C2_REG_LEN)) == NULL)
   {
      pr_err("[FCCi2cProbe]: ERROR The i2c register could not be mapped\n");
      return -1;
   }
   pr_info("[FCCi2cProbe]: Register mapped = 0x%x\n", (uint32_t)I2C_ModuleInit);

   /* Map the base address of the clock registers */
   if ((CLK_ModuleInit = ioremap(CM_PER_REG, CM_PER_REG_LEN)) == NULL)
   {
      pr_err("[FCCi2cProbe]: ERROR The CM_PER register could not be mapped\n");
      iounmap(I2C_ModuleInit);
      return -1;
   }
   pr_info("[FCCi2cProbe]: Register mapped = 0x%x\n", (uint32_t)CLK_ModuleInit);
   
   /* Map base control module register */
   if ((CTRL_ModuleInit = ioremap(CTRL_MODULE_REG, CTRL_MODULE_REG_LEN) ) == NULL)
   {
      pr_err("[FCCi2cProbe]: ERROR The CTRL_MODULE register could not be mapped\n");
      iounmap(I2C_ModuleInit);
      iounmap(CLK_ModuleInit);
      return -1;
   }

   pr_info("[FCCi2cProbe]: Register Mapped = 0x%x\n", (uint32_t)CTRL_ModuleInit);

   /*
      Se habilita el clock I2c2 (Ver Pagina 1267)
      Bit 1-0 MODULEMODE
         Control the way mandatory clocks are managed.
         0x0 = DISABLED : Module is disable by SW. Any OCP access to module results in an error, except if resulting from a module wakeup (asynchronous wakeup).
         0x1 = RESERVED_1 : Reserved
         0x2 = ENABLE 
         0x3 = RESERVED : Reserved
   */

   pr_info("[FCCi2cProbe]: Reading...\n");
   aux_val = ioread32(CLK_ModuleInit + CM_PER_I2C2_CLKCTRL);
   aux_val |= 0x02;

   pr_info("[FCCi2cProbe]: Writing\n");
   iowrite32(aux_val, CLK_ModuleInit + CM_PER_I2C2_CLKCTRL);

   pr_info("[FCCi2cProbe]: Wrinting done\n");
   msleep(100);

   aux_val = 0x0;
   aux_val = ioread32(CLK_ModuleInit + CM_PER_I2C2_CLKCTRL); // read the register that was just written
   pr_info("[FCCi2cProbe]: check\n");
   if ((aux_val & 0x03) != 0x02)
   {
      /* If it is not written as expected, return error */
      pr_err("[FCCi2cProbe]: ERROR writing I2C2 clk register\n");
      iounmap(I2C_ModuleInit);
      iounmap(CLK_ModuleInit);
      iounmap(CTRL_ModuleInit);
      return -1;
   }
   /*
      Enable SDA and SCL pins
      PIN 19 : UART1_Ctsn - I2C2_SCL
      PIN 20 : UART1_Ctsn - I2C2_SDA
   */

   /* Configure pin 20 */
   pr_info("[FCCi2cProbe]: Arrancando a configurar los pines\n");
   aux_val = ioread32(CTRL_ModuleInit + CONF_UART1_CSTN);
   aux_val = PIN_I2C_CFG;
   iowrite32(aux_val, CTRL_ModuleInit + CONF_UART1_CSTN);

   /* Configure pin 19 */
   aux_val = ioread32(CTRL_ModuleInit + CONF_UART1_RSTN); 
   aux_val = PIN_I2C_CFG;
   iowrite32(aux_val, CTRL_ModuleInit + CONF_UART1_RSTN);
   
   pr_info("[FCCi2cProbe]: SDA and SCL successfully configured\n");

   /* Get the irq number from the device tree */
   if ((virq = platform_get_irq(MyI2cPD, 0)) < 0)
   {
      pr_err("[FCCi2cProbe]: ERROR virtual interrupt request could not be obtained\n");
      iounmap(I2C_ModuleInit);
      iounmap(CLK_ModuleInit);
      iounmap(CTRL_ModuleInit);
      return -1;
   }

   if (request_irq(virq, (irq_handler_t)FCCi2cHandlerIRQ, IRQF_TRIGGER_RISING, COMPATIBLE, NULL))
   {
      pr_err("[FCCi2cProbe]: ERROR Could not assign the virtual interrupt request\n");
      iounmap(I2C_ModuleInit);
      iounmap(CLK_ModuleInit);
      iounmap(CTRL_ModuleInit);
      return -1;
   }
  
   pr_info("[FCCi2cProbe]: END -> Probe OK\n");

   return 0;
}

static int FCCi2cRemove (struct platform_device* MyI2cPD)
{
   pr_info("[FCCi2cRemove]: START -> Removal of module\n");

   free_irq(virq, NULL);
   iounmap(I2C_ModuleInit);
   iounmap(CLK_ModuleInit);
   iounmap(CTRL_ModuleInit);

   pr_info("[FCCi2cRemove]: END -> Module removed\n");
   return 0;
}

static void FCCi2cWrite(uint8_t *dataTx, uint8_t len)
{
   pr_info("[FCCi2cWrite]: START -> Write data to i2c\n");
   uint32_t reg = 0;
   uint32_t i = 0;

   /*
      Read the busy bit (Page 4604)
      Bit BB (12)
         0h = Bus is free
         1h = Bus is occupied
   */
   reg = ioread32(I2C_ModuleInit + I2C_IRQSTATUS_RAW);

   while ((reg >> 12) & 1)
   {
      msleep(100);
      pr_err("[FCCi2cWrite]: WARN Bus is busy\n");
      i++;
      if (i == WRITE_RETRIES)
      {
         pr_err("[FCCi2cWrite]: ERROR Retry limit reached\n");
         return;
      }
      reg = ioread32(I2C_ModuleInit + I2C_IRQSTATUS_RAW);
   }

   /* Reserve memory to store data received via I2C */
   I2CData.I2CTxData=dataTx;
   I2CData.I2CTxDataLen = len;

   /* 
      Set the counter with the number of bytes to read (See Page 4629)
      Note that this counter decrements by 1 for each received byte
   */
   iowrite32(I2CData.I2CTxDataLen, I2C_ModuleInit + I2C_CNT);

   /*
      Set module as master and enable transmission (See Page 4632)
      Bit 9 TRX   0h = Receiver mode
                  1h = Transmitter mode
      Bit 10 MST  0h = Slave mode
                  1h = Master mode
   */
   reg = ioread32(I2C_ModuleInit + I2C_CON);
   reg |= 0x600;
   iowrite32(reg, I2C_ModuleInit + I2C_CON);

   /*
      Enable Tx interrupt (See Page 4612)
      XRDY_IE
        Transmit data ready interrupt enable set.
        Mask or unmask the interrupt signaled by bit in I2C_STAT[XRDY].
        0h = Transmit data ready interrupt disabled
        1h = Transmit data ready interrupt enabled
   */
   iowrite32(XRDY_IE, I2C_ModuleInit + I2C_IRQENABLE_SET);

   /* Generate start condition (See Page 4633)
      Bit 1 STP:
         0h = No action or stop condition detected
         1h = Stop condition queried
      Bit 0 STT:
         0h = No action or start condition detected
         1h = Start condition queried
   */
   reg = ioread32(I2C_ModuleInit + I2C_CON);
   reg |= I2C_CON_START;

   iowrite32(reg, I2C_ModuleInit + I2C_CON);

   /* Put task to sleep until the condition is met */
   wait_event(I2CWakeUp, I2CWakeUpCondition > 0);
   I2CWakeUpCondition = 0;

   pr_info("[FCCi2cWrite]: TX OK\n");

   /* Generate STOP condition */
   reg = ioread32(I2C_ModuleInit + I2C_CON);
   reg &= 0xFFFFFFFE;
   reg |= I2C_CON_STOP;
   iowrite32(reg, I2C_ModuleInit + I2C_CON);

   msleep(1);

   pr_info("[FCCi2cWrite]: END -> Tx OK\n");
}

static uint8_t FCCi2cRead(uint8_t *dataRx, uint32_t len)
{
   pr_info("[FCCi2cRead]: START -> Read data from i2c\n");

   uint32_t i = 0;
   uint32_t reg, estado;
   /*
      Read the busy bit (Page 4604)
      Bit BB (12)
         0h = Bus is free
         1h = Bus is occupied
   */
   reg = ioread32(I2C_ModuleInit + I2C_IRQSTATUS_RAW);

   while ((reg >> 12) & 1)
   {
      msleep(100);
      pr_err("[FCCi2cRead]: WARN Bus is busy\n");
      if (i == READ_RETRIES)
      {
         pr_err("[FCCi2cRead]: ERROR Retry limit reached\n");
         return -1;
      }
      else
         i++;
      reg = ioread32(I2C_ModuleInit + I2C_IRQSTATUS_RAW);
   }

   /* Reserve memory to store received data via I2C */
   I2CData.I2CRxDataLen = len;

   if ( (  I2CData.I2CRxData = kmalloc(I2CData.I2CRxDataLen * sizeof(uint8_t), GFP_KERNEL) ) == NULL)
   {
      pr_err("[FCCi2cRead]: ERROR requesting memory for I2CData.I2CRxData\n");
      return -1;
   }

   /* 
      Set the counter with the number of bytes to read (See Page 4629)
      Note that this counter decrements by 1 for each received byte
   */
   iowrite32(I2CData.I2CRxDataLen, I2C_ModuleInit + I2C_CNT);

   /* Configure the BeagleBone to receive data */
   reg = ioread32(I2C_ModuleInit + I2C_CON);

   /*
      (See Page 4631)
      Bit 10 MST  
         0h = Slave mode
         1h = Master mode
      Bit 15 I2C_EN
         0h = Controller in reset. FIFO is cleared and status bits are set to their default value.
         1h = Module enabled
   */
   reg = 0x8400;
   iowrite32(reg, I2C_ModuleInit + I2C_CON);

   /* 
      Enable Rx interrupt (See Page 4611)
      Bit 3 RRDY_IE:
         0h = Receive data ready interrupt disabled
         1h = Receive data ready interrupt enabled
   */
   iowrite32(RRDY_IE, I2C_ModuleInit + I2C_IRQENABLE_SET);

   /* 
      Generate start condition (See Page 4633)
      Bit 1 STP:
         0h = No action or stop condition detected
         1h = Stop condition queried
      Bit 0 STT:
         0h = No action or start condition detected
         1h = Start condition queried
   */
   reg = ioread32(I2C_ModuleInit + I2C_CON);
   reg &= 0xFFFFFFFC;
   reg |= I2C_CON_START;
   iowrite32(reg, I2C_ModuleInit + I2C_CON);

   /* Put process to sleep until the condition is met */
   if ((estado = wait_event_interruptible(I2CWakeUp, I2CWakeUpCondition > 0)) < 0)
   {
      I2CWakeUpCondition = 0;
      pr_err("[FCCi2cRead]: ERROR sleeping by interrupt\n");
      return estado;
   }

   I2CWakeUpCondition=0;

   /* Generate STOP condition */
   reg = ioread32(I2C_ModuleInit + I2C_CON);
   reg &= 0xFFFFFFFE;
   reg |= I2C_CON_STOP;
   iowrite32(reg, I2C_ModuleInit + I2C_CON);
   pr_info("[FCCi2cRead]: Successfully received!\n");

   /* Copy the buffers */
   for(i=0; i<I2CData.I2CRxDataLen; i++)
   {
      dataRx[i] = I2CData.I2CRxData[i];
   }

   pr_info("[FCCi2cRead]: END -> data read from i2c = %d\n", I2CData.I2CRxDataLen);

   return I2CData.I2CRxDataLen;
}

static void MPUInit(void)
{
   pr_info("[MPUInit]: START -> MPU6050 module initialization\n");

   uint8_t aux = 0;
   uint8_t buffer[2];

   /* Enter running mode, exit sleep mode. (See page 41) */
   buffer[0] = MPU6050_PWR_MGMT_1;
   buffer[1] = 0x00;
   FCCi2cWrite(buffer, 2);
   
   /* Configure clock source with gyro pll */
   buffer[0] = MPU6050_PWR_MGMT_1;
   buffer[1] = 0x01;
   FCCi2cWrite(buffer, 2);

   msleep(100);

   /*
      Configure gyro and accelerometer filter (Page 13)
      Accelerometer Fs=1Khz  |            Gyroscope
      Bandwidth(Hz) Delay(ms)| Bandwidth(Hz) Delay(ms) Fs(kHz)
      260            0       |       256       0.98         8
   */
   buffer[0] = MPU6050_CONFIG;
   buffer[1] = 0x03;
   FCCi2cWrite(buffer, 2);

   /* Configure the sample generation period (Page 12) */
   buffer[0] = MPU6050_SMPLRT_DIV;
   buffer[1] = 0x04;
   FCCi2cWrite(buffer, 2);

   /* Configure gyro to have a scale of +-250°/s (See page 14.) */
   buffer[0] = MPU6050_GYRO_CONFIG;
   FCCi2cWrite(buffer, 1);

   /* Clear self-test bits (See page 14.) */
   FCCi2cRead(buffer, 1);        // Read current register value
   aux = buffer[0];
   buffer[0] = MPU6050_GYRO_CONFIG; 
   buffer[1] = (aux & ~0xE0);    // Clear self-test bits
   FCCi2cWrite(buffer, 2);       // Write

   /* Set higher resolution full-scale */
   buffer[0] = MPU6050_GYRO_CONFIG; 
   buffer[1] = (aux & ~0x18); // Pongo en 0'b los bits 4 y 3
   FCCi2cWrite(buffer, 2);
  
   buffer[0] = MPU6050_GYRO_CONFIG;
   buffer[1] = (aux | GSCALE_250 << 3);
   FCCi2cWrite(buffer, 2);

   /* Configure accelerometer to have a scale of +-2g (See page 15.) */
   buffer[0] = MPU6050_ACCEL_CONFIG;
   FCCi2cWrite(buffer, 1);

   FCCi2cRead(buffer, 1);        // Read current register value
   aux = buffer[0];
   buffer[0] = MPU6050_ACCEL_CONFIG;
   buffer[1] = (aux & ~0xE0);    // Clear self-test bits
   FCCi2cWrite(buffer, 2);

   buffer[0] = MPU6050_ACCEL_CONFIG;
   buffer[1] = (aux & ~0x18); // Clear bits 4 and 3
   FCCi2cWrite(buffer, 2);
 
   buffer[0] = MPU6050_ACCEL_CONFIG;
   buffer[1] = (aux | ACEL_2G << 3); // Set accelerometer to maximum scale
   FCCi2cWrite(buffer, 2);

   /*
      Configure interrupt pin (Page 27)
      (MODULE INT PIN)
      Disable INT pin
      Allow Beagle to access the sensor's auxiliary i2c bus 
   */
   buffer[0] = MPU6050_INT_PIN_CFG;
   buffer[1] = 0x02;
   FCCi2cWrite(buffer, 2);

   /*
      When this pin is '1', enable "DATA READY" interrupt, 
      which occurs every time a write operation is made to all sensor registers.
   */
   buffer[0] = MPU6050_INT_ENABLE;
   buffer[1] = 0x01;
   FCCi2cWrite(buffer, 2);

   /*
      FIFO configuration (Page 40)
      FIFO_EN: When set to 1, this bit enables FIFO operations
      FIFO_RESET: This bit resets the FIFO buffer when set to 1 while FIFO_EN equals 0. 
      This bit automatically clears to 0 after the reset has been triggered
      Enable and reset FIFO
   */
   buffer[0] = MPU6050_USER_CTRL;
   FCCi2cWrite(buffer, 1);

   FCCi2cRead(buffer, 1);
   aux = buffer[0];
   buffer[0] = MPU6050_USER_CTRL;
   buffer[1] = (aux | 0x44);
   FCCi2cWrite(buffer, 2);

   buffer[0] = MPU6050_USER_CTRL;
   FCCi2cWrite(buffer, 1);
   FCCi2cRead(buffer, 1);
   aux = buffer[0];

   /* Configure to allow different sensed variables to write to FIFO (Page 17) */
   buffer[0] = MPU6050_FIFO_EN;
   buffer[1] = 0xF8;
   FCCi2cWrite(buffer, 2);

   buffer[0] = MPU6050_FIFO_EN;
   FCCi2cWrite(buffer, 1);
   FCCi2cRead(buffer, 1);
   aux = buffer[0];

   pr_info("[MPUInit]: END -> MPU6050 module initialized\n");
}

static uint16_t MPUReadFifo(void)
{
   pr_info("[MPUReadFifo]: START -> Read data from FIFO\n");

   uint16_t Count;
   uint8_t *BufferTx;

   if (( BufferTx = kmalloc(sizeof(uint8_t), GFP_KERNEL) ) == NULL)
   {
      pr_err("[MPUReadFifo]: ERROR Could not allocate dynamic memory for BufferTx\n");
      return -1;
   }

   /*
      These registers keep track of the number of samples currently in the FIFO buffer.
      FIFO_COUNT 16-bit unsigned value. Indicates the number of bytes stored in the FIFO
      buffer
   */
   /* FIFO_COUNTH variable contains the number of samples in the FIFO */
   *BufferTx = MPU6050_FIFO_COUNTH;
   FCCi2cWrite(BufferTx, 1);
   FCCi2cRead(BufferTx, 1);
   pr_info("[MPUReadFifo]: 0x%x\n", *BufferTx);

   Count = *BufferTx;

   *BufferTx = MPU6050_FIFO_COUNTL;
   FCCi2cWrite(BufferTx, 1);
   FCCi2cRead(BufferTx, 1);
   pr_info("[MPUReadFifo]: 0x%x\n", *BufferTx);
   
   /* Compose the FIFO counter value from high and low parts */
   Count = Count << 8 | *BufferTx;

   kfree(BufferTx);

   pr_info("[MPUReadFifo]: END -> FIFO size = %d\n", Count);

   return Count;
}

static int FCCopen(struct inode *inode, struct file *file)
{
   pr_info("[FCCopen]: START -> Char device opened\n");
   pr_info("[FCCopen]: inode = %p, file = %p\n", inode, file);

   // TODO DESPUES DE ESTO TENGO UN NULL POINTER EXCEPTION:
   // Unable to handle kernel NULL pointer dereference at virtual address 000000a4

   static uint32_t I2CComands;
   uint8_t reg[2];

   /* Configure the I2C module according to the Technical Reference Manual (Rev. P) (See Page 4596) */

   /* Step 0, turn off the i2c module (See Page 4631) */
   pr_info("[FCCopen]: Turning off the I2C module\n");
   if (!I2C_ModuleInit) {
      pr_err("[FCCopen]: Error: I2C_ModuleInit es nulo -> %p\n", I2C_ModuleInit);
      //return -ENODEV;
   }
   iowrite32(I2C_OFF, I2C_ModuleInit + I2C_CON);

   /* 1. Set prescale to get a CLK frequency of 12Mhz, where fclk is divided by 2 (See Page 4598) */
   pr_info("[FCCopen]: Setting prescale to %d\n", PRESCALER_X2);
   iowrite32(PRESCALER_X2, I2C_ModuleInit + I2C_PSC);

   /*
      2. Calculate the clock to get a transmission speed of 100 Kbps
      Using this register (I2C_SCLL) we set the time
      I2C clock for 100Kbps -> (1/12)*(SCLL+7) = 5uS
                                ICLK*(SCLL + 7) = tLOW  
      This register sets tLow - See page 4637
      To configure tHigh (I2C_SCLH) we use the following equation:
      
      tHIGH = (SCLH + 5) * ICLK
      5uS   = (SCLH+5)   *(1/12)

      (See Page 4638)
   */
   I2CComands = 53;
   iowrite32(I2CComands, I2C_ModuleInit + I2C_SCLL);

   I2CComands = 55;
   iowrite32(I2CComands, I2C_ModuleInit + I2C_SCLH);

   /* 3. Define the master's address (See Page 4643) */
   iowrite32(OWN_ADDR, I2C_ModuleInit + I2C_OA);

   /* Set the slave address to communicate with (See page 4635) */
   iowrite32(MPU6050_ADDRESS, I2C_ModuleInit + I2C_SA);

   /*
      4. Take the I2C module out of reset (I2C_CON:I2C_EN = 1). (See Page 4932)
      Bit 15 I2C_EN:
         0h = Controller in reset. FIFO are cleared and status bits are set to their default value.
         1h = Module enabled
      Bit 10 MST:
         0h = Slave mode
         1h = Master mode

      The module is enabled and in Master mode
   */
   I2CComands = 0x8400; 
   iowrite32(I2CComands, I2C_ModuleInit + I2C_CON);

   /* Start MPU6050 configuration */
   pr_info("[FCCopen]: MPU6050 configuration\n");
   reg[0] = 0x75;
   FCCi2cWrite(reg, 1);
   FCCi2cRead(reg, 1);
               
   pr_info("[FCCopen]: Who am i = 0x%x\n", reg[0]);
   
   MPUInit();
   
   pr_info("[FCCopen]: END -> Char device opened\n");

   return 0;
}

static int FCCclose(struct inode *inode, struct file *file){
   pr_info("[FCCclose]: Cerrando! \n");
   // Deshabilito modulo I2C
   iowrite32(0x0000, I2C_ModuleInit + I2C_CON);
   iowrite32(0x00, I2C_ModuleInit + I2C_PSC);   // ??? TODO revisar
   iowrite32(0x00, I2C_ModuleInit + I2C_SCLL);  // Set to reset value
   iowrite32(0x00, I2C_ModuleInit + I2C_SCLH);  // Set to reset value
   iowrite32(0x00, I2C_ModuleInit + I2C_OA);    // Set to reset value
   return 0;
}

static ssize_t FCCread(struct file *device_descriptor, char *buff, size_t dataLength, loff_t *offset){
   uint32_t FifoCount = 0;
   uint8_t attempt = 0,i;
   uint8_t *SamplesBuffer;
   uint8_t *MPUCommands;
   uint8_t containers;
   uint16_t *samples;
   uint32_t copy2user;

   /*
      (See Page 30) - MPU-6000 and MPU-6050 Product Specification Revision 3.4
      The sensor can provide 7 variables 43 67 GYRO_XOUT[15:8]   GYRO_XOUT[7:0]  GYRO_YOUT[15:8]  GYRO_YOUT[7:0]  GYRO_ZOUT[15:8]  GYRO_ZOUT[7:0] TEMP[0:7] TEMP[8:15]
                                               ACCEL_XOUT[15:8]  ACCEL_XOUT[7:0] ACCEL_YOUT[15:8] ACCEL_YOUT[7:0] ACCEL_ZOUT[15:8] ACCEL_ZOUT[7:0]
      In total 14 bytes, since my FIFO is 1024 bytes I want to get as many samples as possible so 1024/14 -> I can store 73 samples of all output variables in each container
      with 2 bytes remaining.
   */
   /* Allocate memory for the sensor commands */
   if ((  MPUCommands = kmalloc(2 * sizeof(uint8_t), GFP_KERNEL) ) == NULL)
   {
      pr_err("[FCCread]: ERROR Could not allocate dynamic memory for the read buffer\n");
      return -1;
   }

   /* Data verification: Check if the number of bytes requested for reading is a multiple of 14 bytes */
   if ((dataLength % 14) != 0)
   {
      // pr_err("[FCCread]: ERROR attempting to read that amount of bytes! It must be a multiple of 14\n", (int)dataLength % 14);
      return -1;
   }

   /* Divide the number of bytes into containers */
   containers = (uint8_t)(dataLength / 1022);

   if (dataLength % 1022 != 0)
      containers++;
   
   /* Allocate memory */
   if ((samples = kmalloc(containers * sizeof(uint16_t), GFP_KERNEL)) == NULL)
   {
      pr_err("[FCCread]: ERROR Could not allocate dynamic memory for the sample buffer!\n");
      return -1;
   }

   /* Set the size of the first container to the total length */
   samples[0] = dataLength;

   /* Initialize the samples value to 1022 - the amount of data to be read */
   for (i = 1; i < containers - 1; i++)
      samples[i] = 1022;

   if (dataLength % 1022 != 0) 
      samples[containers - 1] = (int)dataLength % 1022;
   else 
      samples[containers - 1] = 1022;

   if ((SamplesBuffer = kmalloc(dataLength * sizeof(uint8_t), GFP_KERNEL)) == NULL)
   {
      pr_err("[FCCread]: ERROR Could not allocate dynamic memory for the data buffer!\n");
      return -1;
   }

   /* Reset the FIFO and enable it */
   MPUCommands[0] = MPU6050_USER_CTRL;
   MPUCommands[1] = 0x44;
   FCCi2cWrite(MPUCommands, 2);

   for (i = 0; i < containers; i++)
   {
      do
      {
         attempt++;
         pr_info("[FCCread]: FIFO Read Attempt No:%d\n", attempt);
         /* These registers keep track of the number of samples currently in the FIFO buffer. */
         FifoCount = (uint32_t)MPUReadFifo();
         pr_info("[FCCread]: FIFO Size %d Read Size %d\n", 2 * FifoCount, samples[i]);
      } while (FifoCount < (samples[i] / 2));

      attempt = 0;

      *MPUCommands = MPU6050_FIFO_R_W;
      FCCi2cWrite(MPUCommands, 1);
      FCCi2cRead(&SamplesBuffer[1022 * i], samples[i]);
   }

   if ((copy2user = copy_to_user(buff, SamplesBuffer, dataLength * sizeof(uint8_t))) < 0)
   {
      pr_err("[FCCread]: ERROR copy_to_user()\n");
      return -1;
   }

   kfree(samples);
   kfree(MPUCommands);
   kfree(SamplesBuffer);

   return dataLength;
}

static ssize_t FCCwrite(struct file *device_descriptor, const char *buff, size_t dataLength, loff_t *offset){
   /* Not used */
   return 0;
}

static long FCCioctl(struct file *device_descriptor, unsigned int command, unsigned long arguments){
   /* Not used */
   return 0;
}

module_init(driverInit);   // Function is called when the module is loaded with insmod
module_exit(driverExit);   // Function is called when the module is removed with rmmod
