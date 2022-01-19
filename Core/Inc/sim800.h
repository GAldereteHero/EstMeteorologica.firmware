/*
 * moduleA6.h
 *
 *  Created on: 9 dic. 2020
 *      Author: Gonzalo
 */

#ifndef INC_MODULEA6_H_
#define INC_MODULEA6_H_


#endif /* INC_MODULEA6_H_ */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern void Error_Handler(void);


void initModule(void);
void resetModule(void);
_Bool checkAnswer(uint8_t answerOption);
//void updateTemperature(char *pAux, uint8_t temp);
//void updatePluviometer(char *pAux, uint8_t mili);
_Bool uploadDataToServer(uint8_t topic, uint8_t value);


