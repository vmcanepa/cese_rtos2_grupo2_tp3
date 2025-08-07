/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file   : board.h
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

#ifndef BOARD_INC_BOARD_H_
#define BOARD_INC_BOARD_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "stm32f4xx_hal_gpio.h"
/********************** macros ***********************************************/
#define NUCLEO_F103RC		(0)
#define NUCLEO_F401RE		(1)
#define NUCLEO_F446RE		(2)
#define NUCLEO_F429ZI		(3)
#define NUCLEO_F439ZI		(4)
#define NUCLEO_F413ZH		(5)
#define STM32F429I_DISC1	(6)
#define STM32F407G_DISC1	(7)

/* GRUPO2_VIC: Debe definirse desde argumento en compilador. */
#ifndef GRUPO2_VIC
	#define GRUPO2_446
#endif


#ifdef GRUPO2_446
	/* Aca van los defines que activan Jezabel y Mariano para su configuracion: */
	#define BOARD (NUCLEO_F446RE)
	#define BOTONERA	/* tomar los pines de la botonera */

#elif defined(GRUPO2_VIC)
	/* Aca van los defines que activa Victor para su configuracion: */
	#define BOARD (STM32F407G_DISC1)

#endif


/* STM32 Nucleo Boards - 64 Pins */
#if ((BOARD == NUCLEO_F103RC) || (BOARD == NUCLEO_F401RE) || (BOARD == NUCLEO_F446RE))

# ifndef BOTONERA
	#define BTN_A_PIN	B1_Pin
	#define BTN_A_PORT	B1_GPIO_Port
	#define BTN_B_PIN	B1_Pin
	#define BTN_B_PORT	B1_GPIO_Port
	#define BTN_C_PIN	B1_Pin
	#define BTN_C_PORT	B1_GPIO_Port

	#define LED_A_PIN	LD2_Pin
	#define LED_A_PORT	LD2_GPIO_Port
	#define LED_B_PIN	LD2_Pin
	#define LED_B_PORT	LD2_GPIO_Port
	#define LED_C_PIN	LD2_Pin
	#define LED_C_PORT	LD2_GPIO_Port
#endif /* not BOTONERA */

#define BTN_PRESSED	GPIO_PIN_RESET
#define BTN_HOVER	GPIO_PIN_SET

#ifdef GRUPO2_446
	// Leds pullup
	#define LED_ON		GPIO_PIN_RESET
	#define LED_OFF		GPIO_PIN_SET
#else
	#define LED_ON		GPIO_PIN_SET
	#define LED_OFF		GPIO_PIN_RESET
#endif

# ifdef BOTONERA
	// pines y leds de la botonera
	#define LD1_Pin GPIO_PIN_6
	#define LD1_GPIO_Port GPIOA
	#define LD2_Pin GPIO_PIN_5
	#define LD2_GPIO_Port GPIOA
	#define LD3_Pin GPIO_PIN_7
	#define LD3_GPIO_Port GPIOA
	#define BT1_Pin GPIO_PIN_9
	#define BT1_GPIO_Port GPIOA
	#define BT2_Pin GPIO_PIN_6
	#define BT2_GPIO_Port GPIOB
	#define BT3_Pin GPIO_PIN_7
	#define BT3_GPIO_Port GPIOC

	#define LED_RED_PIN		LD1_Pin
	#define LED_BLUE_PIN	LD2_Pin
	#define LED_GREEN_PIN	LD3_Pin
	#define LED_RED_PORT	LD1_GPIO_Port
	#define LED_BLUE_PORT	LD2_GPIO_Port
	#define LED_GREEN_PORT	LD3_GPIO_Port

	#define BTN_PIN		B1_Pin
	#define BTN_PORT	B1_GPIO_Port

#endif /* BOTONERA */

#endif /* STM32 Nucleo Boards - 64 Pins */

/* STM32 Discovery Kits */
#if (BOARD == STM32F407G_DISC1)

#define BTN_A_PIN	B1_Pin
#define BTN_A_PORT	B1_GPIO_Port
#define BTN_B_PIN	B1_Pin
#define BTN_B_PORT	B1_GPIO_Port
#define BTN_C_PIN	B1_Pin
#define BTN_C_PORT	B1_GPIO_Port

#define BTN_PRESSED	GPIO_PIN_SET
#define BTN_HOVER	GPIO_PIN_RESET

#define LED_A_PIN	LD3_Pin
#define LED_A_PORT	LD3_GPIO_Port
#define LED_B_PIN	LD4_Pin
#define LED_B_PORT	LD4_GPIO_Port
#define LED_C_PIN	LD5_Pin
#define LED_C_PORT	LD5_GPIO_Port

#define LED_ON		GPIO_PIN_SET
#define LED_OFF		GPIO_PIN_RESET

#ifndef BOTONERA
	#define BTN_PIN         BTN_A_PIN
	#define BTN_PORT        BTN_A_PORT

	#define LED_RED_PIN     LED_C_PIN
	#define LED_RED_PORT    LED_C_PORT
	#define LED_GREEN_PIN   LED_A_PIN
	#define LED_GREEN_PORT  LED_A_PORT
	#define LED_BLUE_PIN    LED_B_PIN
	#define LED_BLUE_PORT   LED_B_PORT
#endif /* not BOTONERA */

#endif /* STM32 Discovery Kits */

/* STM32 Nucleo Boards - 144 Pins */
#if ((BOARD == NUCLEO_F429ZI) || (BOARD == NUCLEO_F439ZI) || (BOARD == NUCLEO_F413ZH))

#define BTN_A_PIN	USER_Btn_Pin
#define BTN_A_PORT	USER_Btn_GPIO_Port
#define BTN_B_PIN	USER_Btn_Pin
#define BTN_B_PORT	USER_Btn_GPIO_Port
#define BTN_C_PIN	USER_Btn_Pin
#define BTN_C_PORT	USER_Btn_GPIO_Port

#define BTN_PRESSED	GPIO_PIN_SET
#define BTN_HOVER	GPIO_PIN_RESET

#define LED_A_PIN	LD1_Pin
#define LED_A_PORT	LD1_GPIO_Port
#define LED_B_PIN	LD2_Pin
#define LED_B_PORT	LD2_GPIO_Port
#define LED_C_PIN	LD3_Pin
#define LED_C_PORT	LD3_GPIO_Port

#define LED_ON		GPIO_PIN_SET
#define LED_OFF		GPIO_PIN_RESET

#endif /* STM32 Nucleo Boards - 144 Pins */

/* STM32 Discovery Kits */
#if (BOARD == STM32F429I_DISC1)

#define BTN_A_PIN	B1_Pin
#define BTN_A_PORT	B1_GPIO_Port
#define BTN_B_PIN	B1_Pin
#define BTN_B_PORT	B1_GPIO_Port
#define BTN_C_PIN	B1_Pin
#define BTN_C_PORT	B1_GPIO_Port

#define BTN_PRESSED	GPIO_PIN_SET
#define BTN_HOVER	GPIO_PIN_RESET

#define LED_A_PIN	LD3_Pin
#define LED_A_PORT	LD3_GPIO_Port
#define LED_B_PIN	LD4_Pin
#define LED_B_PORT	LD4_GPIO_Port
#define LED_C_PIN	LD4_Pin
#define LED_C_PORT	LD4_GPIO_Port

#define LED_ON		GPIO_PIN_SET
#define LED_OFF		GPIO_PIN_RESET

#endif

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* BOARD_INC_BOARD_H_ */

/********************** end of file ******************************************/
