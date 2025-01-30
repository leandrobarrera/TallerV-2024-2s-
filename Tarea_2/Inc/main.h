/*
 * main.h
 *
 *  Created on: Jan 29, 2025
 *      Author: leandro
 */

#ifndef MAIN_H_
#define MAIN_H_


#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Definicion de los estados de la FSM */

typedef enum{

	STATE_IDLE = 0,
	STATE_MODO_RGB ,
	STATE_REFRESH_DISPLAY ,
	STATE_GIRO_ENCODER

} FSM_STATES;


#endif /* MAIN_H_ */
