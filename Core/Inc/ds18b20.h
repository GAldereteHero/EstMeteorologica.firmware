 /* Description 		:This file contains all the functions prototypes for
  *                  the DS18B20 temperature sensor
 *  Created on: 3 dic. 2020
 *      Author: Usuario
 */


#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_



#endif /* INC_DS18B20_H_ */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */


void delay(uint16_t us);
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DS18B20_Start (void);
void DS18B20_Write (uint8_t data);
uint8_t DS18B20_Read (void);
//float getTemp(void);
uint8_t getTemp(void);
void sendTemp(void);


/* USER CODE END Includes */
