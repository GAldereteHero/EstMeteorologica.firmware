/*
 * moduleA6.c
 *
 *  Created on: 9 dic. 2020
 *      Author: Gonzalo
 */

/**********Includes**********/

#include<stdio.h>
#include<string.h>
#include "gpio.h"
#include "usart.h"
#include "sim800.h"

/**********Defines**********/

//#define K_EMBUDO	0.794
//#define CHIP_PERSONAL
#define CHIP_CLARO

/*********Variables***********/
extern uint8_t getTemp(void);

extern uint32_t cont;
extern uint16_t tem;

char body[200];

char rxBuffer[200];
uint8_t sizeRxBuffer = sizeof(rxBuffer);

//char msg[3]    = {0x1A,0x0D,0x0A};
//char msg0[4]   = "AT\r\n";
//char msg1[11]  = "AT+CGATT?\r\n";
//char msg2[12]  = "AT+CGATT=1\r\n";
//char msg3[40]  = "AT+CGDCONT=1,\"IP\",\"datos.personal.com\"\r\n";
//char msg4[14]  = "AT+CGACT=1,1\r\n";
//char msg5[13]  = "ATD*99***1#\r\n";
//char msg6[5]   = "+++\r\n";
//char msg7[10]  = "AT+CIFSR\r\n";
//char msg8[43]  = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n";
//char msg9[12]  = "AT+CIPSEND\r\n";
//char msg10[63] = "GET https://api.thingspeak.com/update?api_key=Z0OCNIE9923JVEEG&";
//char msg11[13] = "AT+CIPSHUT?\r\n";
//char msg12[10] = "AT+CREG?\r\n";
//char msg13[80];
//
//char msg14[14] = "AT+CIPSTATUS\r\n";
//char msg15[53] = "AT+CSTT=\"igprs.claro.com.ar\",\"ctigprs\",\"ctigprs999\"\r\n";
//char msg16[10] = "AT+CIICR\r\n";
//char msg17[9] = "AT+IPR?\r\n";


const char *ATCOMM[] = {
						"AT\r\n",	//0
						"AT+CREG?\r\n", //1
						"AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", //2
						"AT+SAPBR=3,1,\"APN\",\"igprs.claro.com.ar\"\r\n", //3
						"AT+SAPBR=1,1\r\n", //4
						"AT+HTTPINIT\r\n", //5
						"AT+HTTPPARA=\"CID\",1\r\n", //6
						"AT+HTTPPARA=\"URL\",\"https://influx.volentini.com/write?db=facetPrecipitaciones\"\r\n", //7
						"AT+HTTPPARA=\"USERDATA\",\"Authorization: Basic ZnFjZXQGRnFjZXQxODAw;Host: influx.volentini.com;Content-type: application/json;Connection: keep-alive\"\r\n",
						"AT+HTTPDATA=100,1000\r\n", //9
						//"lluvia,sensor=1 value=\"4\"",
						"AT+HTTPSSL=1\r\n", //10
						"AT+HTTPACTION=1\r\n", //11
						"AT+HTTPTERM\r\n", //12
						"AT+SAPBR=0,1\r\n"  //13


				};


#ifdef CHIP_PERSONAL
typedef enum{
	STATE_1, // Enviamos por UART el comando "AT"
	STATE_2, // Enviamos por UART el comando "AT+CGATT?"
	STATE_3, // Enviamos por UART el comando "AT+CGDCONT=1"
	STATE_4, // Enviamos por UART el comando "AT+CGDCONT=1,\"IP\",\"datos.personal.com\""
	STATE_5, // Enviamos por UART el comando "AT+CGACT=1,1"
	STATE_6, // Enviamos por UART el comando "ATD*99***1#"
	STATE_7, // Enviamos por UART el comando "+++"
	STATE_8, // Enviamos por UART el comando "AT+CIFSR"
	STATE_9, // Enviamos por UART el comando "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80"
	STATE_10, // Enviamos por UART el comando "AT+CIPSEND" + "GET https://api.thingspeak.com/update?api_key=Z0OCNIE9923JVEEG&field1=1" + "<SUB>"
	STATE_11 // Enviamos por UART el comando "AT+CIPSHUT"
} statesMefUart;
#endif

//#ifdef CHIP_CLARO
//typedef enum{
//	STATE_1, // Enviamos por UART el comando "AT"
//	STATE_2, // Enviamos por UART el comando "AT+CIPSTATUS"
//	STATE_3, // Enviamos por UART el comando "AT+CSTT="igprs.claro.com.ar","ctigprs","ctigprs999""
//	STATE_4, // Enviamos por UART el comando "AT+CIICR"
//	STATE_5, // Enviamos por UART el comando "AT+CIFSR"
//	STATE_6, // Enviamos por UART el comando "AT+CIPSTART="TCP","api.thingspeak.com",80"
//	STATE_7, // Enviamos por UART el comando "AT+CIPSEND" + "GET https://api.thingspeak.com/update?api_key=Z0OCNIE9923JVEEG&field1=1" + "<SUB>"
//	STATE_8, // Enviamos por UART el comando "AT+CIPSHUT"
//} statesMefUart;
//#endif

#ifdef CHIP_CLARO
typedef enum{
	STATE_1, // Enviamos por UART el comando "AT"
	STATE_2, // Enviamos por UART el comando "AT+SAPBR=3,1,Contype,GPRS" Este comando establece los parametros de conexión GPRS
	STATE_3, // Enviamos por UART el comando "AT+SAPBR=3,1,"APN","igprs.claro.com.ar""
	STATE_4, // Enviamos por UART el comando "AT+SAPBR=1,1" Este comando abre la portadora con los parámetros previamente definidos.
	STATE_5, // Enviamos por UART el comando "AT+HTTPINIT" 	Este comando inicializa el servicio HTTP.
	STATE_6, // Enviamos por UART el comando "AT+HTTPPARA=CID,"1"" Este comando define el perfil de la portadora. Normalmente siempre es 1.
	STATE_7, // Enviamos por UART el comando "AT+HTTPPARA="URL","https://influx.volentini.com/write?db=facetPrecipitaciones"
	STATE_8, // Enviamos por UART el comando "AT+HTTPPARA=\"USERDATA\",\"Authorization: Basic ZnFjZXQGRnFjZXQxODAw;Host: influx.volentini.com;Content-type: application/json;Connection: keep-alive"
	STATE_9, // Enviamos por UART el comando "AT+HTTPDATA=100,5000" Este comando define el tamaño de los datos de usuario a transferir en bytes. El segundo número indica el tiempo (milisegundos) en el que deben llegar los datos antes de que el SIM800 aborte la petición
	STATE_10, // Enviamos por UART la carga util
	STATE_11, // Enviamos por UART el comando "AT+HTTPSSL=1" Para activar las peticiones de tipo HTTPS
	STATE_12, // Enviamos por UART el comando "AT+HTTPACTION=1" Con HTTPACTION se define el método de transferencia. Se aplica lo siguiente: 0=GET, 1=POST, 2=HEAD. La respuesta incluye el código de estado del servidor y la longitud de los datos.
	STATE_13, // Enviamos por UART el comando "AT+HTTPTERM" Este comando aborta una petición HTTP en curso.
	STATE_14  // Enviamos por UART el comando "AT+SAPBR=0,1" Este comando termina la portadora GRPS y sólo se ejecuta después de la transferencia de datos.
} statesMefUart;
#endif


statesMefUart state;

/*********Funciones***********/

_Bool checkAnswer( uint8_t answerOption ){

	switch ( answerOption ){
		case 1:{
			for(uint8_t i = 0; i <= sizeRxBuffer; i++){
				if(rxBuffer[i] == 'O' && rxBuffer[i+1] == 'K' ){ return 1; }
			}
			return 0;
		}break;

		case 2:{
			for(uint8_t i = 0; i <= sizeRxBuffer; i++){
				if(rxBuffer[i] == 'C' && rxBuffer[i+1] == 'O' ){ return 1; }
			}
			return 0;
		}break;

		case 3:{
			for(uint8_t i = 0; i <= sizeRxBuffer; i++){
				if(rxBuffer[i] == '0' && rxBuffer[i+2] == '1' ){ return 1; } //Cambie el primer 1 por un 0
			}
			return 0;
		}break;

		default:{
			return 0;
		}break;
	}

}

void initModule(void){

//	HAL_GPIO_WritePin(PWR_A6_GPIO_Port, PWR_A6_Pin, GPIO_PIN_SET);
//	HAL_Delay(2000);
//	HAL_GPIO_WritePin(PWR_A6_GPIO_Port, PWR_A6_Pin, GPIO_PIN_RESET);
//	HAL_Delay(15000);

//	memset(rxBuffer, 0, sizeRxBuffer);
//	HAL_UART_Transmit(&huart1, (uint8_t *) ATCOMM[1], strlen(ATCOMM[1]), 1000);	// "AT?"
////	HAL_UART_Transmit(&huart1, (uint8_t*) ATCOMM[1], 12, 1000);	// "AT?"
//	while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 1000) == HAL_BUSY){}
////	HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);
//	printf(rxBuffer);
//	printf("\n\n");

	for(uint8_t i=1; i<=3; i++){

		memset(rxBuffer, 0, sizeRxBuffer);
		HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[0], strlen(ATCOMM[0]), 500);	// "AT"
		while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 1000) == HAL_BUSY){}
//		HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);
		printf(rxBuffer);
		printf("\n");

		memset(rxBuffer, 0, sizeRxBuffer);
		HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[1], strlen(ATCOMM[1]), 500);	// "AT+CREG?"
		while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 1000) == HAL_BUSY){}
//		HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);
		printf(rxBuffer);
		printf("\n");

		if( checkAnswer(3) == 1 ){ i = 4;}
		else{/*Error_Handler();*/}
	}
}

void resetModule(void){

	HAL_GPIO_WritePin(RST_A6_GPIO_Port, RST_A6_Pin, GPIO_PIN_SET);
	HAL_Delay(3000);
	HAL_GPIO_WritePin(RST_A6_GPIO_Port, RST_A6_Pin, GPIO_PIN_RESET);
}

//void updateTemperature(char *pAux, uint8_t temp){
//
//	memset(msg13, 0, sizeof(msg13));
//	sprintf(pAux,"%sfield2=%d\r\n",msg10,temp);
//}

//void updatePluviometer(char *pAux, uint8_t mili){
//
//	memset(msg13, 0, sizeof(msg13));
//	sprintf(pAux,"%sfield1=%d\r\n",msg10,mili);
//}

#ifdef CHIP_PERSONAL

_Bool uploadDataToServer (uint8_t topic){

	uint8_t errorCount = 1;
	state = STATE_1;

	for (uint8_t loopCount = 0; loopCount <= 10; loopCount++ ){
		if ( errorCount <= 3){

			switch( state ){

				case STATE_1:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg0, sizeof(msg0), 500);	// "AT"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 2000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(1) == 1 ){ state = STATE_2; }
					else{ errorCount++; state = STATE_1; loopCount--;}
				}break;

				case STATE_2:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg1, sizeof(msg1), 500);	// "AT+CGATT?"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 2000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(1) == 1 ){ state = STATE_3; }
					else{ errorCount++; state = STATE_2; loopCount--;}
				}break;

				case STATE_3:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg2, sizeof(msg2), 500);	// "AT+CGDCONT=1"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 2000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(1) == 1 ){ state = STATE_4; }
					else{ errorCount++; state = STATE_3; loopCount--;}
				}break;

				case STATE_4:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg3, sizeof(msg3), 500);	//"AT+CGDCONT=1,\"IP\",\"datos.personal.com\""
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 2000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(1) == 1 ){ state = STATE_5; }
					else{ errorCount++; state = STATE_4; loopCount--;}
				}break;

				case STATE_5:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg4, sizeof(msg4), 500);	//"AT+CGACT=1,1"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 2000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(1) == 1 ){ state = STATE_6; }
					else{ errorCount++; state = STATE_5; loopCount--;}
				}break;

				case STATE_6:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg5, sizeof(msg5), 500);	//"ATD*99***1#"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 2000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(2) == 1 ){ state = STATE_7; }
					else{ errorCount++; state = STATE_6; loopCount--;}
				}break;

				case STATE_7:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg6, sizeof(msg6), 500);	//"+++"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 2000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(1) == 1 ){ state = STATE_8; }
					else{ errorCount++; state = STATE_7; loopCount--;}
				}break;

				case STATE_8:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg7, sizeof(msg7), 500);	//"AT+CIFSR"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 2000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(1) == 1 ){ state = STATE_9; }
					else{ errorCount++; state = STATE_8; loopCount--;}
				}break;

				case STATE_9:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg8, sizeof(msg8), 500);	//"AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 10000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(1) == 1 ){ state = STATE_10; }
					else{ errorCount++; state = STATE_9; loopCount--;}
				}break;

				case STATE_10:{
					memset(rxBuffer, 0, sizeRxBuffer);

					if( topic == 1){ updatePluviometer(msg13,cont);}
					else{ updateTemperature(msg13, getTemp());}

					HAL_UART_Transmit(&huart1, (uint8_t *)msg9, sizeof(msg9), 500);	// "AT+CIPSEND"
					HAL_Delay(1000);
					HAL_UART_Transmit(&huart1, (uint8_t *)msg13, sizeof(msg13), 500);	//"GET https://api.thingspeak.com/update?api_key=Z0OCNIE9923JVEEG&field1="
					HAL_UART_Transmit(&huart1, (uint8_t *)msg, sizeof(msg), 500);		// "<SUB>"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 10000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);

					if( checkAnswer(1) == 1 ){ state = STATE_11; }
					else{ errorCount++; state = STATE_1; loopCount = 0;}
				}break;

				case STATE_11:{
					HAL_UART_Transmit(&huart1, (uint8_t *)msg11, sizeof(msg11), 500);		//"AT+CIPSHUT"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 2000) == HAL_BUSY){}
					HAL_UART_Transmit(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer), 500);
					state = STATE_1;
				}break;

				default:{
					state = STATE_1;
					loopCount = 11;
				}break;
			}
		}
	else{
		errorCount = 1;
		loopCount = 11;
		return 0;
	}
	}
	return 1;
}

#endif

#ifdef CHIP_CLARO

_Bool uploadDataToServer (uint8_t topic, uint8_t value){

	uint8_t errorCount = 1;
	state = STATE_1;

	for (uint8_t loopCount = 0; loopCount <= 13; loopCount++ ){
		if ( errorCount <= 3){

			switch( state ){

				case STATE_1:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[0], strlen(ATCOMM[0]), 500);	// "AT"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_2; }
					else{ errorCount++; state = STATE_1; loopCount--;}
				}break;

				case STATE_2:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[2], strlen(ATCOMM[2]), 500);	// "AT+SAPBR=3,1,Contype,GPRS"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_3; }
					else{ errorCount++; state = STATE_2; loopCount--;}
				}break;

				case STATE_3:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[3], strlen(ATCOMM[3]), 500);	// "AT+SAPBR=3,1,"APN","igprs.claro.com.ar""
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_4; }
					else{ errorCount++; state = STATE_3; loopCount--;}
				}break;

				case STATE_4:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[4], strlen(ATCOMM[4]), 500);	 // "AT+SAPBR=1,1"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_5; }
					else{ errorCount++; state = STATE_4; loopCount--;}
				}break;

				case STATE_5:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[5], strlen(ATCOMM[5]), 500);	// "AT+HTTPINIT"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_6; }
					else{ errorCount++; state = STATE_5; loopCount--;}
				}break;

				case STATE_6:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[6], strlen(ATCOMM[6]), 500);	//"AT+HTTPPARA=CID,"1""
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_7; }
					else{ errorCount++; state = STATE_6; loopCount--;}
				}break;

				case STATE_7:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[7], strlen(ATCOMM[7]), 500);	//"AT+HTTPPARA="URL","https://influx.volentini.com/write?db=facetPrecipitaciones"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_8; }
					else{ errorCount++; state = STATE_7; loopCount--;}
				}break;

				case STATE_8:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[8], strlen(ATCOMM[8]), 500);	//"AT+HTTPPARA="USERDATA","Authorization: Basic..."
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_9; }
					else{ errorCount++; state = STATE_8; loopCount--;}
				}break;

				case STATE_9:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[9], strlen(ATCOMM[9]), 500);	//"AT+HTTPDATA=100,5000"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					state = STATE_10;


//					if( checkAnswer(1) == 1 ){ state = STATE_10; }
//					else{ errorCount++; state = STATE_9; loopCount--;}


				}break;

				case STATE_10:{
					memset(rxBuffer, 0, sizeRxBuffer);
					memset(body, 0, sizeof(body)/sizeof(char));

//					if( topic == 1){ updatePluviometer(msg13,cont);}
//					else{ updateTemperature(msg13, getTemp());}

					if( topic == 1){sprintf(body,"lluvia,sensor=1 value=\"%d\"",value);}
					else{sprintf(body,"temperatura,sensor=1 value=%d",value);}


					HAL_UART_Transmit(&huart1, (uint8_t *)body, strlen(body), 500);
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_11; }
					else{ errorCount++; state = STATE_10; loopCount = 0;}


				}break;


				case STATE_11:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[10], strlen(ATCOMM[10]), 500);	//"AT+HTTPSSL=1"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_12; }
					else{ errorCount++; state = STATE_11; loopCount--;}
				}break;

				case STATE_12:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[11], strlen(ATCOMM[11]), 500);	//"AT+HTTPACTION=1"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 10000) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");


					if( checkAnswer(1) == 1 ){ state = STATE_13; }
					else{ errorCount++; state = STATE_12; loopCount--;}
				}break;


				case STATE_13:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[12], strlen(ATCOMM[12]), 500);	//"AT+HTTPTERM"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_14; }
					else{ errorCount++; state = STATE_13; loopCount--;}
				}break;


				case STATE_14:{
					memset(rxBuffer, 0, sizeRxBuffer);
					HAL_UART_Transmit(&huart1, (uint8_t *)ATCOMM[13], strlen(ATCOMM[13]), 500);		//"AT+SAPBR=0,1"
					while (HAL_UART_Receive(&huart1, (uint8_t*) rxBuffer, sizeof(rxBuffer), 500) == HAL_BUSY){}
					printf(rxBuffer);
					printf("\n");

					if( checkAnswer(1) == 1 ){ state = STATE_1; }
					else{ errorCount++; state = STATE_14; loopCount--;}
				}break;

				default:{
					state = STATE_1;
					loopCount = 10;
				}break;
			}
		}
	else{
		errorCount = 1;
		loopCount = 10;
		return 0;
	}
	}
	return 1;
}

#endif


