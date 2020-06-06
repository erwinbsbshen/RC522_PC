#include "include.h"

INT16U UartWaitTime = 0, RequestCardTime = 0, LedTime = 0;

INT8U TimerTh0, TimerTl0;

//TIMER1 initialize - prescale:8
// desired value: 20mSec
// actual value: 19.999mSec (0.0%)
void init_timer(void)  //20ms
{
	TMOD = 0x21;

	TH0 = 0x69;
	TL0 = 0xFA;
	ET0 = 1;
	TR0 = 1;

	TH1 = 256 - FOSC / 12 / 32 / BAUD;
	TL1 = 256 - FOSC / 12 / 32 / BAUD;
	TR1 = 1;
}

void  time0_routine(void) interrupt 1
{
  //TIMER1 has overflowed
	//TMOD = 0x01;
 	TH0 = 0x69;
	TL0 = 0xFA;

	RequestCardTime ++;
	LedTime ++;
	if( KeyTime )
	{
		KeyTime --;
	}

	if( ( ( UartWaitTime ++ ) >= TIME10MS ) && ( UartStart == TRUE ) )
	{
		uart_over();
	}
	
	
}