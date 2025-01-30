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
#include <stm32f4xx.h>

/* Definicion de los estados de la FSM */

extern void configMagic(void);
extern void clear_Scanf(void);
extern void read_input(char *data);

typedef enum{

	STATE_IDLE = 0,
	STATE_MODO_RGB ,
	STATE_REFRESH_DISPLAY ,
	STATE_GIRO_ENCODER

} FSM_STATES;

typedef struct
{
FSM_STATES state;
}fsm_states_t;



#endif /* MAIN_H_ */



