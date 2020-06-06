#include "include.h"

#define DataPort P0

sbit    RS=P3^3;
sbit    RW=P3^2;
sbit    EN=P3^5;


void lcdDelay(unsigned int m)            //延时程序，微妙级
{
    while(m--)
    {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }
}

void lcdBuzy()
{
    DataPort=0xff;
    RW=1;
    RS=0;
    EN=1;
    while(DataPort&0x80);
    EN=0;
}

void lcdTransferData(char data1,bit DI)  //传送数据或者命令,当DI=0,传送命令,当DI=1,传送数据.
{
    lcdBuzy();
    RW=0;
    RS=DI;
    DataPort=data1;
    EN=1;
    EN=0;

}



void lcdDingwei(unsigned char line,unsigned char row)     //定位在哪行哪列显示
{
    unsigned int i;
    switch(line)
    {
    case 1:  i=0x80+row;break;
    case 2:  i=0x90+row;break;
    case 3:  i=0x88+row;break;
    case 4:  i=0x98+row;break;
    default: i=0x80;break;

    }
    lcdTransferData(i,0);
    lcdDelay(1);
}

void lcdMsg(unsigned char  *addr,unsigned char line,unsigned char row)    //传送一个字符串
{
    lcdDingwei(line,row);
    while(*addr>0)
    {
        lcdTransferData(*addr,1);
        addr++;
    }


}


void lcdInitinal(void)           //LCD字库初始化程序
{
    lcdTransferData(0x30,0);  //8BIT设置,RE=0: basic instruction set
    lcdTransferData(0x08,0);  //Display on Control
    lcdTransferData(0x10,0);  //Cursor Display Control光标设置
    lcdTransferData(0x0C,0);  //Display Control,D=1,显示开
    lcdTransferData(0x01,0);  //Display Clear
}
