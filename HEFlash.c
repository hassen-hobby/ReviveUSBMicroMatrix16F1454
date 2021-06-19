#include <xc.h>
#include "Flash.h"

//ReviveUSB専用、先頭から指定数ｘ4byte(2bit,8bit,8bit,ダミー)読み出す
void ReadHEF(const char num,char *HEF_data)
{
    char i;

    PMADR = HEFLASH_START;
    PMCON1bits.CFGS = 0;
    for(i=0;i<num;i++){
        PMCON1bits.RD = 1;
        _nop();
        _nop();
        *HEF_data++ = PMDATH & 0x03;
        *HEF_data++ = PMDATL;

        PMADR++;
        PMCON1bits.RD = 1;
        _nop();
        _nop();

        *HEF_data++ = PMDATL;
        HEF_data++;
        PMADR++;
    }
}

static void unlock()
{
    PMCON1bits.WREN = 1;
	PMCON2 = 0x55;
	PMCON2 = 0xAA;
	PMCON1bits.WR = 1;
	_nop();
	_nop();
    PMCON1bits.WREN = 0;
}
//ReviveUSB専用、読み出しと対になるように書き込みます
//部分書き込みはしないバージョン
void WriteHEF(const char num,char *HEF_data)
{
    char i,j,gietemp;
    int pmdattemp;

    gietemp = INTCONbits.GIE;
    INTCONbits.GIE = 0;//割り込み禁止
    PMCON1bits.CFGS = 0;
    for(i=0;i<((1+HEFLASH_END-HEFLASH_START)/FLASH_ROWSIZE);i++){
        PMADR = HEFLASH_START + i*FLASH_ROWSIZE;
        //ブロック消去
        PMCON1bits.FREE = 1;//消去フラグ
        unlock();
        //ラッチ書き込み
        PMCON1bits.LWLO = 1;
        for(j=0;j<FLASH_ROWSIZE/2;j++){
            PMDATH = *HEF_data++;
            PMDATL = *HEF_data++;
            if(j==0)
                pmdattemp = PMDAT;
            unlock();
            PMADR++;
            PMDATL = *HEF_data++;
            HEF_data++;
            unlock();
            PMADR++;
        }
        //書き込み
        PMADR = HEFLASH_START + i*FLASH_ROWSIZE;
        PMDAT = pmdattemp;
        PMCON1bits.LWLO = 0;
        unlock();
    }
    INTCONbits.GIE = gietemp;
}
