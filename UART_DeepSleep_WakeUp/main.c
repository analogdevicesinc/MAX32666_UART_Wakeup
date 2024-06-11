///******************************************************************************
// *
// * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. All Rights Reserved.
// * (now owned by Analog Devices, Inc.),
// * Copyright (C) 2023 Analog Devices, Inc. All Rights Reserved. This software
// * is proprietary to Analog Devices, Inc. and its licensors.
// *
// * Licensed under the Apache License, Version 2.0 (the "License");
// * you may not use this file except in compliance with the License.
// * You may obtain a copy of the License at
// *
// *     http://www.apache.org/licenses/LICENSE-2.0
// *
// * Unless required by applicable law or agreed to in writing, software
// * distributed under the License is distributed on an "AS IS" BASIS,
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// * See the License for the specific language governing permissions and
// * limitations under the License.
/*
 * @file    main.c
 * @brief   Demonstrates the various low power modes.
 *
 * @details Iterates through the various low power modes, using either the RTC
 *          alarm or a GPIO to wake from each.  #defines determine which wakeup
 *          source to use.  Once the code is running, you can measure the
 *          current used on the VCORE rail.
 *
 *          The power states shown are:
 *            1. Active mode power with all clocks on
 *            2. Active mode power with peripheral clocks disabled
 *            3. Active mode power with unused RAMs in light sleep mode
 *            4. Active mode power with unused RAMS shut down
 *            5. SLEEP mode
 *            6. BACKGROUND mode
 *            7. DEEPSLEEP mode
 *            8. BACKUP mode
 */

#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "mxc_errors.h"
#include "nvic_table.h"
#include "pb.h"
#include "led.h"
#include "lp.h"
#include "icc.h"
#include "rtc.h"
#include "uart.h"
#include "simo.h"

/* Shadow register definitions */
#define MXC_R_SIR_SHR17 *((uint32_t *)(0x40005444))


#define USE_CONSOLE 1

//Configuring the Pin P0_10 as input for UART wake up
const mxc_gpio_cfg_t gpio_cfg_uart0a_wakeup = { MXC_GPIO0, (MXC_GPIO_PIN_10), MXC_GPIO_FUNC_ALT3,
                                                MXC_GPIO_PAD_WEAK_PULL_UP, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 };

#if USE_CONSOLE
#define PRINT(...) fprintf(stdout, __VA_ARGS__)
#else
#define PRINT(...)
#endif

// *****************************************************************************


volatile int buttonPressed = 0;
void buttonHandler(void *pb)
{
    buttonPressed = 1;
}


__weak void GPIO0_IRQHandler(void)
{
    MXC_GPIO_Handler(MXC_GPIO_GET_IDX(MXC_GPIO0));
}


static void uartIntHandler(void *data)
{
}


void setTrigger(int waitForTrigger)
{
    int tmp;

    buttonPressed = 0;
    if (waitForTrigger) {
        while (!buttonPressed) {}
    }

    // Debounce the button press.
    for (tmp = 0; tmp < 0x800000; tmp++) {
        __NOP();
    }

// Wait for serial transactions to complete.
#if USE_CONSOLE
    while (MXC_UART_ReadyForSleep(MXC_UART_GET_UART(CONSOLE_UART)) != E_NO_ERROR)
    {

    }
#endif // USE_CONSOLE
}



char read_echo_UART()
{
	char c;
	c = MXC_UART_ReadCharacter(MXC_UART0);
	MXC_UART_WriteCharacter(MXC_UART0,c);
	return c;
}


int main(void)
{
	int error = 0;
    PRINT("\n\n****Low Power Mode Example****\n\n");

    PRINT("This code puts the MAX32666 FTHR into deep sleep mode and then wakes it up using UART0.\n\n");


    PB_RegisterCallback(0, (pb_callback)buttonHandler);

    setTrigger(0);
    /* Prevent SIMO leakage in DS by reducing the SIMO buck clock */
    MXC_R_SIR_SHR17 &= ~(0xC0);


    MXC_LP_USBSWLPDisable();

    MXC_LP_ROM0LightSleepEnable();

    MXC_LP_USBFIFOLightSleepEnable();

    MXC_LP_CryptoLightSleepEnable();

    MXC_LP_SRCCLightSleepEnable();

    MXC_LP_ICacheXIPLightSleepEnable();

    MXC_LP_ICache1LightSleepEnable();

  //  MXC_LP_SysRam5LightSleepEnable();

    MXC_LP_SysRam4LightSleepEnable();

    MXC_LP_SysRam3LightSleepEnable();

    MXC_LP_SysRam2LightSleepEnable();

    MXC_LP_SysRam1LightSleepDisable();

    MXC_LP_SysRam0LightSleepDisable(); // Global variables are in RAM0 and RAM1


    PRINT("All unused RAMs placed in LIGHT SLEEP mode.\n");

    setTrigger(0);

    MXC_LP_ROM0Shutdown();
    MXC_LP_USBFIFOShutdown();
    MXC_LP_CryptoShutdown();
    MXC_LP_SRCCShutdown();
    MXC_LP_ICacheXIPShutdown();
    MXC_LP_ICache1Shutdown();
 //   MXC_LP_SysRam5Shutdown();
    MXC_LP_SysRam4Shutdown();
    MXC_LP_SysRam3Shutdown();
    MXC_LP_SysRam2Shutdown();
    MXC_LP_SysRam1PowerUp();
    MXC_LP_SysRam0PowerUp(); // Global variables are in RAM0 and RAM1

    PRINT("All unused RAMs shutdown.\n");

    setTrigger(0);



	//Initialize UART
	error = MXC_UART_Init(MXC_UART0, 600, MAP_A);
	if (error < E_NO_ERROR)
	{
		printf("-->Error initializing UART: %d\n", error);
		printf("-->Example Failed\n");
		return error;
	}

	PRINT("UART initialization complete.\n");

// Configure wake up from DEEPSLEEP with UART RX pin
    MXC_GPIO_RegisterCallback(&gpio_cfg_uart0a_wakeup, uartIntHandler, NULL);
    MXC_GPIO_IntConfig(&gpio_cfg_uart0a_wakeup, MXC_GPIO_INT_BOTH);
    MXC_GPIO_EnableInt(gpio_cfg_uart0a_wakeup.port, gpio_cfg_uart0a_wakeup.mask);
    NVIC_EnableIRQ((IRQn_Type)MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(gpio_cfg_uart0a_wakeup.port)));
    MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&gpio_cfg_uart0a_wakeup);
    PRINT("Enable GPIO.\n");

    setTrigger(0);
    while (1) {

        PRINT("Entering DEEPSLEEP mode.\n");
        setTrigger(0);
        MXC_LP_EnterDeepSleepMode();
        read_echo_UART();
        PRINT("Waking up from DEEPSLEEP mode.\n");
        read_echo_UART();



    }
}
