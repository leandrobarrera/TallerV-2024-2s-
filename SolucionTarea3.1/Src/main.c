/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 * autor 			: Leandro Barrera Salas
 * fecha 			: 10/09/2024
 ******************************************************************************
 */
#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "adc_driver_hal.h"
#include "usart_driver_hal.h"
#include "exti_driver_hal.h"



//Handlers GPIO, para los pines. Lo de toda la vida.
GPIO_Handler_t userLed = {0}; 	//	PinA5
GPIO_Handler_t LedA = {0}; 		//	PinC8
GPIO_Handler_t LedB = {0}; 		//	PinB8
GPIO_Handler_t LedC = {0}; 		//	PinD2
GPIO_Handler_t LedD = {0}; 		//	PinC10
GPIO_Handler_t LedE = {0};		//	PinC11
GPIO_Handler_t LedF = {0};		//	PinC9
GPIO_Handler_t LedG = {0}; 		//	PinC12



//variables nuevas
uint8_t valor = 0;
uint8_t caso_transistor = 0;  //variable para hacer el switcheo de los transistores y encender el 7segmentos.
uint8_t modo = 0;		//esta variable hace que cuando presionamos el boton y se manda la interrupcion en el micro, sume uno asi misma, cambiando de modo.
uint8_t flag_usart = 0;	//bander para el usart
uint8_t flag_adc = 0;	//nandera para el adc
uint16_t adcData = 0;	//dato que obtenemos del adc y posteriormente sera guardado
uint16_t datoTrimmer = 0;	//variable donde guardamos el adcData del canal0
uint16_t datoFotoresistencia = 0;	//variable donde guardamos el adcData del canal1
uint8_t flag_adc_conversion = 0;
uint8_t sendMsg = 0; //bandera de USART
uint16_t contador = 0; //contador para el modo 4
uint8_t changeModo = 0; //flag del EXTI 5 para cambiar los modos
uint8_t flag_conteo = 0; //flag del contador para dejar de contar mientras estamos en los otros modos.
uint8_t exti_Data = 0; // guarda el valor del data del EXTi cuando se lanza la interrupcion de Clock del Encoder.
uint8_t flag_clock = 0; //bandera para el flag del clock.
uint16_t exti_conteo = 0; //contador del Encoder.
uint8_t usart_data = 0; //el dato del usart2
uint16_t promedio = 0;	//esta variable y la de abajo es una recomendacion, un arreglo para promediar datos, sirve para que no se actualicen los datos de una manera tosca y rapida haciendo dificil la lectura de los mismos, funciona pero podria ser mejor.
uint8_t subdivisiones = 0;

//variables para creacion de numero
uint16_t siete_segmentos = 0;		// # que se imprime en el 7segmentos.
uint8_t unidades = 0;
uint8_t decenas = 0;
uint8_t centenas = 0;
uint8_t millares = 0;
uint8_t pinClockc = 0;



//Handlers de la tarea 3


//GPIO actualizados
GPIO_Handler_t switcheoUnidades= {0};  // PinB7
GPIO_Handler_t switcheoDecenas= {0};  // PinC13
GPIO_Handler_t switcheoCentenas= {0};  // PinA9
GPIO_Handler_t switcheoMillares= {0};  // PinC7


//Handlers GPIO
GPIO_Handler_t LedRed = {0}; //	Pines RGB	PinC5
GPIO_Handler_t LedGreen = {0}; //	Pines RGB	PinB9
GPIO_Handler_t LedBlue = {0}; //	Pines RGB	PinC6
GPIO_Handler_t pinSW = {0}; //	PinPB5
GPIO_Handler_t pinData = {0}; //	PinB3
GPIO_Handler_t pinClock = {0}; //	PinB13
GPIO_Handler_t pinRX = {0}; //	PinA0
GPIO_Handler_t pinTX = {0}; //	PinA0

//Handlers ADC---> tienen GPIO configurado por dentro
ADC_Config_t trimmer = {0};  //		Channel0: PinA0
ADC_Config_t fotoresistencia = {0};		//		Channel1: PinA1

//Handlers EXTI
EXTI_Config_t extiSW = {0};
EXTI_Config_t extiClock = {0};


// buffers para USART
char bufferMsg[128] = {0};



//Handlers para los tres timers que se pidieron
Timer_Handler_t contador_segundos = {0};
Timer_Handler_t blinkyTimer = {0};
Timer_Handler_t display = {0};

//Handlers para el usart
USART_Handler_t usart2 = {0};





//Banderas. Se crearon banderas para la corecta ejecucion y entendimiento de las interrupciones. (estas son de la tarea anterior)
uint8_t display_flag = 0;
uint8_t display_select = 0;


/*
 * The main function, where EVERYTHING HAPPENS. The magic happens...
 */


//usamos un enum para pasar entre modos, escogimos los mismos nombres de la tarea
enum{
	modo_0 = 0,
	modo_1,
	modo_2,
	modo_3,
	modo_4,
	modo_5
};


// se creo una funcion para inicializar el sistema, esto ayuda a desplazarnos mas rapido en el codigo.
void init_system(void);
void display_numbers(uint8_t valor);	//funcion para que se pinten los numeros, se enciendan los displays, se apaguen, etc.
void switcheo_transistor (uint8_t choose);		//funcion para encender los transistores y hacer el switcheo respectivo
void separador_numero (uint16_t valor);			//funcion para la creacion del numero como tal, ya que no usamos el mismo esquema de la tarea pasada, ahora usamos una funcion que genera los numeros que vamos a pintar en el display.

int main(void)
{
	init_system();



	while(1){


		pinClockc = gpio_ReadPin(&pinClock);

		//se usa la bandera para que cuando presionemos m se muestre el mensaje escrito.

		if(flag_usart){
			if (usart_data == 'm'){
				usart_writeMsg(&usart2, "Taller V + Bancolombia rocks!\n");
			}

			flag_usart = 0;
		}

		//changeModo es la variable que usamos para navegar a traves de los modos, y modos es el contador dentro de la misma. Establecemos un maximo de 6 modos para que el contador no siga sumando hasta modo infinito.
		if(changeModo){
			modo++;
			if(modo==6){
				modo = 0;
			}
			changeModo = 0;
		}
		// switch case para navegar entre modoos
		switch(modo){
			//en el modo 0, ejecutamos las indicaciones escritas en la tarea, todas comparten la config. del led RGB, otros comparten configs. de USART, y ADC.
			case modo_0:{
				gpio_WritePin(&LedRed, 0);
				gpio_WritePin(&LedGreen, 0);
				gpio_WritePin(&LedBlue, 0);

				// la variable siete_segmentos es lo que al final se almacena y muestra en el display.
				separador_numero(siete_segmentos);
				// usamos la bandera del display para switchear correctamente los transistores. caso_transistor es una variable que nos permite avanzar al siguiente switcheo del siguiente transistor.
				if (display_flag){
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

					display_flag = 0;
				}
				break;
			}
			case modo_1:{
					gpio_WritePin(&LedRed, 0);
					gpio_WritePin(&LedGreen, 1);
					gpio_WritePin(&LedBlue, 0);
					// mandamos la funcion configsingchannel a ejecutarse, recordemos que es la que inicializa el proceso del adc, y se llama asi porque solo podemos tener un canal ADC al tiempo funcionando.
					adc_ConfigSingleChannel(&trimmer);
					// buen uso del & como operacion, y no siempre como operacion bitwise. Usamos las banderas de la conversion y del propio adc.
					if(flag_adc & flag_adc_conversion){
					//para este modo usamos el trimmer, y el dato de adc se guarda en datoTrimmer
						datoTrimmer = adcData;
						adc_StartSingleConv();
						//bajamos la bandera
						flag_adc = 0;
					}
					//arreglo para mejor visualizacion de los datos mostrados en el micro.
					if(subdivisiones < 6){
						promedio += datoTrimmer;
						subdivisiones ++;
					}else{
						siete_segmentos = promedio / 5;
						subdivisiones = 0;
						promedio = 0;
					}
					separador_numero(siete_segmentos);
					//switcheo de los transistores. Este proceso es generico de todos los casos, es para encender el display y mostrar lo que necesitemos.
					if (display_flag){
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


						display_flag = 0;
					}

					// sendMsg es la variable del usart.
				if (sendMsg){
					//esta es la forma de escribir para que mediante el canal de usart podamos obtener el mensae "modo 1" se tome el espacio y diga el valor que entrega adc (a traves del trimmer).
					sprintf (bufferMsg, "modo 1 \n%d\n",datoTrimmer);
					usart_writeMsg (&usart2, bufferMsg);
					sendMsg = 0;
				}
				break;
			}

			//configs iniciales de siempre para encender el 7segmentos y LED RGB.
			case modo_2:{
				gpio_WritePin(&LedRed, 0);
				gpio_WritePin(&LedGreen, 0);
				gpio_WritePin(&LedBlue, 1);

				separador_numero(siete_segmentos);

				if (display_flag){
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

					display_flag = 0;
				}
				break;
			}
			//para el caso 3 es muy similar al del trimmer solo que usamos los datos que corresponde a fotoresistencia.
			case modo_3:{
				gpio_WritePin(&LedRed, 1);
				gpio_WritePin(&LedGreen, 0);
				gpio_WritePin(&LedBlue, 0);
				adc_ConfigSingleChannel(&fotoresistencia);

				if(flag_adc & flag_adc_conversion){
					datoFotoresistencia = adcData;
					adc_StartSingleConv();
					flag_adc = 0;
				}

				if(subdivisiones < 6){
					promedio += datoFotoresistencia;
					subdivisiones ++;
				}else{
					siete_segmentos = promedio / 5;
					subdivisiones = 0;
					promedio = 0;
				}

				separador_numero(siete_segmentos);

				if (display_flag){
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


					display_flag = 0;
				}

			if (sendMsg){
				sprintf (bufferMsg, "modo 3 \n%d\n",datoFotoresistencia);
				usart_writeMsg (&usart2, bufferMsg);
				sendMsg = 0;
			}
			break;

			}
			//al principio creamos la funcion de contador para que cuando llegue a 4096 se reinicie.
			case modo_4:{
				if(flag_conteo){
					contador ++;
					if(contador == 4096){
						contador = 0;
					}
					//bajamos la bandera para que no siga contando mientras no estamos en este modo.
					flag_conteo = 0;
				}
				gpio_WritePin(&LedRed, 1);
				gpio_WritePin(&LedGreen, 1);
				gpio_WritePin(&LedBlue, 0);
				//recordar, siete segmentos muestra la variable contador.
				siete_segmentos = contador;
				separador_numero(siete_segmentos);

				if (display_flag){
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


					display_flag = 0;
				}
				//mensaje a traves del canal de USART, nada nuevo que no se haya hecho en los modos anteriores.
			if (sendMsg){
				sprintf (bufferMsg, "modo 4\n%d\n",contador);
				usart_writeMsg (&usart2, bufferMsg);
				sendMsg = 0;
			}

				break;
			}
			case modo_5:{



				gpio_WritePin(&LedRed, 1);
				gpio_WritePin(&LedGreen, 1);
				gpio_WritePin(&LedBlue, 1);
				//usamos la bandera para el exti-Encoder. Usamos if para que en el sentido de las manecillas del reloj sume y viceversa en el contador exti.
				if(flag_clock){
					if(exti_Data){
						if(exti_conteo == 0){
							exti_conteo = 4096;
						}
						exti_conteo--;
					}
					//se hicieorn correctamente los reinicios tanto si pasamos de 4095 a como si nos devolvemos desde 0.
					else{
						exti_conteo ++;
						if(exti_conteo == 4096){
							exti_conteo = 0;
							}
					}
					//bajamos bandera para que deje de contar.
					flag_clock = 0;
				}



				siete_segmentos = exti_conteo;
				separador_numero(siete_segmentos);

				if (display_flag){
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


					display_flag = 0;
				}

				if (sendMsg){
					sprintf (bufferMsg, "modo 5\n%d\n",exti_conteo);
					usart_writeMsg (&usart2, bufferMsg);
					sendMsg = 0;
				}
				break;
			}
		}

	}
}

void init_system(void){
	/*	Configuramos el pin*/
	userLed.pGPIOx 							= 	GPIOA;
	userLed.pinConfig.GPIO_PinNumber		=	PIN_5;
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
	LedA.pGPIOx 						= 	GPIOC;
	LedA.pinConfig.GPIO_PinNumber		=	PIN_8;
	LedA.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedA.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedA.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedA.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedA);


	/*	LedB	*/
	LedB.pGPIOx 						= 	GPIOB;
	LedB.pinConfig.GPIO_PinNumber		=	PIN_8;
	LedB.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedB.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedB.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedB.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedB);

	/*	LedC*/
	LedC.pGPIOx 						= 	GPIOD;
	LedC.pinConfig.GPIO_PinNumber		=	PIN_2;
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
	LedE.pinConfig.GPIO_PinNumber		=	PIN_11;
	LedE.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedE.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedE.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedE.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedE);

	/*	LedF*/
	LedF.pGPIOx 						= 	GPIOC;
	LedF.pinConfig.GPIO_PinNumber		=	PIN_9;
	LedF.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedF.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedF.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	LedF.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedF);

	/*	LedG*/
	LedG.pGPIOx 						= 	GPIOC;
	LedG.pinConfig.GPIO_PinNumber		=	PIN_12;
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
	switcheoDecenas.pinConfig.GPIO_PinNumber		=	PIN_13;
	switcheoDecenas.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	switcheoDecenas.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	switcheoDecenas.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	switcheoDecenas.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&switcheoDecenas);

	/*	switcheoCentenas */
	switcheoCentenas.pGPIOx 						= 	GPIOA;
	switcheoCentenas.pinConfig.GPIO_PinNumber		=	PIN_9;
	switcheoCentenas.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	switcheoCentenas.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	switcheoCentenas.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	switcheoCentenas.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&switcheoCentenas);

	/*	switcheoMillares */
	switcheoMillares.pGPIOx 						= 	GPIOC;
	switcheoMillares.pinConfig.GPIO_PinNumber		=	PIN_7;
	switcheoMillares.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	switcheoMillares.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	switcheoMillares.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	switcheoMillares.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&switcheoMillares);


	/*	LedRed*/
	LedRed.pGPIOx 							= 	GPIOC;
	LedRed.pinConfig.GPIO_PinNumber			=	PIN_5;
	LedRed.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedRed.pinConfig.GPIO_PinOutputType		=	GPIO_OTYPE_PUSHPULL;
	LedRed.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	LedRed.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedRed);


	/*	LedBlue*/
	LedBlue.pGPIOx 							= 	GPIOC;
	LedBlue.pinConfig.GPIO_PinNumber		=	PIN_6;
	LedBlue.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedBlue.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedBlue.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	LedBlue.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedBlue);

	/*	LedGreen*/
	LedGreen.pGPIOx 						= 	GPIOB;
	LedGreen.pinConfig.GPIO_PinNumber		=	PIN_9;
	LedGreen.pinConfig.GPIO_PinMode			=	GPIO_MODE_OUT;
	LedGreen.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	LedGreen.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_FAST;
	LedGreen.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

	gpio_Config(&LedGreen);

	//GPIOS del exti, recordar que se usan en modo input.

	/*	GPIO del pinSW */
	pinSW.pGPIOx 							= 	GPIOB;
	pinSW.pinConfig.GPIO_PinNumber			=	PIN_5;
	pinSW.pinConfig.GPIO_PinMode			=	GPIO_MODE_IN;

	gpio_Config(&pinSW);

	extiSW.pGPIOHandler		=		&pinSW;
	extiSW.edgeType			=		EXTERNAL_INTERRUPT_RISING_EDGE;

	exti_Config(&extiSW);

	/*	GPIO del pinData. No necesita todas las configs de pinClock o pinSW. */
	pinData.pGPIOx 							= 	GPIOB;
	pinData.pinConfig.GPIO_PinNumber		=	PIN_3;
	pinData.pinConfig.GPIO_PinMode			=	GPIO_MODE_IN;

	gpio_Config(&pinData);

	/*	GPIO del pinClock */
	pinClock.pGPIOx 						= 	GPIOB;
	pinClock.pinConfig.GPIO_PinNumber		=	PIN_13;
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

	contador_segundos.pTIMx 							= TIM2;
	contador_segundos.TIMx_Config.TIMx_Prescaler		=16000;  //	Genera incrementos de 1ms
	contador_segundos.TIMx_Config.TIMx_Period			=1000;  //	se pone 1000 porque en combinacion con el prescaler que es 16000, esa division da para que el timer vaya a 1000ms
	contador_segundos.TIMx_Config.TIMx_mode			=TIMER_UP_COUNTER;  //
	contador_segundos.TIMx_Config.TIMx_InterruptEnable	=TIMER_INT_ENABLE;  //

	/*	Configuramos el Timer */
	timer_Config(&contador_segundos);

	//	Encedemos el Timer.
	timer_SetState(&contador_segundos,SET);

	blinkyTimer.pTIMx 							= TIM5;
	blinkyTimer.TIMx_Config.TIMx_Prescaler		=16000;  //	Genera incrementos de 1ms
	blinkyTimer.TIMx_Config.TIMx_Period			=250;  //	el prescaler lo ajusta 1ms, entonces lo quiero a 250ms, y es la multiplicacion de uno con el otro.
	blinkyTimer.TIMx_Config.TIMx_mode			=TIMER_UP_COUNTER;  //
	blinkyTimer.TIMx_Config.TIMx_InterruptEnable	=TIMER_INT_ENABLE;  //


	/*	Configuramos el Timer */
	timer_Config(&blinkyTimer);

	//	Encedemos el Timer.
	timer_SetState(&blinkyTimer,SET);

	display.pTIMx 							= TIM3;
	display.TIMx_Config.TIMx_Prescaler		=16000;  //	Genera incrementos de 1ms
	display.TIMx_Config.TIMx_Period			=2;  //	60FPS ultra calidad gamer. Se tuvo que subir porque no se veía fluido el refresco, antes era 15, que significaban 60 FPS
	display.TIMx_Config.TIMx_mode			=TIMER_UP_COUNTER;  //
	display.TIMx_Config.TIMx_InterruptEnable	=TIMER_INT_ENABLE;  //


	/*	Configuramos el Timer */
	timer_Config(&display);

	//	Encedemos el Timer.
	timer_SetState(&display,SET);


	/* Configuramos el USART2
	 * usamos las configs que el profe nos dejo en el documento de la tarea. */

	usart2.ptrUSARTx = USART2;
	usart2.USART_Config.baudrate =	USART_BAUDRATE_115200;
	usart2.USART_Config.datasize = USART_DATASIZE_8BIT;
	usart2.USART_Config.parity = USART_PARITY_NONE;
	usart2.USART_Config.stopbits = USART_STOPBIT_1;
	usart2.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart2.USART_Config.mode = USART_MODE_RXTX;

	usart_Config(&usart2);

	pinRX.pGPIOx = GPIOA;
	pinRX.pinConfig.GPIO_PinNumber		 = PIN_3;
	pinRX.pinConfig.GPIO_PinMode		=	GPIO_MODE_ALTFN;
	pinRX.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	pinRX.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	pinRX.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	pinRX.pinConfig.GPIO_PinAltFunMode 	=	AF7;

	gpio_Config(&pinRX);

	pinTX.pGPIOx = GPIOA;
	pinTX.pinConfig.GPIO_PinNumber		 = PIN_2;
	pinTX.pinConfig.GPIO_PinMode		=	GPIO_MODE_ALTFN;
	pinTX.pinConfig.GPIO_PinOutputType	=	GPIO_OTYPE_PUSHPULL;
	pinTX.pinConfig.GPIO_PinOutputSpeed	=	GPIO_OSPEED_MEDIUM;
	pinTX.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	pinTX.pinConfig.GPIO_PinAltFunMode 	=	AF7;

	gpio_Config(&pinTX);





	/* Configuramos el ADC */

	//el trimmer esta en el canal0, usamos las configs que el profe nos dejo
	trimmer.channel = CHANNEL_0;
	trimmer.resolution = RESOLUTION_12_BIT;
	trimmer.dataAlignment = ALIGNMENT_RIGHT;
	trimmer.interruptState = ADC_INT_ENABLE;
	trimmer.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;

	//cargamos la config.
	adc_ConfigSingleChannel(&trimmer);
	adc_StartSingleConv();


	fotoresistencia.channel = CHANNEL_1;
	fotoresistencia.resolution = RESOLUTION_12_BIT;
	fotoresistencia.dataAlignment = ALIGNMENT_RIGHT;
	fotoresistencia.interruptState = ADC_INT_ENABLE;
	fotoresistencia.samplingPeriod = SAMPLING_PERIOD_112_CYCLES;

// no se carga esta config porque solo puede haber uno cargado, por algo se llama configsinglechannel





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
		gpio_WritePin(&switcheoCentenas,1);
		gpio_WritePin(&switcheoDecenas,1);
		gpio_WritePin(&switcheoMillares,1);
		display_numbers(unidades);
		gpio_WritePin(&switcheoUnidades,0);
		break;
		}
	case 1:{
		gpio_WritePin(&switcheoCentenas,1);
		gpio_WritePin(&switcheoMillares,1);
		gpio_WritePin(&switcheoUnidades,1);
		display_numbers(decenas);
		gpio_WritePin(&switcheoDecenas,0);
		break;
		}
	case 2:{
		gpio_WritePin(&switcheoUnidades,1);
		gpio_WritePin(&switcheoDecenas,1);
		gpio_WritePin(&switcheoMillares,1);
		display_numbers(centenas);
		gpio_WritePin(&switcheoCentenas,0);
		break;
		}
	case 3:{
		gpio_WritePin(&switcheoCentenas,1);
		gpio_WritePin(&switcheoDecenas,1);
		gpio_WritePin(&switcheoUnidades,1);
		display_numbers(millares);
		gpio_WritePin(&switcheoMillares,0);
		break;
		}
	}
}


//funcion para separar el numero, asi recibimos las unidades, decenas, centenas y millares del numero que nos arroje las lecturas de ADC, etc.
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



/* en este callback se sube y baja la bandera (UIF) permitiendo que se de la interrupcion y no quede
 * interrumpido indefinidamente, al mismo tiempo siendo este el timer que usamos para nuestro contador de segundos
 * hacemos que durante esta interrupcion (de 1 seg, valga la redundancia) se nos sume a una variable auxiliar que llamamos
 * contador, que es la que nos ayuda en la parte del codigo del display, para ir mostrando los numeros en el momento que
 * necesitamos. Es decir cuando se hace la interrupcion, esta misma (administrada por el timer2) suma +1 al contador.
 * Esta parte del codigo ademas de mandar la interrupcion, tambien detiene la misma.
 */

//contador del modo 4, subimos la bandera.
void Timer2_Callback(void){
	flag_conteo = 1;
}


//sube la bandera del display
void Timer3_Callback(void){
	display_flag = 1;
}

/* este el callback del Led de estado, usamos el TooglePin para que se enciende y se apague, es la unico para lo
 * que nos sirve esta interrupcion. A grandes rasgos sirve para saber que el sistema funciona.

 Ademas de eso, agregamos la bandera para que se de la conversion de adc y la inicializacion de la variable sendMsg*/
void Timer5_Callback(void){
	gpio_TooglePin(&userLed);
	flag_adc_conversion = 1;
	sendMsg = 1;

}


//Callbacks del EXTI.
//subimos la bandera del clock, y leemos el dato que tenemos del encoder:exti_Data, y lo enviamos por pinData.
void callback_ExtInt13(void){
	flag_clock = 1;
	exti_Data = gpio_ReadPin(&pinData);
}

void callback_ExtInt5(void){
	changeModo = 1;
}

// Callback de ADC. adcData recibe el valor del adc. Subimos la bander del adc.
void adc_CompleteCallback (void){
	adcData = adc_GetValue();
	flag_adc = 1;

}
//Callbacks de USART2. usartdata es la variable que recibe lo recolectado por usart, recordemos que usart esta configurado para trabajar con RX y TX al tiempo.
void usart2_RxCallback(void){
	flag_usart = 1;
	usart_data = usart_getRxData();
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
