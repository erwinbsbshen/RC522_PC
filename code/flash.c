#include  "include.h"

/********************************************************
读单片机EEPROM
********************************************************/
/*
INT8U read_eeprom(INT16U addr)
{
  	INT8U val;
  	IAP_ADDRH=(INT8U)(addr>>8);
  	IAP_ADDRL=(INT8U)(addr);  
  	// 设置等待时间,30M以下0,24M以下1,20M以下2,12M以下3,6M以下4,3M以下5,2M以下6,1M以下7.
  	IAP_CONTR=0x82;    
  	// 0 0 : Standby 待机模式,无ISP/IAP操作
  	// 0 1 : 对 EEPROM 进行字节读
  	// 1 0 : 对 EEPROM 进行字节编程
  	// 1 1 : 对 EEPROM进行扇区擦除
  	IAP_CMD=1;   
  	IAP_TRIG=0x5a;
  	IAP_TRIG=0xa5;
  	nop();
  	val=IAP_DATA;
  	IAP_CONTR=0;
  	IAP_CMD=0;
  	IAP_TRIG=0;
  	IAP_ADDRH=0x80;
  	IAP_ADDRL=0x00;   
  	return val;
}*/
/*
void read_eeproms( INT16U StartAddr, INT8U * Buf, INT8U Len )
{
 	while( Len -- )
	{
		*Buf++ = read_eeprom( StartAddr++ );	
	}
}
*/

/********************************************************
单片机EEPROM扇区擦除
********************************************************/
/*
void erase_eeprom(INT16U first_addr)
{
  	IAP_ADDRH=(INT8U)(first_addr>>8);
  	IAP_ADDRL=(INT8U)(first_addr);
  	//设置等待时间,30M以下0,24M以下1,20M以下2,12M以下3,6M以下4,3M以下5,2M以下6,1M以下7.
  	IAP_CONTR=0x82;  
  	// 0 0 : Standby 待机模式,无ISP/IAP操作
  	// 0 1 : 对 EEPROM 进行字节读
  	// 1 0 : 对 EEPROM 进行字节编程
  	// 1 1 : 对 EEPROM进行扇区擦除
  	IAP_CMD=3; 
  	IAP_TRIG=0x5a;
  	IAP_TRIG=0xa5;
  	nop();
  	IAP_CONTR=0;
  	IAP_CMD=0;
  	IAP_TRIG=0;
  	IAP_ADDRH=0x80;
  	IAP_ADDRL=0x00;   
}

*/
/********************************************************
写单片机EEPROM
********************************************************/
/*
void program_eeprom(INT16U addr,INT8U val)
{
  	//uint data first_addr;
  	//first_addr=addr&0xe000;
  	//erase_eeprom(first_addr);
  	IAP_DATA=val;
  	IAP_ADDRH=(INT8U)(addr>>8);
  	IAP_ADDRL=(INT8U)(addr);
  	//设置等待时间,30M以下0,24M以下1,20M以下2,12M以下3,6M以下4,3M以下5,2M以下6,1M以下7.
  	// 1 0 0 0 0 0 D1 D2
  	IAP_CONTR=0x82;
  	// 0 0 : Standby 待机模式,无ISP/IAP操作
  	// 0 1 : 对 EEPROM 进行字节读
  	// 1 0 : 对 EEPROM 进行字节编程
  	// 1 1 : 对 EEPROM 进行扇区擦除
  	IAP_CMD=2;
  	IAP_TRIG=0x5a;
  	IAP_TRIG=0xa5;
  	nop();
  	IAP_CONTR=0;
  	IAP_CMD=0;
  	IAP_TRIG=0;
  	IAP_ADDRH=0x80;
  	IAP_ADDRL=0x00;
}
*/
/*
void program_eeproms( INT16U StartAddr, INT8U * Buf, INT8U Len )
{
	while( Len -- )
	{
		program_eeprom( StartAddr++, *Buf++ );
		delay_ms(2);	
	}
}
*/
/*
void init_flash(void)
{

}
*/
