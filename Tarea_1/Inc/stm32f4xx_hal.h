/*
 * stm32f4xx_hal.h
 *
 *  Created on: Mar 14, 2024
 *      Author: jucifuenteso
 *
 *  Este archivo contiene informacion basica del micro:
 *  - valores de reloj principal
 *  -distribucion basica de la memoria
 *  -posiciones de memoria
 *
 */

#ifndef STM32F4XX_HAL_H_
#define STM32F4XX_HAL_H_

#include <stdint.h>
#include <stddef.h>


#define HSI_CLOCK_SPEED		8000000  //value for the main clock signal(HSI ->High speed internal
#define HSE_CLOCK_SPEED		16000000 //value for the main clock signal(HSI ->High speed external

//#define NOP()   (	asm	(*NOP))
#define NOP()		asm("NOP")
#define __weak		__attribute__((weak))

/*
 * Base addres of Flash and SRAM memories
 * Datasheet, Memory Map, Figure  14
 *
 * (Remember 1KByte = 1024 bytes
 *
 *
 */

#define FLASH_BASE_ADDR			0x08000000U		//MEMORIA DEL PROGRAMA 512K
#define SRAM_BASE_ADDR			0X20000000U     //memoria RAM 128K.

/*NOTA
 * Observar que existen registros  especificos del cortex m4 en la region 0xE0000000U
 *
 */


/*Ahora agregamos la direccion de la memoria base para cada uno de los perifericos
 * que posee el micro
 * figura 14: mapa de buses
 * -APB1 (ADVANCE PERIPHERAL  BUS)
 * -APB2
 * -AHB1 (ADVANCED High-performanced Bus)
 *
 */

/*
 * AHBX and APBX Bus  Peripherals base adresses
 */

#define APB1_BASE_ADDR		0x40000000U
#define APB2_BASE_ADDR		0x40010000U
#define AHB1_BASE_ADDR		0x40020000U
#define AHB2_BASE_ADDR		0x50000000U

/*
 * Y ahora lo mismo para cada una de las posiciones de memoria
 * de cada periferico descritos en la tabla 1 del manual  de referencia en el micro
 * observe que en dicha tabla hay 4 segmentos:
 * APB1, APB2, AHB1, AHB2
 *
 *
 * Comenzar de arriba hacia abajo como se muestra en la tabla.
 * inicia USB_OTG_FS (AHB2)
 */

//POSICIONES DE MEMORIA PEROFERICOS AHB2

#define USB_OTG_FS_BASE_ADDR	(AHB2_BASE_ADDR + 0x0000U)

//Posiciones de memoria para perifericos del AHB1

#define DMA2_BASE_ADDR		(AHB1_BASE_ADDR + 0x6400U)
#define DMA1_BASE_ADDR		(AHB1_BASE_ADDR + 0x6000U)
#define FIR_BASE_ADDR		(AHB1_BASE_ADDR + 0x3C00U)	//FLASH INTERFACE REGISTER...
#define RCC_BASE_ADDR		(AHB1_BASE_ADDR + 0x3800U)
#define CRC_BASE_ADDR		(AHB1_BASE_ADDR + 0x3000U)
#define GPIOH_BASE_ADDR		(AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR		(AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR		(AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR		(AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR		(AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR		(AHB1_BASE_ADDR + 0x0000U)

/*Posiciones de memoria perifericos  APB2*/
//#define SPI5_BASE_ADDR 		(APB2_BASE_ADDR + 0x5000U)
//#define TIM11_BASE_ADDR 	(APB2_BASE_ADDR + 0x4800U)
//#define TIM10_BASE_ADDR 	(APB2_BASE_ADDR + 0x4400U)
//#define TIM9_BASE_ADDR		(APB2_BASE_ADDR + 0x4000U)
//#define EXTI_BASE_ADDR		(APB2_BASE_ADDR + 0x3C00U)
//#define SYSCFG_BASE_ADDR1	(APB2_BASE_ADDR + 0x3800U)
//#define SPI4_BASE_ADDR		(APB2_BASE_ADDR + 0x3400U)
//#define SPI1_BASE_ADDR		(APB2_BASE_ADDR + 0x3000U)
//#define SDIO_BASE_ADDR		(APB2_BASE_ADDR + 0x2C00U)
//#define ADC1_BASE_ADDR		(APB2_BASE_ADDR + 0x2000U)
//#define USART6_BASE_ADDR		(APB2_BASE_ADDR + 0x1400U)
//#define USART1_BASE_ADDR		(APB2_BASE_ADDR + 0x1000U)
//#define TIM1_BASE_ADDR		(APB2_BASE_ADDR + 0x0000U)

/*Posiciones de memoria para perifericos del APB1*/

//#define PWR_BASE_ADDR			(APB1_BASE_ADDR + 0x7000U)
//#define I2C3_BASE_ADDR			(APB1_BASE_ADDR + 0x5C00U)
//#define I2C2_BASE_ADDR			(APB1_BASE_ADDR + 0x5800U)
//#define I2C1_BASE_ADDR			(APB1_BASE_ADDR + 0x5400U)
//#define USART2_BASE_ADDR			(APB1_BASE_ADDR + 0x4400U)
//#define I2Sext_BASE_ADDR			(APB1_BASE_ADDR + 0x4000U)
//#define SPI3_BASE_ADDR			(APB1_BASE_ADDR + 0x3C00U)
//#define SPI2_BASE_ADDR			(APB1_BASE_ADDR + 0x3800U)
//#define I2S2ext_BASE_ADDR			(APB1_BASE_ADDR + 0x3400U)
//#define IWDG_BASE_ADDR			(APB1_BASE_ADDR + 0x3000U)
//#define WWDG_BASE_ADDR			(APB1_BASE_ADDR + 0x2C00U)
//#define RTC_BASE_ADDR			(APB1_BASE_ADDR + 0x2800U)
//#define TIM5_BASE_ADDR			(APB1_BASE_ADDR + 0x0C00U)
//#define TIM4_BASE_ADDR			(APB1_BASE_ADDR + 0x8000U)
//#define TIM3_BASE_ADDR			(APB1_BASE_ADDR + 0x0400U)
//#define TIM2_BASE_ADDR			(APB1_BASE_ADDR + 0x0000U)

/**
 * macros genericos
 */

#define DISABLE			(0)
#define ENABLE			(1)
#define SET				ENABLE
#define CLEAR			DISABLE
#define RESET			DISABLE
#define FLAG_SET		SET
#define FLAG_RESET		RESET
#define I2C_WRITE		(0)
#define I2C_READ		(1)

/*  +++======= INICIO de la descripción de los elementos  que componen el periferico======+++ */

/*Definición de la estructura de datos  que representa a cada uno de los registros que componen
 * el periferico RCC.
 *
 * NOTA: POSICION  DE MEMORIA (OFFSET) DEBE ENCAJAR  PERFECTAMENTE CON LA POSICION DE DE
 * MEMORIA INDICADA EN LA HOJA DE DATOS
 * OBSERVAR QUE ELEMNTOS "RESERVEDX"
 */

typedef struct
{
	volatile uint32_t CR;		//Clock Control Register 		  		ADR_OFFSET:		0x00
	volatile uint32_t PLLCFGR;	//PLL Configuration register 	  		ADR_OFFSET:		0x04
	volatile uint32_t CFGR;		//Clock Configuration Register 	  		ADR_OFFSET:		0x08
	volatile uint32_t CIR;		//Clock Interrupt Register 		  		ADR_OFFSET:		0x0C
	volatile uint32_t AHB1RSTR;	//AHB1 Peripheral resest register 		ADR_OFFSET:		0x10
	volatile uint32_t AHB2RSTR;	//AHB2 Peripheral resest register 		ADR_OFFSET:		0x14
	volatile uint32_t reserved0;//reserved:					  	  		ADR_OFFSET: 	0x18
	volatile uint32_t reserved1;//reserved:					  	  		ADR_OFFSET: 	0x1C
	volatile uint32_t APB1RSTR;	//APB1 Peripheral resest register 		ADR_OFFSET:		0x20
	volatile uint32_t APB2RSTR;	//APB2 Peripheral resest register 		ADR_OFFSET:		0x24
	volatile uint32_t reserved2;//reserved:					  	  		ADR_OFFSET: 	0x28
	volatile uint32_t reserved3;//reserved:					  	  		ADR_OFFSET: 	0x2C
	volatile uint32_t AHB1ENR;	//AHB1 Peripheral clock enable register ADR_OFFSET:		0x30
	volatile uint32_t AHB2ENR;	//AHB2 Peripheral clock enable register ADR_OFFSET:		0x34
	volatile uint32_t reserved4;//reserved:					  	  		ADR_OFFSET: 	0x38
	volatile uint32_t reserved5;//reserved:					  	  		ADR_OFFSET: 	0x3C
	volatile uint32_t APB1ENR;	//APB1 Peripheral clock enable register ADR_OFFSET:		0x40
	volatile uint32_t APB2ENR;	//APB2 Peripheral clock enable register ADR_OFFSET:		0x44
	volatile uint32_t reserved6;//reserved:					  	  		ADR_OFFSET: 	0x48
	volatile uint32_t reserved7;//reserved:					  	  		ADR_OFFSET: 	0x4C
	volatile uint32_t AHB1LPENR;//AHB1 clock enable low power register 	ADR_OFFSET:		0x50
	volatile uint32_t AHB2LPENR;//AHB2 clock enable low power register 	ADR_OFFSET:		0x54
	volatile uint32_t reserved8;//reserved:					  	  		ADR_OFFSET: 	0x58
	volatile uint32_t reserved9;//reserved:					  	  		ADR_OFFSET: 	0x5C
	volatile uint32_t APB1LPENR;//APB1 clock enable low power register 	ADR_OFFSET:		0x60
	volatile uint32_t APB2LPENR;//APB2 clock enable low power register 	ADR_OFFSET:		0x64
	volatile uint32_t reserved10;//reserved:					  	  	ADR_OFFSET: 	0x68
	volatile uint32_t reserved11;//reserved:					  	  	ADR_OFFSET: 	0x6C
	volatile uint32_t BDCR;		 //Backup domain control register		ADR_OFFSET: 	0x70
	volatile uint32_t CSR;		 //clock control & status register		ADR_OFFSET: 	0x74
	volatile uint32_t reserved12;//reserved:					  	  	ADR_OFFSET: 	0x78
	volatile uint32_t reserved13;//reserved:					  	  	ADR_OFFSET: 	0x7C
	volatile uint32_t SSCGR;	 //spread spectrum clock generation reg	ADR_OFFSET: 	0x80
	volatile uint32_t PLLI2SCFGR;//PLLI2S configurtion register			ADR_OFFSET: 	0x84
	volatile uint32_t reserved14;//reserved:					  	  	ADR_OFFSET: 	0x88
	volatile uint32_t DCKCFGR;	 //dedicated Clocks  Configuration Reg	ADR_OFFSET: 	0x8C

} RCC_RegDef_t;

/* Hacemos un "merge", en el cual ubicamos la estructura RCC_RegDef_t a apuntar a la posicion
 * de memoria  correspondiente,de forma que cada variable  dentro de la estructura  coincide
 * con cada uno de los SFR en la memoria del micro-controlador
 */
#define	RCC		((RCC_RegDef_t *) RCC_BASE_ADDR)
/* DESCRIPCION  BIT A BIT DE CADA UNO DE LOS REGISTROS DEL QUE COMPONEN AL PERIFERICO RCC*/
/* 6.3.1 RCC_CR */
/* 6.3.2 RCC_PLLCFGR */
/* 6.3.3 RCC_CFGR */
/* 6.3.4 RCC_CIR */
/* 6.3.5 RCC_AHB1RSTR */
/* 6.3.6 RCC_AHB2RSTR */
/* 6.3.7 RCC_APB1RSTR */
/* 6.3.8 RCC_APB2RSTR */

/*6.3.9 RCC_AHB1ENR */
#define RCC_AHB1ENR_GPIOAEN		0
#define RCC_AHB1ENR_GPIOBEN		1
#define RCC_AHB1ENR_GPIOCEN		2
#define RCC_AHB1ENR_GPIODEN		3
#define RCC_AHB1ENR_GPIOEEN		4
#define RCC_AHB1ENR_GPIOHEN		7
#define RCC_AHB1ENR_CRCEN		12
#define RCC_AHB1ENR_DMA1EN		21
#define RCC_AHB1ENR_DMA2EN		22

/*6.3.10 RCC_AHB2ENR */
/*6.3.11 RCC_APB1ENR */
/*6.3.12 RCC_APB2ENR */
/*6.3.17 RCC_BDCR */
/*6.3.18 RCC_CSR  */


/* ======FIN DE DESCIPCION DE ELEMENTOS QUE COMPONEN EL PERIFERICO ======*/

/* ++++++ Posibles valores de las configuraciones */
/* System clock (main clock) possible options */
/* Microcontroller Clock Output 1 (MC01) options*/
/* Microcontroller preescaler Clock Output1(MC01PRE) options*/
/* Microcontroller preescaler Clock Output1(MC01PRE) options*/
/* Microcontroller Clock Output 2 (MC02) options */

/* +++=======INICIO de la descripcion de los elementos que componen el periferico====+++*/
/* Definicion de la estructura  de datos  que representa a cada uno de los registros
 * que componen el periferico GPIO.
 *
 * Debido  a que el periferico GPIOx es muy simple, no es muy necesario  crear la descriocion
 * bit a bit de cada uno de los registros que componen el dicho periferico
 * pero si es necesario comprender que hace cada registro para cargar la configuracion.
 *
 */

typedef struct
{
	volatile uint32_t MODER;	// port mode  register				ADDR_OFFSET: 	0x00
	volatile uint32_t OTYPER;	// port output type  register		ADDR_OFFSET: 	0x04
	volatile uint32_t OSPEEDR;	// port output speed  register		ADDR_OFFSET: 	0x08
	volatile uint32_t PUPDR;	// port pull-up/pul-down register	ADDR_OFFSET: 	0x0C
	volatile uint32_t IDR;		// port input data register			ADDR_OFFSET: 	0x10
	volatile uint32_t ODR;		// port output data register		ADDR_OFFSET: 	0x14
	volatile uint32_t BSRR;		// port bit set/reset register		ADDR_OFFSET: 	0x18
	volatile uint32_t LCKR;		// port CONFIGURATION LOCK register	ADDR_OFFSET: 	0x1C
	volatile uint32_t AFR[2];	// port alternate function register	ADDR_OFFSET: 	0x20-0x24
}GPIO_TypeDef;

/* Creamos objeto  de la estructura definida y hacemos que quede ubicada
 * exactamente sobre la posicion de memoria donde se encuentra en registro.
 * Debido a que son varios perifericos GPIOx, es necesario hacer la definicion  par cada uno.
 *
 * Tener cuidado que cada elemento cioncida con su respectiva doreccion base.
 * */
#define GPIOA		((GPIO_TypeDef *) GPIOA_BASE_ADDR)
#define GPIOB		((GPIO_TypeDef *) GPIOB_BASE_ADDR)
#define GPIOC		((GPIO_TypeDef *) GPIOC_BASE_ADDR)
#define GPIOD		((GPIO_TypeDef *) GPIOD_BASE_ADDR)
#define GPIOE		((GPIO_TypeDef *) GPIOE_BASE_ADDR)
#define GPIOH		((GPIO_TypeDef *) GPIOH_BASE_ADDR)

/*For testing assert parameters. */
#define IS_GPIO_ALL_INSTANCE(GPIOx) ((GPIOx == GPIOA) || \
									 (GPIOx == GPIOB) || \
									 (GPIOx == GPIOC) || \
									 (GPIOx == GPIOD) || \
									 (GPIOx == GPIOE) || \
									 (GPIOx == GPIOH))

/*
 * typedef struct
 * {
 * 		volatile uint32_t dummy;			//Dummy Example register		ADDR_OFFSET: 0X00
 * }DUMMY_RegDef_t;
 */

/*No es necesario  hacer por ahora */


/*Incluimos las librerias de cada periferico*/
#endif /* _STM32F4XX_HALL_H_ */
