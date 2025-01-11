/*
 * i2c_driver_hal.h
 *
 *  Created on: 10/09/2024
 *      Author: lbarreras
 */


#ifndef I2C_DRIVER_HAL_H_
#define I2C_DRIVER_HAL_H_

#include<stm32f4xx.h>
#include"gpio_driver_hal.h"
 enum{
	 eI2C_WRITE_DATA	= 0,
	 eI2C_READ_DATA
 };
enum{
	eI2C_MODE_SM = 0,
	eI2C_MODE_FM
};


#define I2C_MAIN_CLOCK_4_MHz 	4
#define I2C_MAIN_CLOCK_16_Mhz 	16
#define I2C_MAIN_CLOCK_20_Mhz	20

#define I2C_MODE_SM_SPEED	80
#define I2C_MODE_FM_SPEED	13

#define I2C_MAX_RISE_TIME_SM	17
#define I2C_MAX_RISE_TIME_FM	5

 typedef struct
 {
	 I2C_TypeDef *pI2Cx;
	 uint8_t slaveAddress;
	 uint8_t i2c_mode;
	 uint8_t i2c_mainClock;
	 uint8_t i2c_data;

 }I2C_Handler_t;

 /*prototipos de las funciones publicas */
 void i2c_Config(I2C_Handler_t *pHnadlerI2C);
 uint8_t i2c_ReadSingleRegister(I2C_Handler_t *pHanlderI2C, uint8_t regToRead);
 uint8_t i2c_ReadManyRegisters(I2C_Handler_t *pHandlerI2C, uint8_t regToRead, uint8_t *bufferData, uint8_t numberOfBytes);
 void i2c_WriteSingleRegisters(I2C_Handler_t *pHandlerI2C, uint8_t regToWrite, uint8_t newValue);
 void i2c_WriteManyRegisters(I2C_Handler_t *pHandlerI2C, uint8_t regToWrite, uint8_t *bufferRxData, uint8_t numberOfBytes);


 /*nuevas funciones*/
 void i2c_StartTransaction(I2C_Handler_t *ptrHandlerI2C);
 void i2c_SendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite);
 void i2c_SendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddress);
 void i2c_SendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);
 void i2c_StopTransaction(I2C_Handler_t *ptrHandlerI2C);




 //void i2c_setPins(GPIO_Hnalder_t	*setSdaPin,  GPIO_Handler_t *setScalPin);
 //void i2c_clearBussyFlagsState(I2C_Handler_t *pHandlerI2C);

#endif /*I2C_DRIVER_HAL_H_ */


