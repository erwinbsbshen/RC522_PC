#include "include.h"


void delay_ms( INT16U tms )
{
    INT16U i;

    while( tms-- )
    {
    	for( i = 0; i < 300; i ++ )
    	{
            nop();
            nop();
            nop();
            nop();
            nop();
            nop();
    	}
    }
}

void init_led( void )
{
    P1 = 0x00;
    delay_ms(200);
    P1 = 0Xff;
}

void init_port( void )
{
//    P0M1 = 0x00;
//    P0M0 = 0xff;

    P3M1 &= ~0x10;
    P3M0 |= 0x10;
    //
    //	P1M1=0x00;
    //	P1M0=0x00;
    //
    //	P2M1=0x00;
    //	P2M0=0xff;
    //
    //	P30=1;
    //	P3M1=0x41;
    //	P3M0=0xa2;
    //
    //	P4M1=0x02;
    //	P4M0=0x00;
    BEEP_OFF;
    LED_OFF;
}

void init_par( void )
{
    BEEP_OFF;
    LED_OFF;
}

void init_wdt( void )	 //2.7S
{
    //	WDT_CONTR = 0xC1;
    //	WDT_CONTR = 0x3E;
}

void feed_wdt( void )
{
    //	WDT_CONTR = 0x3E;
}

void init_all(void)
{
    EA = 0;

    init_timer();
    init_uart();
    init_port();
    init_rc522();
    init_par();
    init_wdt();
    init_led();

    EA = 1;
}

void main(void)
{
    if((PCON&0x10)==0) //如果POF位=0
    {
       PCON=PCON|0x10;  //将POF位置1
       IAP_CONTR=0x60;  //软复位,从ISP监控区启动
    }
    else
    {
       PCON=PCON&0xef;  //将POF位清零
    }

    lcdInitinal();
    lcdMsg("浙江中医药大学",1,0);
	lcdMsg("嵌入式课程",2,0);
	lcdMsg("XXX",3,0);
    lcdMsg("www.zcmu.edu.cn",4,0);
    delay_ms(2000);
    lcdInitinal();
    delay_ms(2);
    lcdMsg("余额：----",4,0);
    lcdMsg("卡号：----",2,0);
    lcdMsg("消费：----",3,0);
    lcdMsg("状态：----",1,0);

    init_all();


    while(1)
    {
        feed_wdt();
        ctrl_process();
    }
}
