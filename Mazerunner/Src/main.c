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
uint8_t flag_adc = 0;			//bandera para el usart
uint8_t contador = 0;			//variable para contar
uint8_t switcheo = 0; 			//variable para hacer switch de los transistores.
uint16_t valX = 0;				//valor adc del pinX
uint16_t valY = 0;				//valor adc del pinY
uint16_t adc_data = 0;			//data del adc
int8_t posX = 0;				//posicion en X pantalla
int8_t posY = 0;				//posicion en Y pantalla


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

//Handlers ADC
ADC_Config_t joystick = {0};  //		Channel0: PinA0





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
void drawPixel (I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y);
void animateRandomSquares(I2C_Handler_t *ptrHandlerI2Ctr);
void clearScreen(I2C_Handler_t *ptrHandlerI2Ctr);
void drawLineOnPage6(I2C_Handler_t *ptrHandlerI2Ctr);;

int main(void)
{
	init_system();								//Inicio de todas las configuraciones
	startOLED(&oled);
	clearDisplay(&oled);
	configMagic();								//Configuracion del Magic
	drawLineOnPage6(&oled);
	config_SysTick_ms(HSI_CLOCK_CONFIGURED); 	//Configurando el Systick

	void clearScreen(I2C_Handler_t *ptrHandlerI2Ctr);


	while(1){

		fsm_function(fsm_program.state);



		}

	}


void init_system(void){
	/*	Configuramos los pines*/

	/*	LedBlinky	*/
	userLed.pGPIOx 							= 	GPIOA;
	userLed.pinConfig.GPIO_PinNumber		=	PIN_5;
	userLed.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	/* Cargamos la configuracion de los registros que gobiernan el puerto */
	gpio_Config(&userLed);

	gpio_WritePin(&userLed, SET);

	/*	PinX	*/
	pinX.pGPIOx 						= 	GPIOC;
	pinX.pinConfig.GPIO_PinNumber		=	PIN_8;
	pinX.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	pinX.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	pinX.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	pinX.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	/* Cargamos la configuracion de los registros que gobiernan el puerto */
	gpio_Config(&pinX);

	gpio_WritePin(&pinX, SET);

	/*	PinY	*/
	pinY.pGPIOx 						= 	GPIOC;
	pinY.pinConfig.GPIO_PinNumber		=	PIN_5;
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

	pinScl.pGPIOx = GPIOB;
	pinScl.pinConfig.GPIO_PinNumber 			= PIN_10;
	pinScl.pinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	pinScl.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEED_MEDIUM;
	pinScl.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_OPENDRAIN;
	pinScl.pinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	pinScl.pinConfig.GPIO_PinAltFunMode 		= AF4;

	gpio_Config(&pinScl);

	pinSda.pGPIOx = GPIOB;
	pinSda.pinConfig.GPIO_PinNumber 			= PIN_3;
	pinSda.pinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	pinSda.pinConfig.GPIO_PinOutputSpeed 		= GPIO_OSPEED_MEDIUM;
	pinSda.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_OPENDRAIN;
	pinSda.pinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	pinSda.pinConfig.GPIO_PinAltFunMode 		= AF9;

	gpio_Config(&pinSda);

	oled.pI2Cx = I2C2;
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
		gpio_WritePin(&switcheoDecenas,0);
		display_numbers(unidades);
		gpio_WritePin(&switcheoUnidades,1);
		break;
		}
	case 1:{
		gpio_WritePin(&switcheoUnidades,0);
		display_numbers(decenas);
		gpio_WritePin(&switcheoDecenas,1);
		break;
		}
	}
}


//funcion para separar el numero, asi recibimos las unidades, decenas, centenas y millares del numero que nos arroje las lecturas del conteo, etc.
void separador_numero (uint16_t valor){

	uint16_t numero = 0;
	// supongamos que numero es igual 34
	numero = valor;
	// residuo 4,
	unidades = numero % 10;
	// cociente 3
	numero = numero/10;
	// residuo 3
	decenas = numero % 10;
	// cociente 12
	numero = numero/10;




}

void refresh (void){
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


		}
}

void reducir_tiempo(void){
	contador --;
		if(contador == 0){
			contador = 0;
		}
}

/* Funcion encargada de almacenar los valores de adc para el joystick, con esto switcheamos para que sea posible
 * la lectura de los valores en la coordenada X y Y, se usaran mosfets para un switcheo mas rapido.
 * Importante saber que los pines asignados empiezan uno en set y otro en reset.
 */

void lecturaXY(void){

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

	flag_adc = 0;


}

/* Funcion para hacer efectivo el movimiento del joystick en la pantalla, recordar que el adc tiene holgura
 * entonces los valores tienen que ser tenidos en cuenta en rangos.
 */

void procesar_coordenadas(void){

	if(valX > 3040 && valX < 4095){

		posX = posX +1;
	}


	if(valY > 3040 && valY < 4095){

			posY = posY +1;
		}

	if(valX > 0 && valX < 1040){

			posX = posX -1;
		}

	if(valY > 0 && valY < 1040){

				posY = posY -1;
			}
}

//void drawPixel (I2C_Handler_t *ptrHandlerI2Ctr, uint8_t x, uint8_t y){
//	uint8_t page = y/8;
//	uint8_t bit_position = y%8;
//	//uint8_t pixelData = 1 << bit_position;
//	char lineBytes[128] = { 0xFF };
//
//	setPage(ptrHandlerI2Ctr, page);
//	setColumnAddress(ptrHandlerI2Ctr, x);
//
//	sendDataBytes(ptrHandlerI2Ctr, lineBytes, 1);
//	sendDataBytes(ptrHandlerI2Ctr, lineBytes, 10);
//	sendDataBytes(ptrHandlerI2Ctr, lineBytes, 20);
//}

void clearScreen(I2C_Handler_t *ptrHandlerI2Ctr) {
	char emptyPage[128] = { 0 };  // Página vacía (todos los píxeles apagados)

	// Borrar todas las páginas de la pantalla
	for (uint8_t page = 0; page < 8; page++) {
		setPage(ptrHandlerI2Ctr, page);
		setColumnAddress(ptrHandlerI2Ctr, 0); // Comenzar desde la primera columna
		sendDataBytes(ptrHandlerI2Ctr, emptyPage, 128); // Enviar una página vacía
	}
}



void drawLineOnPage6(I2C_Handler_t *ptrHandlerI2Ctr) {
	char maze[8][128] = {0}; // 8 páginas (filas), 128 columnas

	    // Definir un laberinto estático (1 = pared, 0 = camino)
	    // Cada byte representa 8 píxeles en una columna
	    for (int col = 0; col < 128; col++) {
	        if (col % 16 == 0 || col % 16 == 15) {
	            for (int page = 0; page < 8; page++) {
	                maze[page][col] = 0xFF; // Paredes verticales
	            }
	        }
	    }

	    // Crear pasadizos
	    maze[1][0] = 0x00; // Entrada
	    maze[6][127] = 0x00; // Salida

	    // Dibujar el laberinto en la OLED
	    for (int page = 0; page < 8; page++) {
	        setPage(ptrHandlerI2Ctr, page);
	        setColumnAddress(ptrHandlerI2Ctr, 0);
	        sendDataBytes(ptrHandlerI2Ctr, maze[page], 128);
	    }
}





// apagar, configurar nuevamente, cambiar el canal. Pin de entrada






FSM_STATES fsm_function(uint8_t evento){

	switch(evento){
	case STATE_IDLE:{


		break;
	}
	case STATE_MENSAJE:{





		fsm_program.state = STATE_IDLE;
		break;
	}

	case STATE_JOYSTICK_MOVE:{



		fsm_program.state = STATE_IDLE;
		break;

	}

	case STATE_TIEMPO:{
		refresh();

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
	fsm_program.state = STATE_TIEMPO;
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
	//drawPixel (&oled,50, 50);

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
