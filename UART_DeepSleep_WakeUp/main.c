/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. All Rights Reserved.
 * (now owned by Analog Devices, Inc.),
 * Copyright (C) 2023 Analog Devices, Inc. All Rights Reserved. This software
 * is proprietary to Analog Devices, Inc. and its licensors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file    main.c
 * @brief   Main for UART example.
 * @details 
This Project validates the statement: As Per the MAX32666 users guide, the UART can wake up the MCU from DEEPSLEEP without character loss. See [MAX32665/MAX32666
User Guide (analog.com)|https://www.analog.com/media/en/technical-documentation/user-guides/max32665max32666-user-guide.pdf], section 11 UART:
"Wake-up from DEEPSLEEP on UART edge with no character loss".
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "pb.h"
#include "board.h"
#include "mxc_delay.h"
#include "led.h"
#include "mxc_errors.h"
#include "nvic_table.h"
#include "led.h"
#include "lp.h"
#include "icc.h"
#include "rtc.h"
#include "uart.h"
#include "simo.h"
#include "mxc_sys.h"


/* Shadow register definitions */
#define MXC_R_SIR_SHR17 *((uint32_t *)(0x40005444))

/***** Definitions *****/
#define UART_BAUD 115200
#define BUFF_SIZE 512


/***** Functions *****/

char read_echo_UART()
{
	char c;
	while(1)
	{
		c = MXC_UART_ReadCharacter(MXC_UART0);
		MXC_UART_WriteCharacter(MXC_UART0,c);

	}


}



/******************************************************************************/
int main(void)
{

	int error, retVal;
	uint8_t RxData[BUFF_SIZE];

	printf("UART Baud \t: %d Hz\n", UART_BAUD);
	printf("Test Length \t: %d bytes\n\n", BUFF_SIZE);
	memset(RxData, 0x0, BUFF_SIZE);

	//Initialize receive UART
	error = MXC_UART_Init(MXC_UART0, UART_BAUD, MAP_A);
	if (error < E_NO_ERROR)
	{
		printf("-->Error initializing UART: %d\n", error);
		printf("-->Example Failed\n");
		return error;
	}

	printf("-->UART Initialized\n\n");




//	printf("--->All unused RAMs shutdown.\n");
//
//	MXC_R_SIR_SHR17 &= ~(0xC0);
//	MXC_LP_ROM0Shutdown();
//	MXC_LP_USBFIFOShutdown();
//	MXC_LP_CryptoShutdown();
//	MXC_LP_SRCCShutdown();
//	MXC_LP_ICacheXIPShutdown();
//	MXC_LP_ICache1Shutdown();
//	MXC_LP_SysRam5Shutdown();
//	MXC_LP_SysRam4Shutdown();
//	MXC_LP_SysRam3Shutdown();
//	MXC_LP_SysRam2Shutdown();
//	MXC_LP_SysRam1PowerUp();
//	MXC_LP_SysRam0PowerUp(); // Global variables are in RAM0 and RAM1
//
////Selecting Specific UART GPIOs that helps micro to wake up from deep sleep
//	MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&gpio_cfg_uart0a);
//	MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&gpio_cfg_uart0a_flow);
//
//
//
//	retVal = MXC_SYS_ClockSourceEnable(MXC_SYS_CLOCK_HIRC8);
//	if (retVal != E_NO_ERROR)
//	{
//		printf("\nCLOCK EN ERROR\n");
//		return retVal;
//	}
//
//	retVal = MXC_SYS_Clock_Select(MXC_SYS_CLOCK_HIRC8);
//	if (retVal != E_NO_ERROR)
//	{
//		printf("\nCLOCK DIS ERROR\n");
//		return retVal;
//	}
//
//
//	MXC_LP_EnterDeepSleepMode();





	while(1)
	{
		read_echo_UART();


	}
}
