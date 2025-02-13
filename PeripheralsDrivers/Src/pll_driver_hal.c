/*
 * pll_driver_hal.c
 *
 *  Created on: Feb 6, 2025
 *      Author: leandro
 *
 *
 *
 *
 */

#include <stm32f4xx.h>
#include "pll_driver_hal.h"

uint8_t MAIN_CLOCK;

//Está función configura el reloj para que funcione a 100MHz

void pll_Config_100MHz(void) {

    // 1. Seleccionar HSI como fuente del PLL
    RCC->CR |= RCC_CR_HSION;  // Encender HSI

    while (!(RCC->CR & RCC_CR_HSIRDY));  // Esperar hasta que HSI esté listo

    // 2. Configurar el PLL
    RCC->PLLCFGR = (8 << RCC_PLLCFGR_PLLM_Pos)  |  // PLLM = 8 (16MHz/8 = 2MHz)
                   (100 << RCC_PLLCFGR_PLLN_Pos) |  // PLLN = 100 (2MHz * 100 = 200MHz)
                   (0 << RCC_PLLCFGR_PLLP_Pos)  |  // PLLP = 2 (200MHz / 2 = 100MHz)
                   (0 << RCC_PLLCFGR_PLLSRC_Pos);  // Fuente del PLL: HSI

    // 3. Habilitar el PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));  // Esperar hasta que el PLL esté listo

//     4. Configurar los prescalers de buses
    RCC->CFGR |= (0 << RCC_CFGR_HPRE_Pos)  |  // AHB Prescaler = /1 (100 MHz)
                 (5 << RCC_CFGR_PPRE1_Pos) |  // APB1 Prescaler = /4 (25 MHz)
                 (4 << RCC_CFGR_PPRE2_Pos);  // APB2 Prescaler = /2 (50 MHz)

    // 5. Configurar latencia de Flash según frecuencia
    FLASH->ACR &= ~(0xF << FLASH_ACR_LATENCY_Pos);  // Borrar configuración previa
    FLASH->ACR |= (3 << FLASH_ACR_LATENCY_Pos);  // Configurar 3 WS para 100 MHz

    // 6. Seleccionar PLL como fuente de sistema (SYSCLK)
    RCC->CFGR &= ~(0x3 << RCC_CFGR_SW_Pos);  // Limpiar bits
    RCC->CFGR |= (2 << RCC_CFGR_SW_Pos);  // Seleccionar PLL como SYSCLK
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != (2 << RCC_CFGR_SWS_Pos));  // Esperar cambio

}

void pll_Config_MC01(void){

    // Configurar salida del reloj por MCO1 (PA8) para medir con osciloscopio
    RCC->CFGR |= (3 << RCC_CFGR_MCO1_Pos);  // MCO1 = PLLCLK
    RCC->CFGR |= (4 << RCC_CFGR_MCO1PRE_Pos);  // Divisor /4 → 100 MHz / 4 = 25 MHz

}
//uint8_t pll_GetMainClock(void){
//	if(RCC->CFGR & 0b10){
//		MAIN_CLOCK = PLL;  // Verifica el reloj
//	}
//	else{
//		MAIN_CLOCK = HSI;
//	}
//
//	return MAIN_CLOCK;
//}
