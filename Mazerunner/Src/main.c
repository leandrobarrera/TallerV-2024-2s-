/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 * autor 		   : lbarreras
 * fecha 		   : 02/02/2025
 *
 *
 ******************************************************************************


 =========================================================     M A Z E R U N N E R     ========================================================================


 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "systick_driver_hal.h"
#include "main.h"
#include "usart_driver_hal.h"
#include "adc_driver_hal.h"
#include "i2c_driver_hal.h"
#include "oled_driver.h"
#define HSI_CLOCK_CONFIGURED				0				// 16 MHz
#define BUFFER_SIZE 						64
#define WIDTH 128
#define HEIGHT 64

//Handlers GPIO, para los pines. Lo de toda la vida.
GPIO_Handler_t userLed = {0}; 	//	PinH1
GPIO_Handler_t LedA = {0}; 		//	PinB12
GPIO_Handler_t LedB = {0}; 		//	PinA12
GPIO_Handler_t LedC = {0}; 		//	PinC11
GPIO_Handler_t LedD = {0}; 		//	PinC10
GPIO_Handler_t LedE = {0};		//	PinC12
GPIO_Handler_t LedF = {0};		//	PinA11
GPIO_Handler_t LedG = {0}; 		//	PinD2




//variables
uint16_t valor = 0;				//variable para el tratamiento del numero mostrado en el 7segment
uint16_t siete_segmentos = 0;	//variable para almacenar el valor del 7segmentos
uint8_t caso_transistor = 0;	//variable para hacer switcheo de los transistores, el de unidades por decena y viceversa
uint8_t unidades = 0;			//variable para almacenar el dato de la unidad
uint8_t decenas = 0;			//variable para almacenar el dato de la decena
uint8_t centenas = 0;			//variable para almacenar el dato de la decena
uint8_t millares = 0;			//variable para almacenar el dato de la decena
uint8_t flag_adc = 0;			//bandera para el usart
//uint16_t contador = 0;			//variable para contar
uint8_t switcheo = 0; 			//variable para hacer switch de los transistores.
uint16_t valX = 0;				//valor adc del pinX
uint16_t valY = 0;				//valor adc del pinY
uint16_t adc_data = 0;			//data del adc
int8_t posX = 3;				//posicion en X pantalla
int8_t posY = 3;				//posicion en Y pantalla
uint8_t flag_conteo = 0;
volatile int contador = 6000;
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



//GPIO Handlers

GPIO_Handler_t pinRX = {0}; //	PinC7
GPIO_Handler_t pinTX = {0}; //	PinC6
GPIO_Handler_t pinX = {0}; //	PinA0
GPIO_Handler_t pinY = {0}; //	PinA0
GPIO_Handler_t switcheoUnidades = {0};
GPIO_Handler_t switcheoDecenas = {0};
GPIO_Handler_t switcheoCentenas= {0};  // PinC8
GPIO_Handler_t switcheoMillares= {0};  // PinC5

//Handlers ADC
ADC_Config_t joystick = {0};  //		Channel0: PinA0


char coord[8][128] = {0};
uint8_t maze_matrix[64][128] = {0}; // Debes llenar esta matriz con los datos procesados de la imagen

//Handlers para los timers

Timer_Handler_t blinkyTimer = {0};
Timer_Handler_t display = {0};
Timer_Handler_t contador_segundos = {0};

//Handlers para OLED
GPIO_Handler_t pinSda = { 0 };
GPIO_Handler_t pinScl = { 0 };
I2C_Handler_t oled = { 0 };

// Definimos las posiciones fijas de los tres cuadrados
#define POS1 24   // Cuadrado izquierdo
#define POS2 56   // Cuadrado central
#define POS3 88   // Cuadrado derecho


//Maquina de estados finito :D
fsm_states_t fsm_program  = {0};



/*
 * The main function, where EVERYTHING HAPPENS. The magic happens...
 */



void init_system(void);							//funcion para iniciar las configuraciones
extern void configMagic(void);					//se importa la funcion magic desde el main.h
FSM_STATES fsm_function(uint8_t evento);		//maquina de estados finita
void lecturaXY(void);							//funcion para la lectura de los datos de adc en X y X
void procesar_coordenadas (void);				//funcion para el procesamiento de la lectura
void display_numbers(uint8_t valor);			//funcion para que se pinten los numeros, se enciendan los displays, se apaguen, etc.
void switcheo_transistor (uint8_t choose);		//funcion para encender los transistores y hacer el switcheo respectivo
void separador_numero (uint16_t valor);			//funcion para la creacion del numero como tal, ya que no usamos el mismo esquema de la tarea pasada, ahora usamos una funcion que genera los numeros que vamos a pintar en el display.
void refresh (void);							//funcion para el constante refresco de el display
void reducir_tiempo(void);						//funcion para contar hacia atras cada 1 seg
void animateRandomSquares(I2C_Handler_t *ptrHandlerI2Ctr);
void clearScreen(I2C_Handler_t *ptrHandlerI2Ctr);
void drawLineOnPage6(I2C_Handler_t *ptrHandlerI2Ctr);
void drawPixel2(I2C_Handler_t *ptrHandlerI2Ctr, uint8_t x, uint8_t y);
void clearPixel2(I2C_Handler_t *ptrHandlerI2Ctr, uint8_t x, uint8_t y);
uint8_t transformX(uint8_t x);
uint8_t ReadCoord(uint8_t x, uint8_t y);
void drawMaze(void);

int main(void)
{
	init_system();								//Inicio de todas las configuraciones
	startOLED(&oled);
	clearDisplay(&oled);
	configMagic();								//Configuracion del Magic
	config_SysTick_ms(HSI_CLOCK_CONFIGURED); 	//Configurando el Systick


	drawMaze();








	void clearScreen(I2C_Handler_t *ptrHandlerI2Ctr);


	while(1){

		fsm_function(fsm_program.state);



		}

	}


void init_system(void){
	/*	Configuramos los pines*/

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

	/*	PinX	*/
	pinX.pGPIOx 						= 	GPIOB;
	pinX.pinConfig.GPIO_PinNumber		=	PIN_5;
	pinX.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	pinX.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	pinX.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	pinX.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	/* Cargamos la configuracion de los registros que gobiernan el puerto */
	gpio_Config(&pinX);

	gpio_WritePin(&pinX, SET);

	/*	PinY	*/
	pinY.pGPIOx 						= 	GPIOC;
	pinY.pinConfig.GPIO_PinNumber		=	PIN_4;
	pinY.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	pinY.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	pinY.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	pinY.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	/* Cargamos la configuracion de los registros que gobiernan el puerto */
	gpio_Config(&pinY);

	gpio_WritePin(&pinY, RESET);


	/* Configuramos el ADC */

	//el joystick esta en el canal0, usamos las configs que el profe nos dejo

	joystick.channel 				= CHANNEL_0;
	joystick.resolution 			= RESOLUTION_12_BIT;
	joystick.dataAlignment 			= ALIGNMENT_RIGHT;
	joystick.interruptState 		= ADC_INT_ENABLE;
	joystick.samplingPeriod 		= SAMPLING_PERIOD_112_CYCLES;

	//cargamos la config.
	adc_ConfigSingleChannel(&joystick);
	adc_StartSingleConv();


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







	/* La primera fila para definir el que queremos usar (quedan a eleccion del programador) no obstante
	 * en la tarea se nos pide que usemos ciertos timers con cierto numero especifico de bits, 32 para unos y 16 para otros.
	 * Por tanto se uso esta escogencia. En la segunda fila del prescaler se usa el 16000 para que durante todo este
	 * proceso de interrupciones, y teniendo que el micro va a 16MHz, el 16000 divide esta cifra y la deja en un formato de
	 * 1 ms. Este 1 ms se multiplica por el periodo, y lo elegimos segun lo que requiramos. Para el TIM2 pusimos 1000
	 * que 1ms * 1000, nos da 1000ms lo que es igual a 1 segundo, lo que completa nuestro deseo que este contador de segundos
	 * en efecto, cuente segundos. Para el blinky de manera "estandar" hacemos que vaya a un ratio de 250 ms.
	 */


	contador_segundos.pTIMx 								= TIM2;
	contador_segundos.TIMx_Config.TIMx_Prescaler			= 16000;  			//	Genera incrementos de 1ms
	contador_segundos.TIMx_Config.TIMx_Period				= 1000;  			//	se pone 1000 porque en combinacion con el prescaler que es 16000, esa division da para que el timer vaya a 1000ms
	contador_segundos.TIMx_Config.TIMx_mode					= TIMER_UP_COUNTER;
	contador_segundos.TIMx_Config.TIMx_InterruptEnable		= TIMER_INT_ENABLE;


	/*	Configuramos el Timer */
	timer_Config(&contador_segundos);

	//	Encedemos el Timer.
	timer_SetState(&contador_segundos,SET);


	blinkyTimer.pTIMx 										= TIM5;
	blinkyTimer.TIMx_Config.TIMx_Prescaler					= 16000;  			//Genera incrementos de 1ms
	blinkyTimer.TIMx_Config.TIMx_Period						= 250;  				//el prescaler lo ajusta 1ms, entonces lo quiero a 250ms, y es la multiplicacion de uno con el otro.
	blinkyTimer.TIMx_Config.TIMx_mode						= TIMER_UP_COUNTER;
	blinkyTimer.TIMx_Config.TIMx_InterruptEnable			= TIMER_INT_ENABLE;


	/*	Configuramos el Timer */
	timer_Config(&blinkyTimer);

	/*Cuenta regresiva */
	timer_Config(&blinkyTimer);

	//	Encedemos el Timer.
	timer_SetState(&blinkyTimer,SET);

	display.pTIMx 											= TIM3;
	display.TIMx_Config.TIMx_Prescaler						= 16000;  			//Genera incrementos de 1ms
	display.TIMx_Config.TIMx_Period							= 2;  				//60FPS ultra calidad gamer. Se tuvo que subir porque no se veía fluido el refresco, antes era 15, que significaban 60 FPS
	display.TIMx_Config.TIMx_mode							= TIMER_UP_COUNTER;
	display.TIMx_Config.TIMx_InterruptEnable				= TIMER_INT_ENABLE;


	/*	Configuramos el Timer */
	timer_Config(&display);

	//	Encedemos el Timer.
	timer_SetState(&display,SET);


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

	pinScl.pGPIOx = GPIOA;
	pinScl.pinConfig.GPIO_PinNumber 			= PIN_8;
	pinScl.pinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	pinScl.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEED_MEDIUM;
	pinScl.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_OPENDRAIN;
	pinScl.pinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	pinScl.pinConfig.GPIO_PinAltFunMode 		= AF4;

	gpio_Config(&pinScl);

	pinSda.pGPIOx = GPIOB;
	pinSda.pinConfig.GPIO_PinNumber 			= PIN_8;
	pinSda.pinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	pinSda.pinConfig.GPIO_PinOutputSpeed 		= GPIO_OSPEED_MEDIUM;
	pinSda.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_OPENDRAIN;
	pinSda.pinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	pinSda.pinConfig.GPIO_PinAltFunMode 		= AF9;

	gpio_Config(&pinSda);

	oled.pI2Cx = I2C3;
	oled.i2c_mode = I2C_MODE_SM_SPEED;
	oled.i2c_mainClock = I2C_MAIN_CLOCK_16_Mhz;
	oled.slaveAddress = 0x3C;

	i2c_Config(&oled);

	config_SysTick_ms(0);




}



/*
 *  Overwrite function
 **/

/* Se pinta un led y luego otro, switcheando segun el numero requerido, 0 los enciende, 1 los apaga, esto debido a los transistores usados*/

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




//lo que hacemos primero es apagar los pines que switchean todo lo que no sea unidades (para el primer caso por ejemplo), y si, 1 es apagar 0 encender. Usamos display_number para encender lo que requerimos y asi se completa unidades, lo mismo para decenas.
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


void refresh (void){
	siete_segmentos = contador;
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

void reducir_tiempo(void){
	if(flag_conteo == 1){
	contador -= 100;
		if(contador == 0){
			contador = 6000;
		}
		flag_conteo = 0;
	}

}

/* Funcion encargada de almacenar los valores de adc para el joystick, con esto switcheamos para que sea posible
 * la lectura de los valores en la coordenada X y Y, se usaran mosfets para un switcheo mas rapido.
 * Importante saber que los pines asignados empiezan uno en set y otro en reset.
 */

void lecturaXY(void){

	clearPixel2(&oled, posX, posY);


	adc_StartSingleConv();
	systick_Delay_ms(10);
	valX = adc_GetValue();
	systick_Delay_ms(5);


	gpio_WritePin(&pinX, 0);
	gpio_WritePin(&pinY, 1);
	systick_Delay_ms(5);


	adc_StartSingleConv();
	systick_Delay_ms(10);
	valY = adc_GetValue();
	systick_Delay_ms(5);


	gpio_WritePin(&pinX, 1);
	gpio_WritePin(&pinY, 0);
	systick_Delay_ms(5);

	procesar_coordenadas();

	drawPixel2(&oled, posX, posY);


	flag_adc = 0;


}

/* Funcion para hacer efectivo el movimiento del joystick en la pantalla, recordar que el adc tiene holgura
 * entonces los valores tienen que ser tenidos en cuenta en rangos.
 */

void procesar_coordenadas(void){

	if(valX > 3500 && valX < 3900){
		if(posX <= 63){
			if(ReadCoord(posX+1, posY) == 0){
				posX = posX +1;
			}
		}
	}


	if(valY > 3500 && valY < 3900){
		if(posY >= 1){
			if(ReadCoord(posX, posY-1) == 0){
				posY = posY -1;
			}
		}

	}

	if(valX > 0 && valX < 2500){
		if(posX >= 1){
			if(ReadCoord(posX-2, posY) == 0){
				posX = posX -1;
			}
		}

	}

	if(valY > 0 && valY < 2500){
		if(posY <= 127){
			if(ReadCoord(posX, posY+1) == 0){
				posY = posY +1;
			}
		}

	}
}



void drawPixel2(I2C_Handler_t *ptrHandlerI2Ctr, uint8_t x, uint8_t y) {

	if(x >= 1 && x <= 8){
		uint8_t auxpage1 = coord[0][y] |= (1 << transformX(x));
		coord[0][y] = auxpage1;
	}
	if(x > 8 && x <= 16){
		uint8_t auxpage1 = coord[1][y] |= (1 << transformX(x));
		coord[1][y] = auxpage1;
	}
	if(x > 16 && x <= 24){
		uint8_t auxpage1 = coord[2][y] |= (1 << transformX(x));
		coord[2][y] = auxpage1;
	}
	if(x > 24 && x <= 32){
		uint8_t auxpage1 = coord[3][y] |= (1 << transformX(x));
		coord[3][y] = auxpage1;
	}
	if(x > 32 && x <= 40){
		uint8_t auxpage1 = coord[4][y] |= (1 << transformX(x));
		coord[4][y] = auxpage1;
	}
	if(x > 40 && x <= 48){
		uint8_t auxpage1 = coord[5][y] |= (1 << transformX(x));
		coord[5][y] = auxpage1;
	}
	if(x > 48 && x <= 56){
		uint8_t auxpage1 = coord[6][y] |= (1 << transformX(x));
		coord[6][y] = auxpage1;
	}
	if(x > 56 && x <= 64){
		uint8_t auxpage1 = coord[7][y] |= (1 << transformX(x));
		coord[7][y] = auxpage1;
	}

	    // coordenadas
	    for (int page = 0; page < 8; page++) {
	        setPage(ptrHandlerI2Ctr, page);
	        setColumnAddress(ptrHandlerI2Ctr, 0);
	        sendDataBytes(ptrHandlerI2Ctr, coord[page], 128);
	    }
}

void clearPixel2(I2C_Handler_t *ptrHandlerI2Ctr, uint8_t x, uint8_t y) {
    if(x >= 1 && x <= 8){
        uint8_t auxpage1 = coord[0][y] &= ~(1 << transformX(x));
        coord[0][y] = auxpage1;
    }
    if(x > 8 && x <= 16){
        uint8_t auxpage1 = coord[1][y] &= ~(1 << transformX(x));
        coord[1][y] = auxpage1;
    }
    if(x > 16 && x <= 24){
        uint8_t auxpage1 = coord[2][y] &= ~(1 << transformX(x));
        coord[2][y] = auxpage1;
    }
    if(x > 24 && x <= 32){
        uint8_t auxpage1 = coord[3][y] &= ~(1 << transformX(x));
        coord[3][y] = auxpage1;
    }
    if(x > 32 && x <= 40){
        uint8_t auxpage1 = coord[4][y] &= ~(1 << transformX(x));
        coord[4][y] = auxpage1;
    }
    if(x > 40 && x <= 48){
        uint8_t auxpage1 = coord[5][y] &= ~(1 << transformX(x));
        coord[5][y] = auxpage1;
    }
    if(x > 48 && x <= 56){
        uint8_t auxpage1 = coord[6][y] &= ~(1 << transformX(x));
        coord[6][y] = auxpage1;
    }
    if(x > 56 && x <= 64){
        uint8_t auxpage1 = coord[7][y] &= ~(1 << transformX(x));
        coord[7][y] = auxpage1;
    }

    // coordenadas
    for (int page = 0; page < 8; page++) {
        setPage(ptrHandlerI2Ctr, page);
        setColumnAddress(ptrHandlerI2Ctr, 0);
        sendDataBytes(ptrHandlerI2Ctr, coord[page], 128);
    }
}

uint8_t ReadCoord(uint8_t x, uint8_t y){

	uint8_t page = x/8;

	uint8_t bit_position = x%8;

	uint8_t byte = coord[page][y];
	uint8_t bit_value = (byte >> bit_position) & 1;

	return bit_value;

}

uint8_t transformX(uint8_t x){

	if(x%8 == 1%8){
		return 0;
	}
	if(x%8 == 2%8){
		return 1;
	}
	if(x%8 == 3%8){
		return 2;
	}
	if(x%8 == 4%8){
		return 3;
	}
	if(x%8 == 5%8){
		return 4;
	}
	if(x%8 == 6%8){
		return 5;
	}
	if(x%8 == 7%8){
		return 6;
	}
	if(x%8 == 8%8){
		return 7;
	}
	return 0;
}



void clearScreen(I2C_Handler_t *ptrHandlerI2Ctr) {
	char emptyPage[128] = { 0 };  // Página vacía (todos los píxeles apagados)

	// Borrar todas las páginas de la pantalla
	for (uint8_t page = 0; page < 8; page++) {
		setPage(ptrHandlerI2Ctr, page);
		setColumnAddress(ptrHandlerI2Ctr, 0); // Comenzar desde la primera columna
		sendDataBytes(ptrHandlerI2Ctr, emptyPage, 128); // Enviar una página vacía
	}
}






void drawMaze(void){

	//Bordes
	for(int i = 10 ; i<128 ; i++){
		drawPixel2(&oled,1,i);
		systick_Delay_ms(2);
	}
	for(int i = 1 ; i<64 ; i++){
		drawPixel2(&oled,i,127);
		systick_Delay_ms(2);
	}
	for(int i = 118 ; i>0 ; i--){
		drawPixel2(&oled,64,i);
		systick_Delay_ms(2);
	}
	for(int i = 64 ; i>0 ; i--){
		drawPixel2(&oled,i,0);
		systick_Delay_ms(2);
	}

	//Horizontales
	//X10
	for(int i = 10 ; i<30 ; i++){
		drawPixel2(&oled,10,i);
		systick_Delay_ms(2);
	}
	for(int i = 70 ; i<80 ; i++){
		drawPixel2(&oled,10,i);
		systick_Delay_ms(2);
	}
	for(int i = 100 ; i<120 ; i++){
		drawPixel2(&oled,10,i);
		systick_Delay_ms(2);
	}
	//X20
	for(int i = 30 ; i<40 ; i++){
		drawPixel2(&oled,20,i);
		systick_Delay_ms(2);
	}
	for(int i = 50 ; i<70 ; i++){
		drawPixel2(&oled,20,i);
		systick_Delay_ms(2);
	}
	for(int i = 80 ; i<90 ; i++){
		drawPixel2(&oled,20,i);
		systick_Delay_ms(2);
	}

	//X30
	for(int i = 0 ; i<20 ; i++){
		drawPixel2(&oled,30,i);
		systick_Delay_ms(2);
	}
	for(int i = 30 ; i<40 ; i++){
		drawPixel2(&oled,30,i);
		systick_Delay_ms(2);
	}
	for(int i = 80 ; i<90 ; i++){
		drawPixel2(&oled,30,i);
		systick_Delay_ms(2);
	}

	//X40
	for(int i = 10 ; i<20 ; i++){
		drawPixel2(&oled,40,i);
		systick_Delay_ms(2);
	}
	for(int i = 50 ; i<60 ; i++){
		drawPixel2(&oled,40,i);
		systick_Delay_ms(2);
	}
	for(int i = 100 ; i<110 ; i++){
		drawPixel2(&oled,40,i);
		systick_Delay_ms(2);
	}
	for(int i = 120 ; i<127 ; i++){
		drawPixel2(&oled,40,i);
		systick_Delay_ms(2);
	}

	//X52
	for(int i = 0 ; i<20 ; i++){
		drawPixel2(&oled,52,i);
		systick_Delay_ms(2);
	}
	for(int i = 30 ; i<40 ; i++){
		drawPixel2(&oled,52,i);
		systick_Delay_ms(2);
	}
	for(int i = 60 ; i<70 ; i++){
		drawPixel2(&oled,52,i);
		systick_Delay_ms(2);
	}
	for(int i = 110 ; i<120 ; i++){
		drawPixel2(&oled,52,i);
		systick_Delay_ms(2);
	}

	//Verticales
	//Y10
	for(int i = 10 ; i<20 ; i++){
		drawPixel2(&oled,i,10);
		systick_Delay_ms(2);
	}

	//Y20
	for(int i = 10 ; i<40 ; i++){
		drawPixel2(&oled,i,20);
		systick_Delay_ms(2);
	}

	//Y30
	for(int i = 40 ; i<52 ; i++){
		drawPixel2(&oled,i,30);
		systick_Delay_ms(2);
	}

	//Y40
	for(int i = 0 ; i<20 ; i++){
		drawPixel2(&oled,i,40);
		systick_Delay_ms(2);
	}
	for(int i = 30 ; i<40 ; i++){
		drawPixel2(&oled,i,40);
		systick_Delay_ms(2);
	}

	//Y50
	for(int i = 0 ; i<10 ; i++){
		drawPixel2(&oled,i,50);
		systick_Delay_ms(2);
	}
	for(int i = 30 ; i<64 ; i++){
		drawPixel2(&oled,i,50);
		systick_Delay_ms(2);
	}

	//Y60
	for(int i = 0 ; i<30 ; i++){
		drawPixel2(&oled,i,60);
		systick_Delay_ms(2);
	}

	//Y70
	for(int i = 20 ; i<52 ; i++){
		drawPixel2(&oled,i,70);
		systick_Delay_ms(2);
	}

	//Y80
	for(int i = 40 ; i<64 ; i++){
		drawPixel2(&oled,i,80);
		systick_Delay_ms(2);
	}

	//Y90
	for(int i = 0 ; i<20 ; i++){
		drawPixel2(&oled,i,90);
		systick_Delay_ms(2);
	}
	for(int i = 30 ; i<64 ; i++){
		drawPixel2(&oled,i,90);
		systick_Delay_ms(2);
	}

	//Y100
	for(int i = 10 ; i<30 ; i++){
		drawPixel2(&oled,i,100);
		systick_Delay_ms(2);
	}
	for(int i = 40 ; i<64 ; i++){
		drawPixel2(&oled,i,100);
		systick_Delay_ms(2);
	}

	//Y110
	for(int i = 10 ; i<40 ; i++){
		drawPixel2(&oled,i,110);
		systick_Delay_ms(2);
	}

	//Y120
	for(int i = 20 ; i<52 ; i++){
		drawPixel2(&oled,i,120);
		systick_Delay_ms(2);
	}



}


// apagar, configurar nuevamente, cambiar el canal. Pin de entrada






FSM_STATES fsm_function(uint8_t evento){

	switch(evento){
	case STATE_IDLE:{


		break;
	}
	case STATE_MENSAJE:{

		printf("\n");
		printf("ms: %d\n", contador);


		fsm_program.state = STATE_IDLE;
		break;
	}

	case STATE_JOYSTICK_MOVE:{



		fsm_program.state = STATE_IDLE;
		break;

	}

	case STATE_TIEMPO:{
		refresh();
		reducir_tiempo();
		if(flag_adc == 1){
			lecturaXY();
		}

		fsm_program.state = STATE_IDLE;
		break;
	}

	case STATE_PANTALLA:{


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





//sube la bandera del display
void Timer2_Callback(void){

	flag_conteo = 1;
}

void Timer3_Callback(void){

	fsm_program.state = STATE_TIEMPO;
}

/* este el callback del Led de estado, usamos el TooglePin para que se enciende y se apague, es la unico para lo
 * que nos sirve esta interrupcion. A grandes rasgos sirve para saber que el sistema funciona.

 Ademas de eso, agregamos la bandera para que se de la conversion de adc y la inicializacion de la variable sendMsg*/
void Timer5_Callback(void){
	gpio_TooglePin(&userLed);
	flag_adc = 1;

}


// Callback de ADC. adcData recibe el valor del adc. Subimos la bander del adc.
void adc_CompleteCallback (void){
	fsm_program.state = STATE_JOYSTICK_MOVE;


}

void usart6_RxCallback(void){
	fsm_program.state = STATE_MENSAJE;
	receivedChar = usart_getRxData(&commSerial);
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
