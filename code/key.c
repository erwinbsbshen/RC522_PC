#include "include.h"

INT8U KeyTime = 0;

INT8U get_key(void)
{
	//uchar tmp=0;
	SET_LINE1;
	SET_LINE2;
	CLR_LINE1;
  
	nop();
	if( ( STU_ROW1 == 0 ) && ( !KeyTime ) )
	{
		KeyTime = KEY_DELAY;
		return KEY_1;
	}
	else if( ( STU_ROW2 == 0 ) && ( !KeyTime ) )
	{
		KeyTime = KEY_DELAY;
		return KEY_2;
	}
  
	SET_LINE1;
	SET_LINE2;
	CLR_LINE2;

	nop();
	if( ( STU_ROW1 == 0 ) && ( !KeyTime ) )
	{
 		KeyTime = KEY_DELAY;
		return KEY_3;
	}
	else if( ( STU_ROW2 == 0 ) && ( !KeyTime ) )
	{
		KeyTime = KEY_DELAY;
		return KEY_4;
	}  
  
	return 0; 
}