/*
* Flash.h
*
*/
#include<xc.h>
#if defined(_PIC12F1501_H_)
//1K
#define FLASH_ROWSIZE 16 //size of a row
#define HEFLASH_START 0x0380 //first address in HE Flash memory
#define HEFLASH_END 0x03FF //last address in HE Flash memory
#elif defined(_PIC16F1503_H_)||defined(_PIC16F1507_H_)||defined(_PIC16F1512_H_)||\
defined(_PIC16F1703_H_)||defined(_PIC16F1707_H_)
//2K
#define FLASH_ROWSIZE 16 //size of a row
#define HEFLASH_START 0x0780 //first address in HE Flash memory
#define HEFLASH_END 0x07FF //last address in HE Flash memory
#elif defined(_PIC16F1508_H_)||defined(_PIC16F1513_H_)||\
defined(_PIC16F1704_H_)||defined(_PIC16F1708_H_)||defined(_PIC16F1713_H_)
//4K
#define FLASH_ROWSIZE 32 //size of a row
#define HEFLASH_START 0x0F80 //first address in HE Flash memory
#define HEFLASH_END 0x0FFF //last address in HE Flash memory
#elif defined(_PIC16F1509_H_)||defined(_PIC16F1526_H_)||\
defined(_PIC16F1454_H_)||defined(_PIC16F1455_H_)||defined(_PIC16F1459_H_)||\
defined(_PIC16F1705_H_)||defined(_PIC16F1709_H_)||\
defined(_PIC16F1716_H_)||defined(_PIC16F1717_H_)
//8K
#define FLASH_ROWSIZE 32 //size of a row
#define HEFLASH_START 0x1F80 //first address in HE Flash memory
#define HEFLASH_END 0x1FFF //last address in HE Flash memory
#elif defined(_PIC16F1518_H)||defined(_PIC16F1519_H)||defined(_PIC16F1527_H_)||\
defined(_PIC16F1718_H_)||defined(_PIC16F1719_H_)
//16K
#define FLASH_ROWSIZE 32 //size of a row
#define HEFLASH_START 0x3F80 //first address in HE Flash memory
#define HEFLASH_END 0x3FFF //last address in HE Flash memory
#endif
#define FLASH_ROWMASK FLASH_ROWSIZE-1
