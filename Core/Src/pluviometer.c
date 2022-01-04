/*
 * pluviometer.c
 *
 *  Created on: Dec 3, 2020
 *      Author: Usuario
 */


/*********Includes************/

#include <stdio.h>
#include "tim.h"
#include "moduleA6.h"

/*********Variables***********/

_Bool firstInit = 1;
_Bool init = 1;

const uint32_t timeThreshold = 200;
uint32_t startTime = 0;

uint32_t cont = 0;
uint32_t incrCont = 0;


/********* Variables externas **********/

extern char msg13[80];

/********* Funciones **********/

// Función que realiza un antirebote por Soft e incrementa el contador del balancín.
void getRainfall(void){

//	if( HAL_GetTick() - startTime > timeThreshold){
		cont += 1;
//		startTime = HAL_GetTick();
//	}
}

void sendIncrement(void){

	if (init == 1){
		init = 0;
		initModule();

//		updatePluviometer(msg13,cont);
		if( uploadDataToServer(1) == 0){ /*Error_Handler();*/}
		cont = 0;
	}

	if(incrCont == 5){
		incrCont = 0;

		if(cont != 0){
//			updatePluviometer(msg13,cont);
			if( uploadDataToServer(1) == 0){ /*Error_Handler();*/}
			cont = 0;
		}
		else{
			firstInit = 1;
			init = 1;

			HAL_TIM_Base_Stop_IT(&htim4);
			__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
			__HAL_TIM_SET_COUNTER(&htim2, 0);
			HAL_TIM_Base_Start_IT(&htim2);

			resetModule();
		}
	}
	else{
		incrCont +=1;
	}
}

void startRain(void){

	getRainfall();

	if(firstInit == 1){

		firstInit = 0;
		HAL_TIM_Base_Stop_IT(&htim2);
		HAL_TIM_Base_Start_IT(&htim4);
//		sendIncrement();
	}
}

