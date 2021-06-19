/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/* Standard C includes */
#include <stdint.h>

/* Microchip library includes */
#include "system.h"

#include "usb.h"
#include "usb_device_hid.h"
#include "HEFlash.h"
#include "app_device_ReviveUSB.h"
//#include "fixed_address_memory.h"
#include "User.h"

/** DECLARATIONS ***************************************************/
#define MODE_MOUSE		0
#define MODE_KEYBOARD	1
#define	MODE_JOYSTICK	2
#define	HEF_DATA_MODE		0	//	0:���[�h
#define	HEF_DATA_VALUE		1	//	1:�l
#define	HEF_DATA_MODIFIER	2	//	2:Modifier�i�L�[�{�[�h�p�j

#define MASTER_MOUSE_SPEED		50	//	Mouse�̈ړ����x�̒����l
#define MASTER_WHEEL_SPEED		1000

#define MOVE_OFF	0
#define MOVE_ON		1

#define NUM_OF_PINS		36


/** VARIABLES ******************************************************/
const char	c_version[]="M_M002";
uint8_t mouse_buffer[MOUSE_EP_SIZE];
uint8_t joystick_buffer[JOYSTICK_EP_SIZE];
uint8_t keyboard_buffer[KEYBOARD_IN_SIZE]; 
USB_HANDLE lastTransmissionMouse;
USB_HANDLE lastTransmissionJoystick;
USB_HANDLE lastINTransmissionKeyboard;
USB_HANDLE lastOUTTransmissionKeyboard;
USB_HANDLE USBOutHandle;
USB_HANDLE USBInHandle;

uint8_t result_button_press_set[6] = {0,0,0,0,0,0};

char mouse_move_up;
char mouse_move_down;
char mouse_move_left;
char mouse_move_right;
char mouse_wheel_up;
char mouse_wheel_down;

int temp_mouse_move_up = 0;
int temp_mouse_move_down = 0;
int temp_mouse_move_left = 0;
int temp_mouse_move_right = 0;
int temp_mouse_wheel_up = 0;
int temp_mouse_wheel_down = 0;

uint8_t speed_mouse_move_up;
uint8_t speed_mouse_move_down;
uint8_t speed_mouse_move_left;
uint8_t speed_mouse_move_right;
uint8_t speed_mouse_wheel_up;
uint8_t speed_mouse_wheel_down;

//�{�^���ݒ�p�ϐ�
uint8_t HEF_data[NUM_OF_PINS][4];
//�ݒ�l(HEF���g���Ă�̂Ńv���O����ROM�̖���128���[�h�ɔz�u)
//�v���O�����{�̂����̃G���A���g��Ȃ��悤�ɐݒ�
//File -> ProjectProperties -> XC8 Global Options -> XC8 Linker -> Additional Options:��
//--ROM=default,-1f80-1fff�����
//�������ݎ��ɏ㏑���������Ȃ��ꍇ�̓��C�^�̐ݒ��ύX
//Auto select memories and ranges��Manuall��
//File -> ProjectProperties -> PICkit 3 -> Memories to Program
//Preserve Program Memory�Ƀ`�F�b�N
//Preserve Program Memory Range(s)(hex)��
//0x1f80-0x1fff�����

//USB�p�ϐ�
#if defined(FIXED_ADDRESS_MEMORY)
    #if defined(COMPILER_MPLAB_C18)
        #pragma udata MOUSE_DATA_ADDRESS
            uint8_t mouse_input[MOUSE_EP_SIZE];
            uint8_t joystick_input[JOYSTICK_EP_SIZE];
            uint8_t keyboard_input[KEYBOARD_IN_SIZE];
            uint8_t keyboard_output[KEYBOARD_OUT_SIZE];
            uint8_t ToSendDataBuffer[COM_IN_SIZE];
            uint8_t ReceivedDataBuffer[COM_OUT_SIZE];
        #pragma udata

#elif defined(__XC8)
        uint8_t mouse_input[MOUSE_EP_SIZE] MOUSE_DATA_ADDRESS;
        uint8_t joystick_input[JOYSTICK_EP_SIZE] JOYSTICK_DATA_ADDRESS;
        uint8_t keyboard_input[KEYBOARD_IN_SIZE] KEYBOARD_IN_DATA_ADDRESS;
        uint8_t keyboard_output[KEYBOARD_OUT_SIZE] KEYBOARD_OUT_DATA_ADDRESS;
        uint8_t ToSendDataBuffer[COM_IN_SIZE] HID_CUSTOM_IN_DATA_BUFFER_ADDRESS;
        uint8_t ReceivedDataBuffer[COM_OUT_SIZE] HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS;
    #endif
#else
    uint8_t mouse_input[MOUSE_EP_SIZE];
    uint8_t joystick_input[JOYSTICK_EP_SIZE];
    uint8_t keyboard_input[KEYBOARD_IN_SIZE];
    uint8_t keyboard_output[KEYBOARD_OUT_SIZE];
    uint8_t ToSendDataBuffer[COM_IN_SIZE];
    uint8_t ReceivedDataBuffer[COM_OUT_SIZE];
#endif

static uint8_t joystick_input_out_flag = 0;
static uint8_t mouse_input_out_flag = 0;
static uint8_t keyboard_input_out_flag = 0;

void APP_ReviveUSBInit(void)
{
	int fi,fj;

    //initialize the variable holding the handle for the last
    // transmission
    lastTransmissionMouse = 0;
    lastTransmissionJoystick = 0;
    lastINTransmissionKeyboard = 0;
    lastOUTTransmissionKeyboard = 0;
    USBOutHandle = 0;
    USBInHandle = 0;

    joystick_input[0] = 
    joystick_input[1] = 
    joystick_input[2] =
    joystick_input[3] = 0;
    
    mouse_input[0] = 
    mouse_input[1] = 
    mouse_input[2] = 0;

	keyboard_input[0] = 
	keyboard_input[1] = 
	keyboard_input[2] = 
	keyboard_input[3] = 
	keyboard_input[4] = 
	keyboard_input[5] = 
	keyboard_input[6] = 
	keyboard_input[7] = 0;

	mouse_buffer[0] =
	mouse_buffer[1] =
	mouse_buffer[2] =
	mouse_buffer[3] = 0;

	keyboard_buffer[0] = 
	keyboard_buffer[1] = 
	keyboard_buffer[2] = 
	keyboard_buffer[3] = 
	keyboard_buffer[4] = 
	keyboard_buffer[5] = 
	keyboard_buffer[6] = 
	keyboard_buffer[7] = 0;

	joystick_buffer[0] = 0;
	joystick_buffer[1] = 0;
	joystick_buffer[2] = 0x80;
	joystick_buffer[3] = 0x80;

//HEF�̃{�^���ݒ�l��ǂݍ���
	ReadHEF(NUM_OF_PINS,HEF_data);
////����������Ă�����A�S��0�ɂ���
	if(HEF_data[0][0] != 0x00 && HEF_data[0][0] != 0x01 && HEF_data[0][0] != 0x02)
	{
		for(fi = 0;fi < NUM_OF_PINS; fi++)
		{
            HEF_data[fi][0] = 
            HEF_data[fi][1] = 
            HEF_data[fi][2] = 
            HEF_data[fi][3] = 0;
		}
        WriteHEF(NUM_OF_PINS,HEF_data);
	}
    //enable the HID endpoint
    USBEnableEndpoint(MOUSE_EP,USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    USBEnableEndpoint(JOYSTICK_EP,USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    USBEnableEndpoint(KEYBOARD_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    USBEnableEndpoint(COM_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
}
void APP_ReviveUSBTask(void)
{
	int fi,fj;
	int pressed_keys;
	char result_button_press;
	char tmp;

    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;

//--------------------------------------------------------------------
    //	�{�^�����蕔
    GetPad(result_button_press_set);
 //����������Ă��Ȃ��Ƃ��ׂ̈Ƀo�b�t�@�����������Ă���
	pressed_keys = 2;
	mouse_move_up = MOVE_OFF;
	mouse_move_down = MOVE_OFF;
	mouse_move_left = MOVE_OFF;
	mouse_move_right = MOVE_OFF;
	mouse_wheel_up = MOVE_OFF;
	mouse_wheel_down = MOVE_OFF;
	speed_mouse_move_up = 0;
	speed_mouse_move_down = 0;
	speed_mouse_move_left = 0;
	speed_mouse_move_right = 0;

	for(fi = 0;fi < NUM_OF_PINS ; fi++)
	{
		result_button_press = ((result_button_press_set[(fi/6)] & (0x01 << (fi % 6))) ? 1 : 0);

		switch(HEF_data[fi][HEF_DATA_MODE])
		{
			case MODE_MOUSE:
				if(result_button_press == 1)
				{
					if(HEF_data[fi][HEF_DATA_VALUE] == 0)  // ���N���b�N
						mouse_buffer[0] |= 1;
					else if(HEF_data[fi][HEF_DATA_VALUE] == 1)  // �E�N���b�N
						mouse_buffer[0] |= 0x02;
					else if(HEF_data[fi][HEF_DATA_VALUE] == 2)  // �z�C�[���N���b�N
						mouse_buffer[0] |= 0x04;
					else if(HEF_data[fi][HEF_DATA_VALUE] == 3) // ��ړ�
					{
						mouse_move_up = MOVE_ON;
						if(speed_mouse_move_up == 0)
							speed_mouse_move_up = HEF_data[fi][HEF_DATA_MODIFIER];
					}
					else if(HEF_data[fi][HEF_DATA_VALUE] == 4) // ���ړ�
					{
						mouse_move_down = MOVE_ON;
						if(speed_mouse_move_down == 0)
							speed_mouse_move_down = HEF_data[fi][HEF_DATA_MODIFIER];
					}
					else if(HEF_data[fi][HEF_DATA_VALUE] == 5) //���ړ�
					{
						mouse_move_left = MOVE_ON;
						if(speed_mouse_move_left == 0)
							speed_mouse_move_left = HEF_data[fi][HEF_DATA_MODIFIER];
					}
					else if(HEF_data[fi][HEF_DATA_VALUE] == 6) // �E�ړ�
					{
						mouse_move_right = MOVE_ON;
						if(speed_mouse_move_right == 0)
							speed_mouse_move_right = HEF_data[fi][HEF_DATA_MODIFIER];
					}
					else if(HEF_data[fi][HEF_DATA_VALUE] == 7) //�z�C�[����
					{
						mouse_wheel_up = MOVE_ON;
						speed_mouse_wheel_up = HEF_data[fi][HEF_DATA_MODIFIER];
					}
					else if(HEF_data[fi][HEF_DATA_VALUE] == 8) //�z�C�[����
					{
						mouse_wheel_down = MOVE_ON;
						speed_mouse_wheel_down = HEF_data[fi][HEF_DATA_MODIFIER];
					}	
					else if(HEF_data[fi][HEF_DATA_VALUE] == 9) //�}�E�X�ړ����x�ύX
					{
						speed_mouse_move_up = HEF_data[fi][HEF_DATA_MODIFIER];
						speed_mouse_move_down = HEF_data[fi][HEF_DATA_MODIFIER];
						speed_mouse_move_left = HEF_data[fi][HEF_DATA_MODIFIER];
						speed_mouse_move_right = HEF_data[fi][HEF_DATA_MODIFIER];						
					}
					mouse_input_out_flag = 5;
				}
				break;

			case MODE_KEYBOARD:
				if(result_button_press == 1)
				{
					if(pressed_keys != 8)
					{
						keyboard_buffer[0] |= HEF_data[fi][HEF_DATA_MODIFIER];
						keyboard_buffer[pressed_keys] = HEF_data[fi][HEF_DATA_VALUE];
						pressed_keys++;
					}	
					keyboard_input_out_flag = 5;	
				}
				break;

			case MODE_JOYSTICK:
				if(result_button_press == 1)
				{
					if(HEF_data[fi][HEF_DATA_VALUE] & 0x01) //��
						joystick_buffer[3] = 0x0;
					if(HEF_data[fi][HEF_DATA_VALUE] & 0x02) //��
						joystick_buffer[3] = 0xff;
					if(HEF_data[fi][HEF_DATA_VALUE] & 0x04) //��
						joystick_buffer[2] = 0x0;
					if(HEF_data[fi][HEF_DATA_VALUE] & 0x08) //�E
						joystick_buffer[2] = 0xff;

					joystick_buffer[0] |= HEF_data[fi][HEF_DATA_MODIFIER];
					joystick_buffer[1] |= (HEF_data[fi][HEF_DATA_VALUE] >> 4);
					
					joystick_input_out_flag = 5;
				}
				break;
			default:
				break;
		}
	}	

//---------------------------------------------------------------------
//	USB�f�[�^���M��
    if(!HIDTxHandleBusy(lastTransmissionMouse))
    {
	    //�}�E�X�̈ړ�����
	    if(mouse_move_up == MOVE_ON)
	    {	//��ړ�
			temp_mouse_move_up+=speed_mouse_move_up;
			if(temp_mouse_move_up > MASTER_MOUSE_SPEED)
			{
				mouse_buffer[2] = -1 * (temp_mouse_move_up / MASTER_MOUSE_SPEED);
				temp_mouse_move_up = temp_mouse_move_up % MASTER_MOUSE_SPEED;
			}
		}
		else
			temp_mouse_move_up = 0;

	    if(mouse_move_down == MOVE_ON)
	    {	//���ړ�
			temp_mouse_move_down+=speed_mouse_move_down;
			if(temp_mouse_move_down > MASTER_MOUSE_SPEED)
			{
				mouse_buffer[2] = (temp_mouse_move_down / MASTER_MOUSE_SPEED);
				temp_mouse_move_down = temp_mouse_move_down % MASTER_MOUSE_SPEED;
			}
		}
		else
			temp_mouse_move_down = 0;

	    if(mouse_move_left == MOVE_ON)
	    {	//���ړ�
			temp_mouse_move_left+=speed_mouse_move_left;
			if(temp_mouse_move_left > MASTER_MOUSE_SPEED)
			{
				mouse_buffer[1] = -1 * (temp_mouse_move_left / MASTER_MOUSE_SPEED);
				temp_mouse_move_left = temp_mouse_move_left % MASTER_MOUSE_SPEED;
			}
		}
		else
			temp_mouse_move_left = 0;

	    if(mouse_move_right == MOVE_ON)
	    {	//�E�ړ�
			temp_mouse_move_right+=speed_mouse_move_right;
			if(temp_mouse_move_right > MASTER_MOUSE_SPEED)
			{
				mouse_buffer[1] = (temp_mouse_move_right / MASTER_MOUSE_SPEED);
				temp_mouse_move_right = temp_mouse_move_right % MASTER_MOUSE_SPEED;
			}
		}
		else
			temp_mouse_move_right = 0;

	    if(mouse_wheel_up == MOVE_ON)
	    {	//�z�C�[����
			temp_mouse_wheel_up+=speed_mouse_wheel_up;
			if(temp_mouse_wheel_up > MASTER_WHEEL_SPEED)
			{
				mouse_buffer[3] = (temp_mouse_wheel_up / MASTER_WHEEL_SPEED);
				temp_mouse_wheel_up = temp_mouse_wheel_up % MASTER_WHEEL_SPEED;
			}
		}
		else
			temp_mouse_wheel_up = 0;

	    if(mouse_wheel_down == MOVE_ON)
	    {	//�z�C�[����
			temp_mouse_wheel_down+=speed_mouse_wheel_down;
			if(temp_mouse_wheel_down > MASTER_WHEEL_SPEED)
			{
				mouse_buffer[3] = -1 * (temp_mouse_wheel_down / MASTER_WHEEL_SPEED);
				temp_mouse_wheel_down = temp_mouse_wheel_down % MASTER_WHEEL_SPEED;
			}
		}
		else
			temp_mouse_wheel_down = 0;

        //copy over the data to the HID buffer
        //�}�E�X�f�[�^�̑��M
        mouse_input[0] = mouse_buffer[0];
        mouse_input[1] = mouse_buffer[1];
        mouse_input[2] = mouse_buffer[2];
        mouse_input[3] = mouse_buffer[3];

		mouse_buffer[0] =0;
		mouse_buffer[1] =0;
		mouse_buffer[2] =0;
		mouse_buffer[3] =0;
		if( mouse_input_out_flag > 0 )
		{
	        //Send the 8 byte packet over USB to the host.
	        lastTransmissionMouse = HIDTxPacket(MOUSE_EP, (uint8_t*)&mouse_input, sizeof(mouse_input));
	        mouse_input_out_flag--;
	 	}       
    }
    if(!HIDTxHandleBusy(lastINTransmissionKeyboard))
    {	       	//Load the HID buffer
    	keyboard_input[0] = keyboard_buffer[0];
    	keyboard_input[1] = keyboard_buffer[1];
		keyboard_input[2] = keyboard_buffer[2];
	    keyboard_input[3] = keyboard_buffer[3];
    	keyboard_input[4] = keyboard_buffer[4];
    	keyboard_input[5] = keyboard_buffer[5];
    	keyboard_input[6] = keyboard_buffer[6];
    	keyboard_input[7] = keyboard_buffer[7];

		keyboard_buffer[0] =
		keyboard_buffer[2] =
		keyboard_buffer[3] =
		keyboard_buffer[4] =
		keyboard_buffer[5] =
		keyboard_buffer[6] =
		keyboard_buffer[7] = 0;

		if( keyboard_input_out_flag > 0 )
		{
	    	//Send the 8 byte packet over USB to the host.
			lastINTransmissionKeyboard = HIDTxPacket(KEYBOARD_EP, (uint8_t*)keyboard_input, sizeof(keyboard_input));
			keyboard_input_out_flag--;
		}
	}
    if(!HIDTxHandleBusy(lastTransmissionJoystick))
    {
        //Buttons
        joystick_input[0] = joystick_buffer[0];
        joystick_input[1] = joystick_buffer[1];
        //X-Y
        joystick_input[2] = joystick_buffer[2];
        joystick_input[3] = joystick_buffer[3];

		joystick_buffer[0] = 0;
		joystick_buffer[1] = 0;
		joystick_buffer[2] = 0x80;
		joystick_buffer[3] = 0x80;

		if( joystick_input_out_flag > 0 )
		{
        	lastTransmissionJoystick = HIDTxPacket(JOYSTICK_EP, (uint8_t*)&joystick_input, sizeof(joystick_input));
        	joystick_input_out_flag--;
		}      	
    }
    
//---------------------------------------------------------------------
//	USB�f�[�^�ʐM��
    if(!HIDRxHandleBusy(USBOutHandle))				//Check if data was received from the host.
    {
        switch(ReceivedDataBuffer[0])
        {
            case 0x80:  // HEF�Ƀf�[�^��ݒ�	//�s���ԍ�,�f�[�^�̏��Ŋi�[
            	HEF_data[ReceivedDataBuffer[1]][0] = ReceivedDataBuffer[2];
            	HEF_data[ReceivedDataBuffer[1]][1] = ReceivedDataBuffer[3];
            	HEF_data[ReceivedDataBuffer[1]][2] = ReceivedDataBuffer[4];
                WriteHEF(NUM_OF_PINS,HEF_data);
                break;
            case 0x81:  //�{�^���̉�����Ԃ�ԐM
                ToSendDataBuffer[0] = 0x81;				//Echo back to the host PC the command we are fulfilling in the first byte.  In this case, the Get Pushbutton State command.
				ToSendDataBuffer[1] = result_button_press_set[0] & 0x3f;
				ToSendDataBuffer[2] = result_button_press_set[1] & 0x3f;
				ToSendDataBuffer[3] = result_button_press_set[2] & 0x3f;
				ToSendDataBuffer[4] = result_button_press_set[3] & 0x3f;
				ToSendDataBuffer[5] = result_button_press_set[4] & 0x3f;
				ToSendDataBuffer[6] = result_button_press_set[5] & 0x3f;

                if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(COM_EP,(uint8_t*)&ToSendDataBuffer[0],64);
                }
                break;

            case 0x37:	//���݂̐ݒ�󋵂�ԐM����1
                {		//�f�[�^�͈�x��64byte��������Ȃ��̂ŁA���ɕ����đ���
	                if(!HIDTxHandleBusy(USBInHandle))
	                {
						ToSendDataBuffer[0] = 0x37;  	//Echo back to the host the command we are fulfilling in the first byte.  In this case, the Read POT (analog voltage) command.
						for(fi = 0;fi < 18; fi++)
						{
							for(fj = 0;fj < 3;fj++)
							{
								ToSendDataBuffer[fi*3+fj+1] = HEF_data[fi][fj];
							}
						}

		                if(!HIDTxHandleBusy(USBInHandle))
        		        {
	            	        USBInHandle = HIDTxPacket(COM_EP,(uint8_t*)&ToSendDataBuffer[0],64);
	            	    }    
	                }
                }
                break;
            case 0x38:	//���݂̐ݒ�󋵂�ԐM����2
                {
	                if(!HIDTxHandleBusy(USBInHandle))
	                {
						ToSendDataBuffer[0] = 0x38;  	//Echo back to the host the command we are fulfilling in the first byte.  In this case, the Read POT (analog voltage) command.
						for(fi = 0;fi < 18; fi++)
						{
							for(fj = 0;fj < 3;fj++)
							{
								ToSendDataBuffer[fi*3+fj+1] = HEF_data[(fi+18)][fj];
							}
						}

		                if(!HIDTxHandleBusy(USBInHandle))
        		        {
	            	        USBInHandle = HIDTxPacket(COM_EP,(uint8_t*)&ToSendDataBuffer[0],64);
	            	    }    
	                }
                }
                break;
            case 0x55:	//�\�t�g���Z�b�g��������
				UCONbits.SUSPND = 0;		//Disable USB module
				UCON = 0x00;				//Disable USB module
                __delay_ms(20);
            	asm("reset");
            	break;
            case 0x56: // V=0x56 Get Firmware version
                ToSendDataBuffer[0] = 0x56;				//Echo back to the host PC the command we are fulfilling in the first byte.  In this case, the Get Pushbutton State command.
				tmp = strlen(c_version);
				if( 0 < tmp && tmp <= (64-2) )
				{
					for( fi = 0; fi < tmp; fi++ )
					{
						ToSendDataBuffer[fi+1] = c_version[fi];
					}
					// �Ō��NULL������ݒ�
					ToSendDataBuffer[fi+1] = 0x00;
				}
				else
				{
					//�o�[�W����������ُ�
					ToSendDataBuffer[1] = 0x00;
				}				
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(COM_EP,(uint8_t*)&ToSendDataBuffer[0],64);
                }
                break;
        }
         //Re-arm the OUT endpoint for the next packet
        USBOutHandle = HIDRxPacket(COM_EP,(uint8_t*)&ReceivedDataBuffer,64);
    }
//---------------------------------------------------------------------
//	�L�[�{�[�hLED(NumLock,CapsLock,ScrollLock)
    if(HIDRxHandleBusy(lastOUTTransmissionKeyboard) == false)
    {
        if(keyboard_output[0] & 0x01)//NumLock
        {
            LED_ON(1);
        }
        else
        {
            LED_OFF(1);
        }
        if(keyboard_output[0] & 0x02)//CapsLock
        {
            LED_ON(2);
        }
        else
        {
            LED_OFF(2);
        }
        if(keyboard_output[0] & 0x04)//ScrollLock
        {
            LED_ON(3);
        }
        else
        {
            LED_OFF(3);
        }

        lastOUTTransmissionKeyboard = HIDRxPacket(KEYBOARD_EP,(uint8_t*)&keyboard_output,sizeof(keyboard_output));
    }

}
/*******************************************************************************
 End of File
*/
