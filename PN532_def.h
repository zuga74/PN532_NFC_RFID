/*
 * PN532_def.h
 *
 *  Created on: 20 сент. 2021 г.
 *      Author: Rafael Boltachev
 */

#ifndef INC_PN532_DEF_H_
#define INC_PN532_DEF_H_

#include "main.h"

// ----- for change I2C to hi2c1  ------
//#define PN532_I2C_HANDLE 				hi2c1


// ----- for SPI STM32 HAL ? maybe------
//#define PN532_SPI_HANDLE 			hspi2
//#define PN532_TRANSMIT(b, s, t)		(HAL_SPI_Transmit(&PN532_SPI_HANDLE, b, s, t) == HAL_OK)
//#define PN532_RECEIVE(b, s, t)		(HAL_SPI_Receive(&PN532_SPI_HANDLE, b, s, t) == HAL_OK)


// ----- for MDR1986VE1 ------
//You need to write functions: transmit_to_pn532, receive_from_pn532, GetTick, Delay
//#include "MDR1986VE1T_IT.h"
//#include "MDR1986VE1T.h"
//BOOL transmit_to_pn532(uint8_t * buf, uint16_t size, uint32_t timeout);
//#define PN532_TRANSMIT(b, s, t)		transmit_to_pn532(b, s, t)
//BOOL receive_from_pn532(uint8_t * buf, uint16_t size, uint32_t timeout);
//#define PN532_RECEIVE(b, s, t) 			receive_from_pn532(b, s, t)
//uint32_t GetTick(void);
//#define PN532_GET_TICK					GetTick
//void Delay(uint32_t ms);
//#define PN532_DELAY						Delay


#endif /* INC_MYPN532_DEF_H_ */
