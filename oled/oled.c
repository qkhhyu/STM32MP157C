#include "oled.h"

/* 要控制使用的I2C对应的I2C控制器的设备节点 */
#define I2C_DEVICE "/dev/i2c-2"
/* OLED屏幕 设备地址 */
#define OLED_ADDR 0x3C      /* 通过调整0R电阻,屏可以有0x3C和0x3D两个7位地址 -- 默认0x3C */

#define	Brightness	0xCF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

/* i2c设备文件描述符 */
int fd_i2c2;

/* 描述：  oled的iic总线初始化 
 * 参数：  无
 * 返回值：无               */
static void iic_init(void)
{
    /* 打开I2C控制器设备文件 */
    fd_i2c2 = open(I2C_DEVICE, O_RDWR);
    if(fd_i2c2 < 0) printf("oled init, open i2c-2 failed \n");

    /* 设置I2C设备地址 */
    if(ioctl(fd_i2c2, I2C_SLAVE_FORCE, OLED_ADDR) < 0)
        printf("set slave address failed \n");
}

/* 描述：  向OLED写入数据函数 
 * 参数：  要写入的数据 
 * 返回值：无               */
void oled_wrdata(unsigned char iic_data)
{
    unsigned char wrdata_buf[2];
    wrdata_buf[0] = 0x40;       /* 表示写入数据 */
    wrdata_buf[1] = iic_data;   /* 要写入的数据 */
    write(fd_i2c2, wrdata_buf, 2);
}

/* 描述：  向OLED发送命令函数 
 * 参数：  要写入的数据 
 * 返回值：无               */
void oled_wrcmd(unsigned char iic_cmd)
{
    unsigned char wrcmd_buf[2];
    wrcmd_buf[0] = 0x00;        /* 表示写入命令 */
    wrcmd_buf[1] = iic_cmd;     /* 要写入的命令 */
    write(fd_i2c2, wrcmd_buf, 2);
}

/* 描述：  OLED全屏显示函数 
 * 参数：  要全屏显示的数据 
 * 返回值：无               */
void oled_fill(unsigned char bmp_dat) 
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		oled_wrcmd(0xb0+y);
		oled_wrcmd(0x00);
		oled_wrcmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		oled_wrdata(bmp_dat);
	}
}

/* 描述：  oled屏幕初始化 
 * 参数：  无
 * 返回值：无               */
void oled_init(void)
{	
	iic_init();
	sleep(0.1);     //初始化之前的延时很重要！
	oled_wrcmd(0xae);//--turn off oled panel
	oled_wrcmd(0x00);//---set low column address
	oled_wrcmd(0x10);//---set high column address
	oled_wrcmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	oled_wrcmd(0x81);//--set contrast control register
	oled_wrcmd(Brightness); // Set SEG Output Current Brightness
	oled_wrcmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	oled_wrcmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	oled_wrcmd(0xa6);//--set normal display
	oled_wrcmd(0xa8);//--set multiplex ratio(1 to 64)
	oled_wrcmd(0x3f);//--1/64 duty
	oled_wrcmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	oled_wrcmd(0x00);//-not offset
	oled_wrcmd(0xd5);//--set display clock divide ratio/oscillator frequency
	oled_wrcmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	oled_wrcmd(0xd9);//--set pre-charge period
	oled_wrcmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	oled_wrcmd(0xda);//--set com pins hardware configuration
	oled_wrcmd(0x12);
	oled_wrcmd(0xdb);//--set vcomh
	oled_wrcmd(0x40);//Set VCOM Deselect Level
	oled_wrcmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	oled_wrcmd(0x02);//
	oled_wrcmd(0x8d);//--set Charge Pump enable/disable
	oled_wrcmd(0x14);//--set(0x10) disable
	oled_wrcmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	oled_wrcmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	oled_wrcmd(0xaf);//--turn on oled panel
	oled_fill(0x00); //初始清屏
}

/* 描述：  OLED 设置坐标
 * 参数：  要设置的坐标（x,y），y为页范围0～7
 * 返回值：无               */
static void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	oled_wrcmd(0xb0+y);
	oled_wrcmd(((x&0xf0)>>4)|0x10);
	oled_wrcmd((x&0x0f)|0x01);
}

/* 描述：  显示6*8一组标准ASCII字符串
 * 参数：  显示的坐标（x,y），y为页范围0～7
 * 返回值：无               */
void oled_P6x8Str(unsigned char x, unsigned char y,char ch[])
{
	unsigned char c = 0,i = 0,j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if(x > 126) {x = 0; y++; }
		OLED_Set_Pos(x, y);
		for(i = 0; i < 6; i++)
		oled_wrdata(F6x8[c][i]);
		x += 6;
		j++;
	}
}
void oled_P8x16Strclear(unsigned char x,unsigned char y)
{
	unsigned char c=0,i=0,j=0;
	unsigned char cline = 16-x/8;
	while (cline--)
	{
		c =0x00;
		if(x>120){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<8;i++)
		oled_wrdata(F8X16[c*16+i]);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		oled_wrdata(F8X16[c*16+i+8]);
		x+=8;
	}
}
/* 描述：  显示8*16一组标准ASCII字符串
 * 参数：  显示的坐标（x,y），y为页范围0～7
 * 返回值：无               */
void oled_P8x16Str(unsigned char x,unsigned char y,char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<8;i++)
		oled_wrdata(F8X16[c*16+i]);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		oled_wrdata(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}

/* 描述：  显示显示BMP图片128×64起始点坐标(x,y)
 * 参数：  x的范围0～127，y为页的范围0～7
 * 返回值：无               */
void Draw_BMP(unsigned char x0, unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	oled_wrdata(BMP[j++]);
	    }
	}
}


/* 描述：  将OLED从休眠中唤醒
 * 参数：  无
 * 返回值：无               */
void OLED_ON(void)
{
	oled_wrcmd(0X8D);  //设置电荷泵
	oled_wrcmd(0X14);  //开启电荷泵
	oled_wrcmd(0XAF);  //OLED唤醒
}

/* 描述：  让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
 * 参数：  无
 * 返回值：无               */
void OLED_OFF(void)
{
	oled_wrcmd(0X8D);  //设置电荷泵
	oled_wrcmd(0X10);  //关闭电荷泵
	oled_wrcmd(0XAE);  //OLED休眠
}
