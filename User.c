#include "system.h"
 /* ピン配置
 * PORTA,3 _IN3
 * PORTA,4 _IN2
 * PORTA,5 _IN1
 * PORTC,0 _OUT1
 * PORTC,1 _OUT2
 * PORTC,2 _OUT3
 * PORTC,3 _OUT4
 * PORTC,4 _OUT5
 * PORTC,5 _OUT6

 */

//入出力方向設定
void UserInitialize()
{
    ANSELA = 
    ANSELC = 
    LATA = 
    LATC = 0;
//    TRISA = 0xFF;
    TRISC = 0;
    OPTION_REG = 0x00;
}

void GetPad(uint8_t *result_button_press_set)
{
    uint8_t i,a;
    
    result_button_press_set[0] = 
    result_button_press_set[1] = 
    result_button_press_set[2] = 0;
    for(i=0;i<6;i++)
    {
        a = 1<<i;
        TRISC = ~a;
    	__delay_us(4);
        if(!RA3)result_button_press_set[0] |= a;
        if(!RA4)result_button_press_set[1] |= a;
        if(!RA5)result_button_press_set[2] |= a;
    }
    TRISC = 0;
}
void LED_ON(uint8_t Number)
{
}
void LED_OFF(uint8_t Number)
{
}
void User_ISR()
{
}