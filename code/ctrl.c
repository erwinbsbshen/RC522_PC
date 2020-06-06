#include "include.h"

const INT8U DefaultKeyABuf[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

INT8U CardRevBuf[16] = { 0 };
INT8U const CardKeyABlockBuf[16] = {
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0xff,0x07,0x80,0x69,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
								};

INT8U CtrlMode = CTRL_BOARD_MODE;
INT8U OptMode  = OPT_INC_MODE;
INT8U bPass = 1; bWarn = 0;
INT8U LedOnType = LED_LONG;

int CARDREDUCE = 2;
int CARDADD = 10;


void pass( void )
{
	LED_ON;
	BEEP_ON;
	delay_ms(2000);
	BEEP_OFF;
	LED_OFF;
}

void warn(void)
{
	INT8U i;
	
	LED_OFF;
	for( i = 0; i < 3; i++ )
	{
		LED_ON;
		BEEP_ON;
		delay_ms(200);
		BEEP_OFF;
		LED_OFF;
		delay_ms(200);
	}
}

void cal_keyA( INT8U *DestBuf )
{
	const INT8U KeyABuf[] = { 0x20, 0x12, 0x10, 0x01, 0x00, 0x00 };

	memcpy( DestBuf, KeyABuf, 6 );
}

void send_rebck( psUartData psUartDataRevSend )
{
	INT8U * pTmp;
	INT8U Len,Check;
	INT8U i;

	Len = psUartDataRevSend->UartCmdLen	+ psUartDataRevSend->UartDataLen + 2;
	pTmp = ( INT8U * )psUartDataRevSend;

	send_byte( 0xFC );
	Check = 0xFC;
	for( i = 0; i < Len; i++ )
	{
		send_byte( *pTmp );
		Check ^= *pTmp++;
	}
	send_byte( Check );	
}

void uart_over( void )
{
	UartCount = 0;
	UartStart = FALSE;
	UartComp = FALSE;
}

INT8U check_com( psUartData psUartDataRevSend )
{
	psUartDataRevSend->UartDataBuf[0] = 0xAA;

	return TRUE;
}

INT8U req_card_sn( psUartData psUartDataRevSend )
{
	if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
	{
		if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
		{
			psUartDataRevSend->UartErrCode = ERROR_NOCARD;
			memset( psUartDataRevSend->UartDataBuf, 0x00, psUartDataRevSend->UartDataLen );
			bWarn = 1;
			return FALSE;
		}	
	}
	
	if( PcdAnticoll( &CardRevBuf[2] ) != MI_OK ) //防冲撞，返回卡的序列号 4字节 
	{
		psUartDataRevSend->UartErrCode = ERROR_ATCLL;
		memset( psUartDataRevSend->UartDataBuf, 0x00, psUartDataRevSend->UartDataLen );
		bWarn = 1;
		return FALSE;	
	}

	memcpy( psUartDataRevSend->UartDataBuf, &CardRevBuf[2], 4 );

	bPass = 1;

	return TRUE;
}

INT8U updata_key( psUartData psUartDataRevSend )
{
	INT8U CardKeyABuf[6];
	INT8U KeyBlockAddr;

	psUartDataRevSend->UartDataLen = 7;
	KeyBlockAddr = psUartDataRevSend->UartDataBuf[0];

	if( KeyBlockAddr % 4 != 3 )
	{
		psUartDataRevSend->UartErrCode = ERROR_BLOCK_ADDR;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 6 );
		bWarn = 1;
		return FALSE;	
	}

	if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
	{
		if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
		{
			psUartDataRevSend->UartErrCode = ERROR_NOCARD;
			memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 6 );
			bWarn = 1;
			return FALSE;
		}	
	}

	if( PcdAnticoll( &CardRevBuf[2] ) != MI_OK ) //防冲撞，返回卡的序列号 4字节 
	{
		psUartDataRevSend->UartErrCode = ERROR_ATCLL;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 6 );
		bWarn = 1;
		return FALSE;	
	}

	if( PcdSelect( &CardRevBuf[2] ) != MI_OK )//选卡
	{
		psUartDataRevSend->UartErrCode = ERROR_SLCT;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 6 );
		bWarn = 1;
		return FALSE;
	}

	memcpy( CardKeyABuf, &psUartDataRevSend->UartDataBuf[1], 6 );

	if( PcdAuthState( PICC_AUTHENT1A, KeyBlockAddr, CardKeyABuf, &CardRevBuf[2] ) != MI_OK )// 验证密码
	{
		psUartDataRevSend->UartErrCode = ERROR_KEYA_IC;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 6 );
		bWarn = 1;
		return FALSE;	
	}

	memcpy( CardKeyABlockBuf, &psUartDataRevSend->UartDataBuf[7], 6 );

	if( PcdWrite( KeyBlockAddr, CardKeyABlockBuf ) != MI_OK )// 写卡
	{
		psUartDataRevSend->UartErrCode = ERROR_WRITE_IC;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 6 );
		bWarn = 1;
		return FALSE;
	}

   	memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 6 );
	bPass = 1;
	return TRUE;
}

INT8U block_write( psUartData psUartDataRevSend )
{
	INT8U CardWriteBuf[16];
	INT8U CardKeyABuf[6];
	INT8U KeyBlockAddr;

	psUartDataRevSend->UartDataLen = 0x11;
	KeyBlockAddr = psUartDataRevSend->UartDataBuf[0];

	if( KeyBlockAddr % 4 == 3 )
	{
		psUartDataRevSend->UartErrCode = ERROR_BLOCK_ADDR;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;	
	}

	if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
	{
		if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
		{
			psUartDataRevSend->UartErrCode = ERROR_NOCARD;
			memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
			bWarn = 1;
			return FALSE;
		}	
	}

	if( PcdAnticoll( &CardRevBuf[2] ) != MI_OK ) //防冲撞，返回卡的序列号 4字节 
	{
		psUartDataRevSend->UartErrCode = ERROR_ATCLL;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;	
	}

	if( PcdSelect( &CardRevBuf[2] ) != MI_OK )//选卡
	{
		psUartDataRevSend->UartErrCode = ERROR_SLCT;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;
	}

	memcpy( CardKeyABuf, &psUartDataRevSend->UartDataBuf[1], 6 );

	if( PcdAuthState( PICC_AUTHENT1A, KeyBlockAddr, CardKeyABuf, &CardRevBuf[2] ) != MI_OK )// 验证密码
	{
		psUartDataRevSend->UartErrCode = ERROR_KEYA_IC;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;	
	}

	memcpy( CardWriteBuf, &psUartDataRevSend->UartDataBuf[7], 16 );
	if( PcdWrite( KeyBlockAddr, CardWriteBuf ) != MI_OK )//写卡
	{
		psUartDataRevSend->UartErrCode = ERROR_WRITE_IC;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;
	}
	bPass = 1;
	
	return TRUE;
}

INT8U block_read( psUartData psUartDataRevSend )
{
	INT8U CardReadBuf[16];
	INT8U CardKeyABuf[6];
	INT8U KeyBlockAddr;

	psUartDataRevSend->UartDataLen = 0x11;
	KeyBlockAddr = psUartDataRevSend->UartDataBuf[0];

	if( KeyBlockAddr % 4 == 3 )
	{
		psUartDataRevSend->UartErrCode = ERROR_BLOCK_ADDR;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;	
	}

	if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
	{
		if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
		{
			psUartDataRevSend->UartErrCode = ERROR_NOCARD;
			memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
			bWarn = 1;
			return FALSE;
		}	
	}

	if( PcdAnticoll( &CardRevBuf[2] ) != MI_OK ) //防冲撞，返回卡的序列号 4字节 
	{
		psUartDataRevSend->UartErrCode = ERROR_ATCLL;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;	
	}

	if( PcdSelect( &CardRevBuf[2] ) != MI_OK )//选卡
	{
		psUartDataRevSend->UartErrCode = ERROR_SLCT;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;
	}

	memcpy( CardKeyABuf, &psUartDataRevSend->UartDataBuf[1], 6 );

	if( PcdAuthState( PICC_AUTHENT1A, KeyBlockAddr, CardKeyABuf, &CardRevBuf[2] ) != MI_OK )// 验证密码
	{
		psUartDataRevSend->UartErrCode = ERROR_KEYA_IC;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;	
	}

	if( PcdRead( KeyBlockAddr, CardReadBuf ) != MI_OK )// 读卡
	{
		psUartDataRevSend->UartErrCode = ERROR_READ_IC;
		memset( &psUartDataRevSend->UartDataBuf[1], 0x00, 16 );
		bWarn = 1;
		return FALSE;
	}
	memcpy( &psUartDataRevSend->UartDataBuf[1], CardReadBuf, 16 );
	bPass = 1;
			
	return TRUE;
}

void display_error( void )
{
        lcdMsg("程序出错", 1, 3 );
        lcdMsg("----", 4, 3);
}

void display_no_id( void )
{
    char DisplayBuf[10];
    memset(DisplayBuf,0,10);

	switch( OptMode )
        {
		case OPT_INC_MODE:
                        lcdMsg("消费模式", 1, 3 );
                        lcdMsg("消费：",3,0);
                        sprintf(DisplayBuf,"%d.00",CARDREDUCE);
                        lcdMsg(DisplayBuf , 3, 3 );
			break;

		case OPT_ADD_MODE:
                        lcdMsg("充值模式", 1, 3 );
                        lcdMsg("充值：",3,0);
                        sprintf(DisplayBuf,"%d.0",CARDADD);
                        lcdMsg(DisplayBuf , 3, 3 );
			break;

		case OPT_READ_MODE:
                        lcdMsg("读卡模式", 1, 3 );
                        lcdMsg("读卡：----",3,0);
			break;

		case OPT_REG_MODE:
                        lcdMsg("开卡模式", 1, 3 );
                        lcdMsg("开卡：----",3,0);
			break;

		default:
			break;
	}
}

void display_money( INT32U CardMoney )
{
	char DisplayBuf[10];
	char *pDisplayChar;
	INT32U Tmp;

	pDisplayChar = DisplayBuf;

	if( CardMoney > 9999 )	//最大显示9999
	{
		CardMoney = 9999;
	}

	Tmp = CardMoney;

	*pDisplayChar++ = Tmp / 1000 + '0';
	Tmp %= 1000;
	*pDisplayChar++ = Tmp / 100 + '0';
	Tmp %= 100;
	*pDisplayChar++ = Tmp / 10 + '0';
	Tmp %= 10;
	*pDisplayChar++ = Tmp + '0';
	*pDisplayChar++ = '\0';

        lcdMsg(DisplayBuf, 4, 3 );
}

void display_id( INT8U * SrcBuf )
{
	char DisplayBuf[10];
	char *pDisplayChar;
	INT8U Tmp, i;

	pDisplayChar = DisplayBuf;

	for( i = 0; i < 4; i++ )
	{
		Tmp = ( ( *SrcBuf ) >> 4 ) & 0x0F;	
		if( ( Tmp >=0 ) && ( Tmp <= 9 )	)
		{
			*pDisplayChar ++ = '0' + Tmp;	
		}
		else
		{
			*pDisplayChar ++ = 'A' + Tmp - 10;
		}

		Tmp = ( *SrcBuf ) & 0x0F;	
		if( ( Tmp >=0 ) && ( Tmp <= 9 )	)
		{
			*pDisplayChar ++ = '0' + Tmp;	
		}
		else
		{
			*pDisplayChar ++ = 'A' + Tmp - 10;
		}

		SrcBuf ++;
	}
	*pDisplayChar ++ = '\0';

        lcdMsg(DisplayBuf, 2, 3 );
}

INT8U ctrl_by_pc( psUartData psUartDataRevSend )
{
	psUartDataRevSend = psUartDataRevSend;

	CtrlMode = CTRL_PC_MODE;
	
        lcdMsg("电脑控制", 1, 3 );
        lcdMsg("----", 4, 3);
        lcdMsg("----", 3, 3);
	return TRUE;	
}

INT8U ctrl_by_board( psUartData psUartDataRevSend )
{
	psUartDataRevSend = psUartDataRevSend;
	
	CtrlMode = CTRL_BOARD_MODE;

	display_no_id();
	
	return TRUE;	
}

void pc_process( void )
{
	psUartData psUartDataRevSend;
	INT8U i;
	INT8U Check = 0xFE;
	INT8U Len;

	if( UartComp != TRUE )
	{
		return;
	}

	psUartDataRevSend = (psUartData)UartBuf;
	Len = psUartDataRevSend->UartCmdLen + psUartDataRevSend->UartDataLen + 3;

	for( i = 0; i < ( Len - 1 ); i ++ )
	{
		Check ^= UartBuf[i];
	}
	/*
	if( Check != UartBuf[Len - 1] )
	{
		psUartDataRevSend->UartErrCode = ERROR_CHECK;
		send_rebck( psUartDataRevSend );
		uart_over();
		return;
	}
	*/
	
	switch( psUartDataRevSend->UartCmd )
	{	
		case 0x0002:  //检测串口状态 -> fe 03 01 c1 c0 er 00 ck,  <- fc 03 01 c1 c0 er aa ck
			check_com( psUartDataRevSend );
			break;
		case 0x0003: //查询卡号  ->	fe 03 04 c1 c0 er 00 00 00 00 ck, <- fc 03 04 c1 c0 er sn3 sn2 sn1 sn0 ck
			req_card_sn( psUartDataRevSend );
			break;

		case 0x0110: //修改密码方式0 fe 03 0d c1 c0 er kn o5 o4.. o0 n5 n4.. n0 ck, <-fe 03 07 c1 c0 er kn n5 n4.. n0 ck	
			updata_key( psUartDataRevSend ); //修改密码 kn%4 == 3	
			break;

		case 0x0120: //读数据块方式0  -> fe 03 07 c1 c0 er kn ky5 ... ky0 ck, <- fc 03 11 c1 c0 er kn d15...d0 ck 
			block_read( psUartDataRevSend ); //读数据块 kn%4 != 3	
			break;

		case 0x0130: //写数据块方式0  -> fe 03 07 c1 c0 er kn ky5 ... ky0 ck, <- fc 03 11 c1 c0 er kn d15...d0 ck 
			block_write( psUartDataRevSend ); //读数据块 kn%4 != 3	
			break;

		case 0x0140: //板子控制
			ctrl_by_board( psUartDataRevSend ); //读数据块 kn%4 != 3	
			break;	
			
		default:
			psUartDataRevSend->UartErrCode = ERROR_NOCMD;	
			break;
	}
	send_rebck( psUartDataRevSend );
	uart_over();
}

INT8U board_uart_event( void )
{
	psUartData psUartDataRevSend;
	INT8U i;
	INT8U Check = 0xFE;
	INT8U Len;
	INT8U Result = FALSE;

	if( UartComp != TRUE )
	{
		return Result;
	}

	psUartDataRevSend = (psUartData)UartBuf;
	Len = psUartDataRevSend->UartCmdLen + psUartDataRevSend->UartDataLen + 3;

	for( i = 0; i < ( Len - 1 ); i ++ )
	{
		Check ^= UartBuf[i];
	}
	/*
	if( Check != UartBuf[Len - 1] )
	{
		psUartDataRevSend->UartErrCode = ERROR_CHECK;
		send_rebck( psUartDataRevSend );
		uart_over();
		return;
	}
	*/
	
	switch( psUartDataRevSend->UartCmd )
	{	
		case 0x0141:  //计算机控制
			ctrl_by_pc( psUartDataRevSend );
			Result = TRUE;
			break;
			
		default:
			psUartDataRevSend->UartErrCode = ERROR_NOCMD;	
			break;
	}
	send_rebck( psUartDataRevSend );
	uart_over();

	return Result;
}

void board_process( void )
{
	INT8U CardReadBuf[16];
	INT8U CardWriteBuf[16];
	INT8U CardKeyABuf[6];
	INT32U CardMoney;
        INT8U KeyVal;


	if( board_uart_event() == TRUE )   //计算机控制
	{
		return;
	}

	KeyVal = get_key();
	switch( KeyVal )
	{
		case KEY_1:
			OptMode = OPT_INC_MODE;
                        lcdMsg("消费模式", 1, 3 );
			break;

		case KEY_2:
			OptMode = OPT_ADD_MODE;
                        lcdMsg("充值模式", 1, 3 );
			break;

		case KEY_3:
			OptMode = OPT_READ_MODE;
                        lcdMsg("读卡模式", 1, 3 );
			break;

		case KEY_4:
			OptMode = OPT_REG_MODE;
                        lcdMsg("开卡模式", 1, 3 );
			break;

		default:
			break;
	}

	if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
        {
		if( PcdRequest( PICC_REQIDL, &CardRevBuf[0] ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
                {
			display_no_id();
			return;
		}	
	}

	if( PcdAnticoll( &CardRevBuf[2] ) != MI_OK ) //防冲撞，返回卡的序列号 4字节 
	{
		display_no_id();
		return;	
	}

	if( PcdSelect( &CardRevBuf[2] ) != MI_OK )//选卡
	{
		display_no_id();
		return;
        }
	display_id( &CardRevBuf[2] );

	cal_keyA( CardKeyABuf );		//计算密码

	switch( OptMode )
	{
		case OPT_REG_MODE:			//新卡注册
			if( PcdAuthState( PICC_AUTHENT1A, 4, DefaultKeyABuf, &CardRevBuf[2] ) != MI_OK )// 验证密码
			{
				display_error();	
				return;	
			}
			memset( CardWriteBuf, 0, 16 );
			if( PcdWrite( 4, CardWriteBuf ) != MI_OK )// 写卡
			{
				display_error();
				return;	
			}
			memcpy( CardWriteBuf, CardKeyABlockBuf, 16 );
			memcpy( CardWriteBuf, CardKeyABuf, 6 );
			if( PcdWrite( 7, CardWriteBuf ) != MI_OK )// 修改密码
			{
				display_error();
				return;	
			}
                        lcdMsg("注册成功", 1, 3);
			bPass = 1;
			break;
				
		case OPT_ADD_MODE:		//充值方式
			if( PcdAuthState( PICC_AUTHENT1A, 4, CardKeyABuf, &CardRevBuf[2] ) != MI_OK )// 验证密码
			{
				display_error();
				return;	
			}
			if( PcdRead( 4, CardReadBuf ) != MI_OK )// 读数据
			{
				display_error();
				return;	
			}
			memcpy( ( INT8U * )&CardMoney, CardReadBuf, 4 ); 
                        CardMoney += CARDADD;			//充值增加100
			memset( CardWriteBuf, 0, 16 );
			memcpy( CardWriteBuf, ( INT8U * )&CardMoney, 4 ); 
			if( PcdWrite( 4, CardWriteBuf ) != MI_OK )// 写数据
			{
				display_error();
				return;	
			}
			display_money( CardMoney );
			bPass = 1;
			break;
			
		case OPT_INC_MODE:		//消费方式
			if( PcdAuthState( PICC_AUTHENT1A, 4, CardKeyABuf, &CardRevBuf[2] ) != MI_OK )// 验证密码
			{
				display_error();
				return;	
			}
			if( PcdRead( 4, CardReadBuf ) != MI_OK )// 读数据
			{
				display_error();
				return;	
			}
			memcpy( ( INT8U * )&CardMoney, CardReadBuf, 4 ); 
			if( CardMoney < 8 )
			{
				display_error();
				bWarn = 1;
				return;
			}
                        CardMoney -= CARDREDUCE;	//消费 -8
			memset( CardWriteBuf, 0, 16 );
			memcpy( CardWriteBuf, ( INT8U * )&CardMoney, 4 ); 
			if( PcdWrite( 4, CardWriteBuf ) != MI_OK )// 写数据
			{
				display_error();
				return;	
			}
			display_money( CardMoney );
			bPass = 1;
			break;  
			
		case OPT_READ_MODE:		//读卡方式
			if( PcdAuthState( PICC_AUTHENT1A, 4, CardKeyABuf, &CardRevBuf[2] ) != MI_OK )// 验证密码
			{
				display_error();
				return;	
			}
			if( PcdRead( 4, CardReadBuf ) != MI_OK )// 读数据
			{
				display_error();
				return;	
			}
			memcpy( ( INT8U * )&CardMoney, CardReadBuf, 4 ); 
			display_money( CardMoney );
			bPass = 1;
			break;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
			
		default:
			break;
	}
	PcdHalt();	
}

void ctrl_process( void )
{

	if( CtrlMode == CTRL_PC_MODE )
        {
		pc_process();
	}
	else if( CtrlMode == CTRL_BOARD_MODE )
        {
		board_process();
	}

	if( bPass )
        {
		bPass = 0;
		pass();	

	}
	if( bWarn )
	{
		bWarn = 0;
		warn();
	}
}

