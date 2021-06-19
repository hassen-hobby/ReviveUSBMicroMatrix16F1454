#include <xc.h>
#include "Flash.h"

//ReviveUSB��p�A�擪����w�萔��4byte(2bit,8bit,8bit,�_�~�[)�ǂݏo��
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
//ReviveUSB��p�A�ǂݏo���Ƒ΂ɂȂ�悤�ɏ������݂܂�
//�����������݂͂��Ȃ��o�[�W����
void WriteHEF(const char num,char *HEF_data)
{
    char i,j,gietemp;
    int pmdattemp;

    gietemp = INTCONbits.GIE;
    INTCONbits.GIE = 0;//���荞�݋֎~
    PMCON1bits.CFGS = 0;
    for(i=0;i<((1+HEFLASH_END-HEFLASH_START)/FLASH_ROWSIZE);i++){
        PMADR = HEFLASH_START + i*FLASH_ROWSIZE;
        //�u���b�N����
        PMCON1bits.FREE = 1;//�����t���O
        unlock();
        //���b�`��������
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
        //��������
        PMADR = HEFLASH_START + i*FLASH_ROWSIZE;
        PMDAT = pmdattemp;
        PMCON1bits.LWLO = 0;
        unlock();
    }
    INTCONbits.GIE = gietemp;
}
