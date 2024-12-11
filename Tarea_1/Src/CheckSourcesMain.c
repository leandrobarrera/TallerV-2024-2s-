/*
 * CheckSourcesMain.c
 *
 *  Created on: Dec 2, 2024
 *      Author: namontoy
 *      Edited by: lbarreras
 */

#include <stdint.h>
#include <stdio.h>
#include <stm32f4xx.h>

/* Definiciond de variables */
uint32_t rotation;
int16_t temperatureValue;
int dummy;

// Definicion de las cabeceras de las funciones del main
extern void configMagic(void);
int16_t getTemperature(uint8_t getData);
uint32_t checkRotation(void);
uint8_t leaking(uint16_t const range);
uint8_t positiveFunction(uint8_t data);
int dostuff(char *data, int value);
uint8_t weirdFunction(uint8_t data);



/*
 * The main function, where everything happens.
 */
int main(void)
{
	/* Configuramos la magia! */
	configMagic();

	// Ciclo principal
	printf("Hola Mundo!\n");

	/* Probando diferentes funciones */
	checkRotation();
	getTemperature(23);
	leaking(350);
	positiveFunction(2);
	dostuff("abc", 1024);
	weirdFunction(26);

	while (1) {
	}

	return 0;
}

/* Describir cuales son los problemas que hay en esta función y como se pueden corregir
 * El último condicional iterado no esta bien posicionado debido a que la condicion para que
 * sea verdadera es contraria a la condicion del primer condicional; entonces esa funcion siempre
 * entrega un 1 nunca un 0, precisamente por esta ultima condicion.
 *
 * */
uint32_t checkRotation(void){
 if (rotation < 23){
	if(rotation > 25){
		if(rotation > 45){
			/*if(rotation < 23) { poner el 23 primero y luego el 25 y por ultimo 45, por ejemplo*/
				return 0;
				}
		return 1;
			}
		}
return 0;
	}


/* Describir cuales son los problemas que hay en esta función y como se pueden corregir
 * el error es que el contador i se esta sumando al principio del ciclo for
 * pero cuando finaliza este se resta por la misma cantidad, por tanto nunca se sale
 * de la funcion.
 *
 *
 * */
int16_t getTemperature(uint8_t getData){
	if(getData == 1){
		for(int i = 0; i < 10; i++){
			temperatureValue = i*35;
			/*i--; quitar i-- */
		}
		return temperatureValue;
	}
	return 0;
}

/* Describir cuales son los problemas que hay en esta función y como se pueden corregir
 * cuando se definio la cantidad la cantidad de elementos del array se definieron 10, no obstante
 * cuando los retornamos, el a[10] final es el numero de posiciones, que es totalmente distinto porque
 * se empieza desde 0, asi que estamos devolviendo uno mas que no existe.
 *
 *
 * */
uint8_t leaking(uint16_t const range){
	char a[10];

	/* Utilice una linea de codigo del ciclo FOR y luego la otra, ¿que observa en la salida del cppcheck?*/
	for(uint8_t size = 0; size < range; size++){
	//for(uint8_t size = 0; size < 350; size++){ cuando usamos este a[size] = size*2 se sale de los limites, la variable tipo char sobrepasa su rango, lo que arroja un error, ademas que el uso de esto es por si mismo redundante,porque arriba definimos que 350 es lo que recibe la funcion.
		a[size] = size*2;
	}

	/* Utilice una linea de codigo del ciclo FOR y luego la otra, ¿que observa en la salida del cppcheck?*/
	return a[9];
	/*return a[10]; cambiar a[10] por a[9] esta linea esta outofbondings, segun Cppcheck
	 * estamos sobrepasando el rango del tipo de variable char (que va desde -127 a 128)*/
}

/* Describir cuales son los problemas que hay en esta función y como se pueden corregir
 * el retorno del else devuelve lo mismo que el primer condicional, por tanto sobra esa operacion.
 *
 *
 * */
uint8_t positiveFunction(uint8_t data){
	if(data == 1){
		return 1;
	}else{
		/*return 1; cambiar por un valor distinto del condicional igual para darle utilidad a la funcion */
		return 2;
	}
}

/* Describir cuales son los problemas que hay en esta función y como se pueden corregir
 * dentro del condicional no se puede asignar, el weird = data deberia estar fuera, ademas
 * nuestro return devuelven lo mismo.
 *
 *
 * */
uint8_t weirdFunction(uint8_t data){
	uint8_t weird = 10;
	uint8_t data = 15; //definimos antes del if
	if((weird & data) == 25){
	/*if((weird = data) == 25){ //weird = data no debe asignarse ahi */
		return 0;
	}else{
		/*return 0; cambiar valor de return */
		return 1;
	}
}

/* Describir cuales son los problemas que hay en esta función y como se pueden corregir
 * dentro de la funcion el puntero data se esta manejando como la entrada de un arreglo de datos de tipo
 * char donde se esta tratando de asignar a uno de los elementos de el array un numero por fuera
 * de los limites permitidos de los datos tipo char (de -127 a 128)
 *
 * */
int dostuff(char *data, int value){
	/*data[125] = 200; cambiar el 200 por un numero dentro del rango */
	data[125] = 100;
	dummy = value + 1;

	return 0;
}




/* Punto b.
 *
 *Cppcheck Es el comando base para ejecutar la herramienta Cppcheck, que realiza un análisis estático del código.

El parámetro --template=gcc ajusta el formato en el que se presentan los resultados del análisis, utilizando un estilo similar al del compilador GCC.
Esto facilita que los mensajes sean más comprensibles y fáciles de integrar en herramientas de desarrollo como la IDE que usamos, STM32CubeIDE.

Cuando se emplea --enable=all, se activa un análisis exhaustivo que considera todas las reglas disponibles. Este enfoque incluye revisiones estrictas,
chequeos de rendimiento y la detección de patrones comunes que podrían generar errores. Con ello se hace un análisis profundo y completo del código.

Con --platform=unix64, se configura la herramienta para que analice el código bajo un entorno Unix de 64 bits.
Esto impacta aspectos como los tamaños de los tipos de datos y otros detalles específicos de esa plataforma.

El uso de --std=c11 asegura que el análisis esté alineado con el estándar C11, verificando que el código cumpla con las reglas y características propias
de esta versión del lenguaje.

La opción --suppress=missingIncludeSystem se utiliza para evitar advertencias sobre la ausencia de headers típicos, como <stdio.h> o <stdlib.h>.
Esto resulta especialmente útil cuando se sabe que dichos archivos estarán disponibles en el entorno real, aunque la herramienta no pueda
localizarlos durante el análisis.

Mediante --verbose, se agrega información adicional al reporte, mostrando detalles sobre el progreso del análisis y proporcionando
mensajes útiles para entender las acciones que realiza la herramienta.
Por último, --suppress=unusedFunction elimina las alertas relacionadas con funciones que han sido declaradas pero aún no se están utilizando.
Esto es especialmente práctico en proyectos en sus primeras etapas, cuando algunas funciones están planeadas pero no implementadas.
 *
 */
