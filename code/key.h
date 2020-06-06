#ifndef __key_h__
#define __key_h__

sbit  line1 = P3^6;
sbit  line2 = P2^1;
sbit  row1  = P3^7;
sbit  row2  = P2^2;

#define  SET_LINE1  line1 = 1
#define  CLR_LINE1  line1 = 0
#define  SET_LINE2  line2 = 1
#define  CLR_LINE2  line2 = 0

#define  STU_ROW1  row1
#define  STU_ROW2  row2

#define		KEY_DELAY	15

#define		KEY_N	0
#define		KEY_1	1
#define		KEY_2	2
#define		KEY_3	3
#define		KEY_4	4


extern INT8U KeyTime;
extern INT8U get_key( void );

#endif
