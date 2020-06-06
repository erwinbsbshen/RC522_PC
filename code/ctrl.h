#ifndef  __ctrl_h__
#define  __ctrl_h__

sbit  beep = P3^4;
sbit  led  = P1^7;

#define	BEEP_ON		beep = 1
#define BEEP_OFF	beep = 0

#define	LED_ON		led = 0
#define	LED_OFF		led = 1
#define	LED_XOR		led = ~led

#define	CTRL_PC_MODE		0		//计算机控制方式
#define	CTRL_BOARD_MODE		1		//

#define	OPT_REG_MODE		0		//注册方式
#define	OPT_ADD_MODE		1	  	//充值方式
#define	OPT_INC_MODE		2	  	//消费方式
#define	OPT_READ_MODE		3	 	//读卡方式


#define ERROR_NONE			0x00	//正常
#define	ERROR_NOMONEY		0x01	//余额不足
#define	ERROR_NOCMD			0x02	//无效命令
#define	ERROR_CHECK			0x03	//校验错误
#define ERROR_KEYA_IC 		0x04	//密码错误
#define	ERROR_READ_IC  		0x05	//读卡失败
#define	ERROR_WRITE_IC  	0x06	//写卡失败
#define ERROR_WRITE_KEY_IC	0x07	//修改密码失败
#define ERROR_SET_CSTSN		0x08	//设置客户编码失败
#define ERROR_NOCARD		0x09	//IC卡不存在
#define	ERROR_ATCLL			0x0A	//防冲突失败
#define ERROR_SLCT			0x0B	//选卡失败
#define	ERROR_BLOCK_ADDR	0x0C	//块错误
#define	ERROR_OP_TYPE		0x0D	//操作方式错误	
	 
extern void ctrl_process( void );
extern void uart_over( void );

#endif
