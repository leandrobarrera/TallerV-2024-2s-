/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 * autor 		   : lbarreras
 * fecha 		   : 02/08/2025
 ******************************************************************************
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "systick_driver_hal.h"
#include "pll_driver_hal.h"
#include "main.h"
#include "pwm_driver_hal.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include <math.h>
#define HSI_CLOCK_CONFIGURED				0				// 16 MHz
#define BUFFER_SIZE 						64
#define ARRAY_SIZE 100  // 100 muestras por array

//Handlers GPIO, para los pines. Lo de toda la vida.
GPIO_Handler_t userLed = {0}; 	//	PinH1
GPIO_Handler_t LedA = {0}; 		//	PinB12
GPIO_Handler_t LedB = {0}; 		//	PinA12
GPIO_Handler_t LedC = {0}; 		//	PinC11
GPIO_Handler_t LedD = {0}; 		//	PinC10
GPIO_Handler_t LedE = {0};		//	PinC12
GPIO_Handler_t LedF = {0};		//	PinA11
GPIO_Handler_t LedG = {0}; 		//	PinD2
GPIO_Handler_t pinCollector = {0}; 		//	PinA6
GPIO_Handler_t pinBase = {0}; 		//	PinA7


//variables
uint8_t valor = 0;
uint8_t caso_transistor = 0;  //variable para hacer el switcheo de los transistores y encender el 7segmentos.
uint8_t modo = 0;		//esta variable hace que cuando presionamos el boton y se manda la interrupcion en el micro, sume uno asi misma, cambiando de modo.
uint8_t flag_usart = 0;	//bandera para el usart
uint16_t contador = 0; //contador para el modo 4
uint8_t changeModo = 0; //flag del EXTI 5 para cambiar los modos
uint8_t flag_conteo = 0; //flag del contador para dejar de contar mientras estamos en los otros modos.
uint8_t exti_Data = 0; // guarda el valor del data del EXTi cuando se lanza la interrupcion de Clock del Encoder.
uint8_t flag_clock = 0; //bandera para el flag del clock.
uint16_t exti_conteo = 0; //contador del Encoder.
uint8_t usart_data = 0; //el dato del usart2
uint16_t dutty = 0;
uint32_t adc_data = 0; //valor adc
uint8_t flag_adc = 0; //bandera switch adc
uint32_t voltage_c = 0; //voltaje del collector
uint32_t voltage_b = 0; //voltaje del base
uint16_t adc_array_base[ARRAY_SIZE] = {0};      // array para v_base
uint16_t adc_array_collector[ARRAY_SIZE] = {0}; // array para v_collector
uint8_t index_base = 0;      // indice para base
uint8_t index_collector = 0; // indice para collector


//variables para creacion de numero
uint16_t siete_segmentos = 0;		// # que se imprime en el 7segmentos.
uint8_t unidades = 0;
uint8_t decenas = 0;
uint8_t centenas = 0;
uint8_t millares = 0;
int conteo_ms = 0;		//variable para guardar el conteo de ms del systick

//variables para USART6
USART_Handler_t commSerial={0};
GPIO_Handler_t pinTx ={0};
GPIO_Handler_t pinRx ={0};
uint8_t receivedChar = 	'\0';
uint8_t sendMsg = 0;
char bufferData[64] = {0};
char bufferReception[BUFFER_SIZE];
char userMsg[BUFFER_SIZE] = {0};
char cmd[16];
uint32_t firstParameter;
uint32_t secondParameter;
uint8_t counterReception;


//GPIO actualizados
GPIO_Handler_t switcheoUnidades= {0};  // PinB7
GPIO_Handler_t switcheoDecenas= {0};  // PinC9
GPIO_Handler_t switcheoCentenas= {0};  // PinC8
GPIO_Handler_t switcheoMillares= {0};  // PinC5


//Handlers GPIO
GPIO_Handler_t LedRed = {0}; //	Pines RGB	PinB8
GPIO_Handler_t LedGreen = {0}; //	Pines RGB	PinB9
GPIO_Handler_t LedBlue = {0}; //	Pines RGB	PinA5
GPIO_Handler_t pinSW = {0}; //	PinA4
GPIO_Handler_t pinData = {0}; //	PinA1
GPIO_Handler_t pinClock = {0}; //	PinA0
GPIO_Handler_t pinRX = {0}; //	PinA0
GPIO_Handler_t pinTX = {0}; //	PinA0




//Handlers EXTI
EXTI_Config_t extiSW = {0};
EXTI_Config_t extiClock = {0};
EXTI_Config_t extiReset = {0};



//Handlers para los tres timers que se pidieron
Timer_Handler_t contador_segundos = {0};
Timer_Handler_t blinkyTimer = {0};
Timer_Handler_t display = {0};


// Handler PWM
PWM_Handler_t red_pwm = {0};
PWM_Handler_t base = {0};
PWM_Handler_t collector = {0};

//Maquina de estados finito :D
fsm_states_t fsm_program  = {0};

//Handlers ADC
ADC_Config_t v_base = {0};
ADC_Config_t v_collector = {0};



/*
 * The main function, where EVERYTHING HAPPENS. The magic happens...
 */

// se creo una funcion para inicializar el sistema, esto ayuda a desplazarnos mas rapido en el codigo.

void init_system(void);
void display_numbers(uint8_t valor);			//funcion para que se pinten los numeros, se enciendan los displays, se apaguen, etc.
void switcheo_transistor (uint8_t choose);		//funcion para encender los transistores y hacer el switcheo respectivo
void separador_numero (uint16_t valor);			//funcion para la creacion del numero como tal, ya que no usamos el mismo esquema de la tarea pasada, ahora usamos una funcion que genera los numeros que vamos a pintar en el display.
void giro (void);
void refresh (void);
extern void configMagic(void);
FSM_STATES fsm_function(uint8_t evento);



int main(void)
{
	//configPLL(100);
	init_system();
	config_SysTick_ms(0);
	configMagic();



	//Configurando el Systick
	config_SysTick_ms(HSI_CLOCK_CONFIGURED);

	while(1){

		fsm_function(fsm_program.state);


		}

	}


void init_system(void){
	/*	Configuramos el pin*/

	/*	LedBlinky	*/
	userLed.pGPIOx 							= 	GPIOH;
	userLed.pinConfig.GPIO_PinNumber		=	PIN_1;
	userLed.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	/* Cargamos la configuracion de los registros que gobiernan el puerto */
	gpio_Config(&userLed);

	gpio_WritePin(&userLed, SET);



	/*configuramos los pines, y cargamos las configuraciones asi como aprendimos en la tarea #1, nada nuevo. Aqui
	 * los "leds" son las divisiones que tiene el 7segmentos (a,b,c,d....), entonces los pines van a estas divisiones
	 * y encienden estos "leds" enumerados alfabeticamente. El 7segmentos tambien puede verse como un pin muy grande con
	 * varias conexiones.
	 */

	/*	LedA	*/
	LedA.pGPIOx 						= 	GPIOB;
	LedA.pinConfig.GPIO_PinNumber		=	PIN_12;
	LedA.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedA.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedA.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedA.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedA);



	/*	LedB	*/
	LedB.pGPIOx 						= 	GPIOA;
	LedB.pinConfig.GPIO_PinNumber		=	PIN_12;
	LedB.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedB.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedB.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedB.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedB);

	/*	LedC*/
	LedC.pGPIOx 						= 	GPIOC;
	LedC.pinConfig.GPIO_PinNumber		=	PIN_11;
	LedC.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedC.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedC.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedC.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedC);

	/*	LedD*/
	LedD.pGPIOx 						= 	GPIOC;
	LedD.pinConfig.GPIO_PinNumber		=	PIN_10;
	LedD.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedD.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedD.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedD.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedD);

	/*	LedE*/
	LedE.pGPIOx 						= 	GPIOC;
	LedE.pinConfig.GPIO_PinNumber		=	PIN_12;
	LedE.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedE.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedE.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedE.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedE);

	/*	LedF*/
	LedF.pGPIOx 						= 	GPIOA;
	LedF.pinConfig.GPIO_PinNumber		=	PIN_11;
	LedF.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedF.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedF.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedF.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedF);

	/*	LedG*/
	LedG.pGPIOx 						= 	GPIOD;
	LedG.pinConfig.GPIO_PinNumber		=	PIN_2;
	LedG.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedG.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedG.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	LedG.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedG);

	//la variable switcheo se convirtio en switcheoD (derecho) y switcheoI (izquierdo), ya que cambiamos la configuracion de los transistores, ya no estan mas en base comun.

	/*	switcheoUnidades */
	switcheoUnidades.pGPIOx 						= 	GPIOB;
	switcheoUnidades.pinConfig.GPIO_PinNumber		=	PIN_7;
	switcheoUnidades.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	switcheoUnidades.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	switcheoUnidades.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	switcheoUnidades.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&switcheoUnidades);

	/*	switcheoDecenas */
	switcheoDecenas.pGPIOx 							= 	GPIOC;
	switcheoDecenas.pinConfig.GPIO_PinNumber		=	PIN_9;
	switcheoDecenas.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	switcheoDecenas.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	switcheoDecenas.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	switcheoDecenas.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&switcheoDecenas);

	/*	switcheoCentenas */
	switcheoCentenas.pGPIOx 						= 	GPIOC;
	switcheoCentenas.pinConfig.GPIO_PinNumber		=	PIN_8;
	switcheoCentenas.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	switcheoCentenas.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	switcheoCentenas.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	switcheoCentenas.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&switcheoCentenas);

	/*	switcheoMillares */
	switcheoMillares.pGPIOx 						= 	GPIOC;
	switcheoMillares.pinConfig.GPIO_PinNumber		=	PIN_5;
	switcheoMillares.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	switcheoMillares.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	switcheoMillares.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	switcheoMillares.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&switcheoMillares);


	/*	LedRed*/
	LedRed.pGPIOx 							= 	GPIOB;
	LedRed.pinConfig.GPIO_PinNumber			=	PIN_8;
	LedRed.pinConfig.GPIO_PinMode			=	GPIO_MODE_ALTFN;
	LedRed.pinConfig.GPIO_PinOutputType		=	GPIO_OTYPE_PUSHPULL;
	LedRed.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	LedRed.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	LedRed.pinConfig.GPIO_PinAltFunMode		= 	AF2;
	gpio_Config(&LedRed);
	gpio_WritePin(&LedRed, 0);



	/*	LedBlue*/
	LedBlue.pGPIOx 							= 	GPIOB;
	LedBlue.pinConfig.GPIO_PinNumber		=	PIN_9;
	LedBlue.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedBlue.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedBlue.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	LedBlue.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedBlue);
	gpio_WritePin(&LedBlue, 0);

	/*	LedGreen*/
	LedGreen.pGPIOx 						= 	GPIOA;
	LedGreen.pinConfig.GPIO_PinNumber		=	PIN_5;
	LedGreen.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedGreen.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedGreen.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	LedGreen.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedGreen);
	gpio_WritePin(&LedGreen, 0);

	/* pinCollector */
	pinCollector.pGPIOx 							= 	GPIOA;
	pinCollector.pinConfig.GPIO_PinNumber			=	PIN_6;
	pinCollector.pinConfig.GPIO_PinMode				=	GPIO_MODE_ALTFN;
	pinCollector.pinConfig.GPIO_PinOutputType		=	GPIO_OTYPE_PUSHPULL;
	pinCollector.pinConfig.GPIO_PinOutputSpeed		=	GPIO_OSPEED_FAST;
	pinCollector.pinConfig.GPIO_PinPuPdControl		=	GPIO_PUPDR_NOTHING;
	pinCollector.pinConfig.GPIO_PinAltFunMode		= 	AF2;


	gpio_Config(&pinCollector);


	/* pinBase */
	pinBase.pGPIOx 							= 	GPIOA;
	pinBase.pinConfig.GPIO_PinNumber		=	PIN_7;
	pinBase.pinConfig.GPIO_PinMode			=	GPIO_MODE_ALTFN;
	pinBase.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	pinBase.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	pinBase.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	pinBase.pinConfig.GPIO_PinAltFunMode	= 	AF2;


	gpio_Config(&pinBase);





	//GPIOS del exti, recordar que se usan en modo input.

	/*	GPIO del pinSW */
	pinSW.pGPIOx 							= 	GPIOA;
	pinSW.pinConfig.GPIO_PinNumber			=	PIN_4;
	pinSW.pinConfig.GPIO_PinMode			=	GPIO_MODE_IN;

	gpio_Config(&pinSW);

	extiSW.pGPIOHandler		=		&pinSW;
	extiSW.edgeType			=		EXTERNAL_INTERRUPT_RISING_EDGE;

	exti_Config(&extiSW);

	/*	GPIO del pinData. No necesita todas las configs de pinClock o pinSW. */
	pinData.pGPIOx 							= 	GPIOA;
	pinData.pinConfig.GPIO_PinNumber		=	PIN_1;
	pinData.pinConfig.GPIO_PinMode			=	GPIO_MODE_IN;

	gpio_Config(&pinData);

	/*	GPIO del pinClock */
	pinClock.pGPIOx 						= 	GPIOA;
	pinClock.pinConfig.GPIO_PinNumber		=	PIN_0;
	pinClock.pinConfig.GPIO_PinMode		=	GPIO_MODE_IN;

	gpio_Config(&pinClock);

	extiClock.pGPIOHandler		=		&pinClock;
	extiClock.edgeType			=		EXTERNAL_INTERRUPT_FALLING_EDGE;

	exti_Config(&extiClock);


	/*son GPIO asi que se cargan y configuran muy similar a la tarea 1, solo es
	 * tener presente el handler de exti y agregarlo.
	 */




	/*esto es lo nuevo, esta forma de cargar las configuraciones de los Handlers de los timers es parecida a
	 * los del GPIO, la primera fila para definir el que queremos usar (quedan a eleccion del programador) no obstante
	 * en la tarea se nos pide que usemos ciertos timers con cierto numero especifico de bits, 32 para unos y 16 para otros.
	 * Por tanto se uso esta escogencia. En la segunda fila del prescaler se usa el 16000 para que durante todo este
	 * proceso de interrupciones, y teniendo que el micro va a 16MHz, el 16000 divide esta cifra y la deja en un formato de
	 * 1 ms. Este 1 ms se multiplica por el periodo, y lo elegimos segun lo que requiramos. Para el TIM2 pusimos 1000
	 * que 1ms * 1000, nos da 1000ms lo que es igual a 1 segundo, lo que completa nuestro deseo que este contador de segundos
	 * en efecto, cuente segundos. Para el blinky de manera "estandar" hacemos que vaya a un ratio de 250 ms.
	 */

	blinkyTimer.pTIMx 								= TIM5;
	blinkyTimer.TIMx_Config.TIMx_Prescaler			=10000;  //	Genera incrementos de 1ms
	blinkyTimer.TIMx_Config.TIMx_Period				=2500;  //	el prescaler lo ajusta 1ms, entonces lo quiero a 250ms, y es la multiplicacion de uno con el otro.
	blinkyTimer.TIMx_Config.TIMx_mode				=TIMER_UP_COUNTER;  //
	blinkyTimer.TIMx_Config.TIMx_InterruptEnable	=TIMER_INT_ENABLE;  //


	/*	Configuramos el Timer */
	timer_Config(&blinkyTimer);

	//	Encedemos el Timer.
	timer_SetState(&blinkyTimer,SET);

	display.pTIMx 								= TIM2;
	display.TIMx_Config.TIMx_Prescaler			=16000;  //	Genera incrementos de 1ms
	display.TIMx_Config.TIMx_Period				=4;  //	60FPS ultra calidad gamer. Se tuvo que subir porque no se veía fluido el refresco, antes era 15, que significaban 60 FPS
	display.TIMx_Config.TIMx_mode				=TIMER_UP_COUNTER;  //
	display.TIMx_Config.TIMx_InterruptEnable	=TIMER_INT_ENABLE;  //


	/*	Configuramos el Timer */
	timer_Config(&display);

	//	Encedemos el Timer.
	timer_SetState(&display,SET);


	/* Configuracion del PWM */

	red_pwm.ptrTIMx = TIM4;
	red_pwm.config.channel = PWM_CHANNEL_3;
	red_pwm.config.periodo = 500;
	red_pwm.config.prescaler = 150; // freq 10us
	red_pwm.config.duttyCicle = 1; /* Se define el ciclo de trabajo (dutty cycle) del PWM en 100 (50%) */

	/* Se carga el PWM con los parametros establecidos */
	pwm_Config(&red_pwm);
	pwm_Enable_Output(&red_pwm);

	pwm_Start_Signal(&red_pwm);



	collector.ptrTIMx = TIM3;
	collector.config.channel = PWM_CHANNEL_1;
	collector.config.periodo = 500;
	collector.config.prescaler = 50; // freq
	collector.config.duttyCicle = 500; /* Se define el ciclo de trabajo (dutty cycle) del PWM en 100 (25%) */


	/* Se carga el PWM con los parametros establecidos */
	pwm_Config(&collector);

	pwm_Enable_Output(&collector);

	pwm_Start_Signal(&collector);

	base.ptrTIMx = TIM3;
	base.config.channel = PWM_CHANNEL_2;
	base.config.periodo = 500;
	base.config.prescaler = 50; // freq
	base.config.duttyCicle = 500; /* Se define el ciclo de trabajo (dutty cycle) del PWM en 100 (25%) */


	/* Se carga el PWM con los parametros establecidos */
	pwm_Config(&base);

	pwm_Enable_Output(&base);

	pwm_Start_Signal(&base);





	fsm_program.state = STATE_IDLE;

	/* Configuramos los pines del puerto serial
	 * pin sobre el que funciona el USART6 (TX) */

	pinTx.pGPIOx                          = GPIOC;
	pinTx.pinConfig.GPIO_PinNumber        = PIN_6;
	pinTx.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinAltFunMode	  = AF8;
	pinTx.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_FAST;
	pinTx.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;

	/* Escribimos la configuracion en la memoria del MCU */
	gpio_Config(&pinTx);

	/* Pin sobre el que funciona el USART2 (RX) */

	pinRx.pGPIOx                          = GPIOC;
	pinRx.pinConfig.GPIO_PinNumber        = PIN_7;
	pinRx.pinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinAltFunMode	  = AF8;
	pinRx.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_PUSHPULL;
	pinRx.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_FAST;
	pinRx.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;

	/* Escribimos la configuracion en la memoria del MCU */
	gpio_Config(&pinRx);



	//Configuramos el puerto serial (USART2)
	commSerial.ptrUSARTx				= USART6;
	commSerial.USART_Config.baudrate    = USART_BAUDRATE_115200;
	commSerial.USART_Config.datasize	= USART_DATASIZE_8BIT;
	commSerial.USART_Config.parity		= USART_PARITY_NONE;
	commSerial.USART_Config.stopbits	= USART_STOPBIT_1;
	commSerial.USART_Config.mode 		= USART_MODE_RXTX;
	commSerial.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;

	/* Escribimos la configuracion en la memoria del MCU */
	usart_Config(&commSerial);

	usart_WriteChar(&commSerial, '\0');



	/* Configuramos el ADC */

	//ADC para leer voltaje de colector. Pin PC0

	v_collector.channel 			= CHANNEL_10;
	v_collector.resolution 			= RESOLUTION_12_BIT;
	v_collector.dataAlignment 		= ALIGNMENT_RIGHT;
	v_collector.interruptState 		= ADC_INT_ENABLE;
	v_collector.samplingPeriod 		= SAMPLING_PERIOD_112_CYCLES;

	//cargamos la config.
	adc_ConfigSingleChannel(&v_collector);
	adc_StartSingleConv();

	//ADC para leer voltaje de base. PinPC1

	v_base.channel 					= CHANNEL_11;
	v_base.resolution 				= RESOLUTION_12_BIT;
	v_base.dataAlignment 			= ALIGNMENT_RIGHT;
	v_base.interruptState 			= ADC_INT_ENABLE;
	v_base.samplingPeriod 			= SAMPLING_PERIOD_112_CYCLES;

	// no se carga esta config porque solo puede haber uno cargado, por algo se llama configsinglechannel

	//cargamos la config.
	//adc_ConfigSingleChannel(&v_base);
	//adc_StartSingleConv();






}



/*
 *  Overwrite function
 **/

//funcion para pintar los numeros, seleccionamos los leds que se encienden para hacer X numero.

void display_numbers (uint8_t valor){
	switch (valor){
		case 0: {
					gpio_WritePin(&LedA, 0);
					gpio_WritePin(&LedB, 0);
					gpio_WritePin(&LedC, 0);
					gpio_WritePin(&LedD, 0);
					gpio_WritePin(&LedE, 0);
					gpio_WritePin(&LedF, 0);
					gpio_WritePin(&LedG, 1);
					break;
				}
		case 1:{
					gpio_WritePin(&LedA, 1);
					gpio_WritePin(&LedB, 0);
					gpio_WritePin(&LedC, 0);
					gpio_WritePin(&LedD, 1);
					gpio_WritePin(&LedE, 1);
					gpio_WritePin(&LedF, 1);
					gpio_WritePin(&LedG, 1);
					break;
				}
		case 2:{
					gpio_WritePin(&LedA, 0);
					gpio_WritePin(&LedB, 0);
					gpio_WritePin(&LedC, 1);
					gpio_WritePin(&LedD, 0);
					gpio_WritePin(&LedE, 0);
					gpio_WritePin(&LedF, 1);
					gpio_WritePin(&LedG, 0);
					break;
				}
		case 3:{
					gpio_WritePin(&LedA, 0);
					gpio_WritePin(&LedB, 0);
					gpio_WritePin(&LedC, 0);
					gpio_WritePin(&LedD, 0);
					gpio_WritePin(&LedE, 1);
					gpio_WritePin(&LedF, 1);
					gpio_WritePin(&LedG, 0);
					break;
				}
		case 4:{
					gpio_WritePin(&LedA, 1);
					gpio_WritePin(&LedB, 0);
					gpio_WritePin(&LedC, 0);
					gpio_WritePin(&LedD, 1);
					gpio_WritePin(&LedE, 1);
					gpio_WritePin(&LedF, 0);
					gpio_WritePin(&LedG, 0);
					break;
				}
		case 5:{
					gpio_WritePin(&LedA, 0);
					gpio_WritePin(&LedB, 1);
					gpio_WritePin(&LedC, 0);
					gpio_WritePin(&LedD, 0);
					gpio_WritePin(&LedE, 1);
					gpio_WritePin(&LedF, 0);
					gpio_WritePin(&LedG, 0);
					break;
				}
		case 6:{
					gpio_WritePin(&LedA, 0);
					gpio_WritePin(&LedB, 1);
					gpio_WritePin(&LedC, 0);
					gpio_WritePin(&LedD, 0);
					gpio_WritePin(&LedE, 0);
					gpio_WritePin(&LedF, 0);
					gpio_WritePin(&LedG, 0);
					break;
				}
		case 7:{
					gpio_WritePin(&LedA, 0);
					gpio_WritePin(&LedB, 0);
					gpio_WritePin(&LedC, 0);
					gpio_WritePin(&LedD, 1);
					gpio_WritePin(&LedE, 1);
					gpio_WritePin(&LedF, 1);
					gpio_WritePin(&LedG, 1);
					break;
				}
		case 8:{
					gpio_WritePin(&LedA, 0);
					gpio_WritePin(&LedB, 0);
					gpio_WritePin(&LedC, 0);
					gpio_WritePin(&LedD, 0);
					gpio_WritePin(&LedE, 0);
					gpio_WritePin(&LedF, 0);
					gpio_WritePin(&LedG, 0);
					break;
				}
		case 9:{
					gpio_WritePin(&LedA, 0);
					gpio_WritePin(&LedB, 0);
					gpio_WritePin(&LedC, 0);
					gpio_WritePin(&LedD, 1);
					gpio_WritePin(&LedE, 1);
					gpio_WritePin(&LedF, 0);
					gpio_WritePin(&LedG, 0);
					break;
				}
		}
}


//aqui es donde se supone que esta generando el fantasma en DECENAS, no se pudo encontrar una forma de solucionarlo. Quedo poder ver y preguntar.
//lo que hacemos primero es apagar los pines que switchean todo lo que no sea unidades (para el primer caso por ejemplo), y si, 1 es apagar 0 encender. Usamos display_number para encender lo que requerimos y asi se completa unidades, lo mismo para los otros.
void switcheo_transistor (uint8_t choose){
	switch(choose){
	case 0:{
		gpio_WritePin(&switcheoCentenas,0);
		gpio_WritePin(&switcheoDecenas,0);
		gpio_WritePin(&switcheoMillares,0);
		display_numbers(unidades);
		gpio_WritePin(&switcheoUnidades,1);
		break;
		}
	case 1:{
		gpio_WritePin(&switcheoCentenas,0);
		gpio_WritePin(&switcheoMillares,0);
		gpio_WritePin(&switcheoUnidades,0);
		display_numbers(decenas);
		gpio_WritePin(&switcheoDecenas,1);
		break;
		}
	case 2:{
		gpio_WritePin(&switcheoUnidades,0);
		gpio_WritePin(&switcheoDecenas,0);
		gpio_WritePin(&switcheoMillares,0);
		display_numbers(centenas);
		gpio_WritePin(&switcheoCentenas,1);
		break;
		}
	case 3:{
		gpio_WritePin(&switcheoCentenas,0);
		gpio_WritePin(&switcheoDecenas,0);
		gpio_WritePin(&switcheoUnidades,0);
		display_numbers(millares);
		gpio_WritePin(&switcheoMillares,1);
		break;
		}
	}
}


//funcion para separar el numero, asi recibimos las unidades, decenas, centenas y millares del numero que nos arroje las lecturas del conteo, etc.
void separador_numero (uint16_t valor){

	uint16_t numero = 0;
	// supngamos que numero es igual 1234
	numero = valor;
	// residuo 4,
	unidades = numero % 10;
	// cociente 123
	numero = numero/10;
	// residuo 3
	decenas = numero % 10;
	// cociente 12
	numero = numero/10;
	// residuo 2
	centenas = numero % 10;
	// cociente 1
	numero = numero/10;
	millares =numero%10;



}

void giro (void){
	if(exti_Data){

		if(exti_conteo == 4095){
			exti_conteo = 0;
			}
		else{ printf("CW derecha\n");
			exti_conteo ++;
		}
	}
	//se hicieorn correctamente los reinicios tanto si pasamos de 4095 a como si nos devolvemos desde 0.
		else{
			if(exti_conteo == 0){
				exti_conteo = 4095;
			}
			else{ printf("CCW izquierda\n");
				exti_conteo--;
			}
		}
}

void refresh (void){
	siete_segmentos = exti_conteo;
	separador_numero(siete_segmentos);
		switch(caso_transistor){
			case 0:{
				switcheo_transistor(caso_transistor);
				caso_transistor ++;
				break;
			}
			case 1:{
				switcheo_transistor(caso_transistor);
				caso_transistor ++;
				break;
			}
			case 2:{
				switcheo_transistor(caso_transistor);
				caso_transistor ++;
				break;
			}
			case 3:{
				switcheo_transistor(caso_transistor);
				caso_transistor = 0;
				break;
			}

		}
}

void analizeRecievedChar(void){
	if(commSerial.receivedChar !='\0'){
		bufferReception[counterReception] = commSerial.receivedChar;
		counterReception++;

		if (commSerial.receivedChar == '@'){
			bufferReception[counterReception] = '\0';
			counterReception = 0;
			fsm_program.state = STATE_COMMAND_COMPLETE;
		}
	}
}




/* Función encargada de analizar los comandos percibidos por el puerto serial */
void parseCommands(char *ptrBufferReception){
    // Esta función de C lee la cadena de caracteres a la que apunta el "ptr" y  la divide
    // y almacena en tres elementos diferentes: un string llamado "cmd" y dos numeros
    // integer llamados ""firstParameter" y "SecondParameter".
    // De esta forma, podemos introducir información al micro desde el puerto serial
    sscanf(ptrBufferReception, "%s %lu %lu %s", cmd, &firstParameter, &secondParameter, userMsg);

    // Este primer comando imprime una lista con los otros comandos que tiene el equipo
    if (strcmp(cmd, "help") == 0){
        usart_writeMsg(&commSerial, "Help Menu CMDs:\n");
        usart_writeMsg(&commSerial, "1) help            -- Print this menu\n");
        usart_writeMsg(&commSerial, "2) dummy #A #B    -- dummy cmd, #A and #B are uint32_t\n");
        usart_writeMsg(&commSerial, "3) setPeriod    -- insert the period of the blinky\n");
        usart_writeMsg(&commSerial, "4) setFreq # -- Change the Led_state period (ms) \n");
        usart_writeMsg(&commSerial, "5) setDutty # -- Change the duty cycle (%), ENTRE 1 Y 99:LED & 1-1000 para FRC\n");
        usart_writeMsg(&commSerial, "6) setNumber # -- Change displayed number\n");
        usart_writeMsg(&commSerial, "7) setVoltage # -- PWM-DAC output in mV\n");
        usart_writeMsg(&commSerial, "8) setVoltB # -- PWM-DAC output for base (mV)\n");
        usart_writeMsg(&commSerial, "9) setVoltC # -- PWM-DAC output for collector (mV)\n");
        usart_writeMsg(&commSerial, "10) readVoltB # -- ADC value for base (mV)\n");
        usart_writeMsg(&commSerial, "11) readVoltC # -- ADC value for collector (mV)\n");
        usart_writeMsg(&commSerial, "12) analyzeIcVb # -- Curva Ic vs Vb (mV)\n");
    }

    // El comando dummy sirve para entender como funciona la recepción de números enviados
    // desde la consola
    else if (strcmp(cmd, "dummy") == 0){
        usart_writeMsg(&commSerial, "CMD: dummy\n");
        // Cambiando el formato para presentar el número por el puerto serial
        sprintf(bufferData, "number A = %lu \n", firstParameter);
        usart_writeMsg(&commSerial, bufferData);

        // Cambiando el formato para presentar el número por el puerto serial
        sprintf(bufferData, "number B = %lu \n", secondParameter);
        usart_writeMsg(&commSerial, bufferData);
    }


    else if (strcmp(cmd, "setPeriod")	== 0){
		usart_writeMsg(&commSerial, "cmd: setPeriod\n");
		sprintf(bufferData, "El periodo para el blinky será de: %lu ms\n", firstParameter);
		usart_writeMsg(&commSerial, bufferData);
			if (firstParameter>=100 && firstParameter<=1500){
				timer_SetState(&blinkyTimer, RESET);
				blinkyTimer.TIMx_Config.TIMx_Period = firstParameter;
				timer_Config(&blinkyTimer);
				timer_SetState(&blinkyTimer, SET);
			}

			else {
			usart_writeMsg(&commSerial, "Ponga un número entre 100 y 1500 ms.\n");
			}
		}


    else if (strcmp(cmd, "setNumber")==0){
		usart_writeMsg(&commSerial, "cmd: setNumber\n");
		sprintf(bufferData, "Número que se verá en el display = %lu\n",firstParameter);
		usart_writeMsg(&commSerial, bufferData);
			if (firstParameter>4095) {
				exti_conteo = 4095;
				usart_writeMsg(&commSerial, "número por encima del máximo permitido, asignándole el valor max\n");
    			}
    			else if(firstParameter<=4095){
    				exti_conteo = firstParameter;
    			}
    			else{
    				usart_writeMsg(&commSerial, "Error, numero fuera de rango (0-4095)");
    			}
    			fsm_program.state = STATE_REFRESH_DISPLAY;
    			}

	else if (strcmp(cmd, "setFreq")==0){
		usart_writeMsg(&commSerial, "cmd: setFreq\n");
			if (firstParameter == 1){
				sprintf(bufferData, "Valor del nuevo periodo en ms: %lu\n", secondParameter);
				usart_writeMsg(&commSerial, bufferData);
					if (secondParameter>0){
						pwm_Update_Frequency(&red_pwm, secondParameter);
						usart_writeMsg(&commSerial, "Freq RGB updated\n");
					}
					else{
						usart_writeMsg(&commSerial, "el valor del periodo debe ser mayor a cero\n");
					}

				}
			else if (firstParameter == 2){
				sprintf(bufferData, "Valor de la nueva freq^⁻1 en ms: %lu\n", secondParameter);
				usart_writeMsg(&commSerial, bufferData);
				if (secondParameter>0){
					//pwm_Update_Frequency(&filtroRC, secondParameter);
					usart_writeMsg(&commSerial, "Freq RC updated\n");
				}
				else{
					usart_writeMsg(&commSerial, "el valor del periodo debe ser mayor a cero\n");
				}
			}
	}


	else if (strcmp(cmd, "setDutty")==0){
		usart_writeMsg(&commSerial, "cmd: setDutty\n");
			if (firstParameter == 1){
				sprintf(bufferData, "Valor del nuevo duty: %lu\n", secondParameter);
				usart_writeMsg(&commSerial, bufferData);
					if (secondParameter>=1 && secondParameter<=100  ){
						pwm_Update_DuttyCycle(&red_pwm, secondParameter);
						usart_writeMsg(&commSerial, "RGB dutty cicle updated\n");
				}
			else{
				usart_writeMsg(&commSerial, "El valor del dutty debe estar entre 1-100 \n");
			}
		}

		else if (firstParameter ==2){
			sprintf(bufferData, "Valor del nuevo duty: %lu\n", secondParameter);
			usart_writeMsg(&commSerial, bufferData);
				if (secondParameter<=1000 && secondParameter>=1 ){
					//pwm_Update_DuttyCycle(&filtroRC, secondParameter);
					usart_writeMsg(&commSerial, "Duty cicle updated\n");
				}
				else{
					usart_writeMsg(&commSerial, "El valor del duty debe estar entre 1-1000 \n");
				}
				}
				else{
					usart_writeMsg(&commSerial, "Elija 1 para cambiar el duty del RGB o 2 para cambiar el del RC\n");
				}
		}

	else if (strcmp(cmd, "setVoltage")==0){
			usart_writeMsg(&commSerial, "cmd: setVoltage\n");
			if (firstParameter<=3300 && firstParameter>=1){

				//dutty = filtroRC.config.periodo * firstParameter / 3300;
				//dutty = (int) (firstParameter * 2)/33 ;
				//pwm_Update_DuttyCycle(&filtroRC, dutty);
				sprintf(bufferData, "Voltage: %lu mV\n", firstParameter);
				usart_writeMsg(&commSerial, bufferData);
			}
			else{
				usart_writeMsg(&commSerial, "Error: voltaje debe estar entre 1 y 3300mv");
			}
		}

	else if (strcmp(cmd, "setVoltB")==0){
		usart_writeMsg(&commSerial, "cmd: setVoltB\n");
		if (firstParameter<=3300 && firstParameter>=1){
			dutty = base.config.periodo * firstParameter / 3300;
			pwm_Update_DuttyCycle(&base, dutty);
			sprintf(bufferData, "Voltage: %lu mV\n", firstParameter);
			usart_writeMsg(&commSerial, bufferData);


		}
		else{
			usart_writeMsg(&commSerial, "Error: voltaje debe estar entre 1 y 3300mv");
		}


	}

	else if (strcmp(cmd, "setVoltC")==0){
		usart_writeMsg(&commSerial, "cmd: setVoltC\n");
		if (firstParameter<=3300 && firstParameter>=1){
			dutty = collector.config.periodo * firstParameter / 3300;
			pwm_Update_DuttyCycle(&collector, dutty);
			sprintf(bufferData, "Voltage: %lu mV\n", firstParameter);
			usart_writeMsg(&commSerial, bufferData);
		}
		else{
			usart_writeMsg(&commSerial, "Error: voltaje debe estar entre 1 y 3300mv");
		}


	}

	else if (strcmp(cmd, "readVoltB")==0){
		usart_writeMsg(&commSerial, "Read voltage of base (mV) \n");
		//adc_ConfigSingleChannel(&v_base);
		//adc_StartSingleConv();
		sprintf(bufferData, "Voltage of base: %lu mV\n", voltage_b);
		usart_writeMsg(&commSerial, bufferData);


	}

	else if (strcmp(cmd, "readVoltC")==0){
		usart_writeMsg(&commSerial, "Read voltage of collector (mV) \n");
		//adc_ConfigSingleChannel(&v_collector);
		//adc_StartSingleConv();
		sprintf(bufferData, "Voltage of collector: %lu mV\n", voltage_c);
		usart_writeMsg(&commSerial, bufferData);


	}

	else if (strcmp(cmd, "analyzeIcVb") == 0) {
	    uint16_t baseVoltages[137];  // Voltajes en la base (mV)
	    uint16_t collectorVoltages[137];  // Voltajes en el colector (mV)
	    float collectorCurrents[137];  // Corriente en el colector (mA)
	    uint16_t resistanceC = 220;  // Resistencia del colector en ohmios
	    uint16_t supplyVoltage = 1000;  // Voltaje de alimentación en mV
	    float leakageCurrent = 1e-12;  // Corriente de saturación inversa (1 pA)
	    float thermalVoltage = 25.0;  // Voltaje térmico en mV
	    float minBaseVoltage = 0.5;  // Voltaje base mínimo (500 mV)
	    float maxBaseVoltage = 0.85;  // Ajustado para alcanzar Ic ≈ 13mA
	    char formattedData[64];  // Buffer para salida formateada

	    usart_writeMsg(&commSerial, "Vb (mV), Vc (mV), Ic (mA)\n");

	    for (int index = 0; index < 137; index++) {
	        // Generar valores de Vb de forma equidistante
	        float baseEmitterVoltage = minBaseVoltage + ((maxBaseVoltage - minBaseVoltage) * (float)index / 136);
	        baseVoltages[index] = (uint16_t)(baseEmitterVoltage * 1000);  // Convertir a mV

	        // Calcular corriente del colector usando ecuación exponencial
	        collectorCurrents[index] = leakageCurrent * (exp(baseEmitterVoltage / thermalVoltage) - 1);
	        collectorCurrents[index] *= 1000;  // Convertir a mA

	        // Simular el voltaje del colector considerando caída en Rc
	        collectorVoltages[index] = supplyVoltage - (uint16_t)(collectorCurrents[index] * resistanceC);

	        // Extraer parte entera y decimal de la corriente
	        int integerPart = (int)collectorCurrents[index];
	        int decimalPart = (int)roundf((collectorCurrents[index] - integerPart) * 100);

	        // Formatear salida como CSV en formato "Vb, Vc, Ic"
	        sprintf(formattedData, "%u, %u, %d.%02d\n", baseVoltages[index], collectorVoltages[index], integerPart, decimalPart);
	        usart_writeMsg(&commSerial, formattedData);
	    }
	}


	 else{
	        // Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados.
	        usart_writeMsg(&commSerial, "Wrong CMD\n");
	    }



    }























FSM_STATES fsm_function(uint8_t evento){

	switch(evento){
	case STATE_IDLE:{


		break;
	}
	case STATE_MODO_RGB:{

		printf("SW\n");
		printf("ms: %d\n", conteo_ms);


		switch(modo){

			case 0:
				gpio_WritePin(&LedGreen, 0);
				pwm_Update_DuttyCycle(&red_pwm, 99);
				//pwm_Start_Signal(&red_pwm);
				//gpio_WritePin(&LedRed, 1);
				gpio_WritePin(&LedBlue, 0);
				modo ++;

				break;
			case 1:
				gpio_WritePin(&LedGreen, 1);
				//gpio_WritePin(&LedRed, 0);
				pwm_Update_DuttyCycle(&red_pwm, 1);
				//pwm_Stop_Signal(&red_pwm);
				//pwm_Disable_Output(&red_pwm);
				gpio_WritePin(&LedBlue, 0);
				modo ++;
				break;
			case 2:
				gpio_WritePin(&LedGreen, 0);
				pwm_Update_DuttyCycle(&red_pwm, 1);
				//pwm_Stop_Signal(&red_pwm);
				//pwm_Disable_Output(&red_pwm);
				//gpio_WritePin(&LedRed, 0);
				gpio_WritePin(&LedBlue, 1);
				modo ++;
				break;
			case 3:
				gpio_WritePin(&LedGreen, 1);
				pwm_Update_DuttyCycle(&red_pwm, 1);
				//pwm_Stop_Signal(&red_pwm);
				//pwm_Disable_Output(&red_pwm);
				//gpio_WritePin(&LedRed, 0);
				gpio_WritePin(&LedBlue, 1);
				modo ++;
				break;
			case 4:
				gpio_WritePin(&LedGreen, 0);
				pwm_Update_DuttyCycle(&red_pwm, 99);
				//pwm_Start_Signal(&red_pwm);
				//gpio_WritePin(&LedRed, 1);
				gpio_WritePin(&LedBlue, 1);
				modo ++;
				break;
			case 5:
				gpio_WritePin(&LedGreen, 1);
				//gpio_WritePin(&LedRed, 1);
				pwm_Update_DuttyCycle(&red_pwm, 99);
				//pwm_Start_Signal(&red_pwm);
				gpio_WritePin(&LedBlue, 0);
				modo ++;
				break;
			case 6:
				gpio_WritePin(&LedGreen, 1);
				//gpio_WritePin(&LedRed, 1);
				pwm_Update_DuttyCycle(&red_pwm, 99);
				//pwm_Start_Signal(&red_pwm);
				gpio_WritePin(&LedBlue, 1);
				modo ++;
				break;
			case 7:
				gpio_WritePin(&LedGreen, 0);
				//gpio_WritePin(&LedRed, 0);
				gpio_WritePin(&LedBlue, 0);
				pwm_Update_DuttyCycle(&red_pwm, 1);
				//pwm_Stop_Signal(&red_pwm);
				//pwm_Disable_Output(&red_pwm);
				modo = 0;
				break;

			}
		fsm_program.state = STATE_IDLE;
		break;
	}

	case STATE_GIRO_ENCODER:{

		giro();
		refresh();
		fsm_program.state = STATE_IDLE;
		break;

	}

	case STATE_REFRESH_DISPLAY:{
		refresh();
		adc_StartSingleConv();
		fsm_program.state = STATE_IDLE;
		break;
	}

	case STATE_CHAR_RECEIVED:{
		fsm_program.state = STATE_IDLE;
		analizeRecievedChar();
			if(fsm_program.state == STATE_COMMAND_COMPLETE){
				parseCommands(bufferReception);
			}
			fsm_program.state = STATE_IDLE;
			break;

	}

	case STATE_COMMAND_COMPLETE:{
break;
	}
	case STATE_READ_ADC:{
		fsm_program.state = STATE_IDLE;
		uint16_t adc_value = adc_GetValue(); // Leer ADC (12 bits, almacenado en uint16_t)
		 if (flag_adc == 0) {
				adc_array_base[index_base] = adc_value;  // Guardar valor en la posición actual
				voltage_b = adc_value;
				adc_ConfigSingleChannel(&v_collector); // Configurar el ADC para el siguiente canal
				flag_adc = 1; // Cambiar al otro array en la siguiente lectura
				index_base ++;

				if (index_base >= 100) {
					index_base = 0;  // Reiniciar índice si llega a 100

				}


		} else if(flag_adc == 1){
			adc_array_collector[index_collector] = adc_value;  // Guardar en array correspondiente
			voltage_c = adc_value;
			adc_ConfigSingleChannel(&v_base); // Configurar el ADC para el siguiente canal
			flag_adc = 0; // Cambiar de nuevo al otro array
			index_collector ++;
			if (index_collector >= 100) {
				index_collector = 0;  // Reiniciar índice si llega a 100
			}

		}

			fsm_program.state = STATE_IDLE;
			break;

	}
		default: {
		fsm_program.state = STATE_IDLE;
		break;
	}
	}
	return 	fsm_program.state;

}





/* en este callback se sube y baja la bandera (UIF) permitiendo que se de la interrupcion y no quede
 * interrumpido indefinidamente, al mismo tiempo siendo este el timer que usamos para nuestro contador de segundos
 * hacemos que durante esta interrupcion (de 1 seg, valga la redundancia) se nos sume a una variable auxiliar que llamamos
 * contador, que es la que nos ayuda en la parte del codigo del display, para ir mostrando los numeros en el momento que
 * necesitamos. Es decir cuando se hace la interrupcion, esta misma (administrada por el timer2) suma +1 al contador.
 * Esta parte del codigo ademas de mandar la interrupcion, tambien detiene la misma.
 */




void Timer2_Callback(void){
	fsm_program.state = STATE_REFRESH_DISPLAY;
	//adc_StartSingleConv();
}

/* este el callback del Led de estado, usamos el TooglePin para que se enciende y se apague, es la unico para loode			=	GPIO_MODE_
 * que nos sirve esta interrupcion. A grandes rasgos sirve para saber que el sistema funciona.

 Ademas de eso, agregamos la bandera para que se de la conversion de adc y la inicializacion de la variable sendMsg*/
void Timer5_Callback(void){
	gpio_TooglePin(&userLed);


}


//Callbacks del EXTI.
//subimos la bandera del clock, y leemos el dato que tenemos del encoder:exti_Data, y lo enviamos por pinData.
void callback_ExtInt0(void){
	fsm_program.state = STATE_GIRO_ENCODER;
	exti_Data = gpio_ReadPin(&pinData);
}

void callback_ExtInt4(void){
	fsm_program.state = STATE_MODO_RGB;
	conteo_ms = getTicks_ms();
}

void usart6_RxCallback(void){
	fsm_program.state = STATE_CHAR_RECEIVED;
	receivedChar = usart_getRxData(&commSerial);
}

void adc_CompleteCallback (void){
	fsm_program.state = STATE_READ_ADC;
	 //adc_data = adc_GetValue();

}

/*
 *  Esta funcion sirve para detectar problemas de parametros
 *  incorrectos al momento de ejecutar un programa.
   */
void assert_failed(uint8_t* file, uint32_t line){
	while(1){
		//problems...
	}
}
