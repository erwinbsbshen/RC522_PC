#ifndef  __timer_h__
#define  __timer_h__

#define	TIME10MS	2
#define	TIME80MS	3
#define	TIME200MS	10

extern INT16U UartWaitTime, RequestCardTime, LedTime;

extern void init_timer(void);  //20ms
extern void timer_fast( void );
extern void timer_slow( void );

#endif