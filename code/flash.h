#ifndef  __flash_h__
#define  __flash_h__

extern INT8U read_eeprom(INT16U addr);
extern void erase_eeprom(INT16U first_addr);
extern void program_eeprom(INT16U addr,INT8U val);
extern void read_eeproms( INT16U StartAddr, INT8U * Buf, INT8U Len );
extern void program_eeproms( INT16U StartAddr, INT8U * Buf, INT8U Len );
extern void init_flash(void);

#endif