/* Copyright 2017-2018, Eric Pernia
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/** @file 	portserie.c
 ** @brief 	Configurción del puerto serie USB y rs232
 **
 **| REV | YYYY.MM.DD | Autor               | Descripción de los cambios                              |
 **|-----|------------|---------------------|---------------------------------------------------------|
 **|   1 | 2020.07.1  | luis Mariano Campos | Version inicial 									      |
 **|     |            |                     | 				 									      |
 ** @addtogroup aplicacion principal
 ** @{ */

/*==================[inlcusiones]============================================*/
#include "commandprocessing.h"
#include "steppermotor.h"
#include "pid.h"

void commandProcessingQueueCreate(void) {
	processingComandQueue = xQueueCreate(SIZECOMMANDQUEUE, sizeof(char*));

	if (processingComandQueue == NULL) {/*Si devolvio NULL es muy probable que no haya suficiente memoria para crear la cola*/
		gpioWrite(LED1, ON);
		while (1)
			;/*Se queda bloquedo el sistema hasta que venga el personal de mantenimiento*/
	}
}
void commandProcessingTask(void * taskParmPtr) {

	char *pCommandToProcess = NULL;
	stepperMotorEnable_t enableMotor;
	stepperMotorMicroSteps_t microSteps;
	stepperMotorDirection_t directionMotor;
	uint16_t sp;
	float spAux;
	uint8_t index = 0;
	uint8_t i=0;
	bool_t validCommand=TRUE;
	uint16_t numberOfSteps;

	while (TRUE) {
		if (xQueueReceive(processingComandQueue, &pCommandToProcess,
				portMAX_DELAY) == pdTRUE) {
			while (*(pCommandToProcess + index) != '\0') {
				taskENTER_CRITICAL();
				printf("El comando es:%c\n", *(pCommandToProcess + index));
				taskEXIT_CRITICAL();
				//gpioToggle( LED3 );
				index++;
			}
			index = 0;
			switch (*(pCommandToProcess + index)) {
			case 'M':
				switch (*(pCommandToProcess + 1)) {
				case 'E':
					enableMotor = STEPPER_ENABLE;
					xQueueSend(stepperMotorEnableQueue, &enableMotor,portMAX_DELAY);
					break;
				case 'D': //deshabilitar el motor
					enableMotor = STEPPER_DISABLE;
					xQueueSend(stepperMotorEnableQueue, &enableMotor,portMAX_DELAY);
					break;
				case 'M': //establezco los micropasos
					switch (*(pCommandToProcess + 2)) {
					case 'S':
						switch (*(pCommandToProcess + 3)) {
						case 'F': //establecer los micropasos a FullStep
							microSteps = RESOLUTION_FULL_STEP;
							xQueueSend(stepperMotorMicroStepsQueue, &microSteps,
											portMAX_DELAY);
							break;
						case 'H': //establecer los micropasos a halfstep
							microSteps = RESOLUTION_HALF_STEP;
							xQueueSend(stepperMotorMicroStepsQueue, &microSteps,
																		portMAX_DELAY);
							break;
						case '0':
							switch (*(pCommandToProcess + 4)) {
							case '4': //Microstep 1/4
								microSteps=RESOLUTION_4_STEP;
								xQueueSend(stepperMotorMicroStepsQueue, &microSteps,
																			portMAX_DELAY);
								break;
							case '8': //Microstep 1/8
								microSteps=RESOLUTION_8_STEP;
								xQueueSend(stepperMotorMicroStepsQueue, &microSteps,
																			portMAX_DELAY);
								break;
							default: //command invalid
								break;
							}
							break;
						case '1':
							if (*(pCommandToProcess + 4) == '6') {
								//llamar a la función Microstep 1/16
								microSteps=RESOLUTION_16_STEP;
								xQueueSend(stepperMotorMicroStepsQueue, &microSteps,
																			portMAX_DELAY);
							} else {
								// comand invalid
							}
							break;
						case '3':
							if (*(pCommandToProcess + 4) == '2') {
								//llamar a la función Microstep 1/32
								microSteps = RESOLUTION_32_STEP;
								xQueueSend(stepperMotorMicroStepsQueue, &microSteps,
																			portMAX_DELAY);
							} else {
								// comand invalid
							}
							break;
						default://command invalid;
							break;
						}
						break;
					}
					break;
				case 'T':
							switch(*(pCommandToProcess + 2)){
							case 'H':
								directionMotor = STEPPER_RIGHT_OPEN;

								xQueueSend(stepperMotorDirectionQueue, &directionMotor,portMAX_DELAY);
								break;
							case 'A':
								directionMotor = STEPPER_LEFT_CLOSE;
								xQueueSend(stepperMotorDirectionQueue, &directionMotor,portMAX_DELAY);
								break;
							default://
								break;
							}
					break;
				case 'S': //establezco la cantidad de pulso, es decir los pasos
						i=2;
						validCommand=TRUE;
						while(*(pCommandToProcess + i)!='\0'){
							if('0'<=*(pCommandToProcess + i) && '9'>=*(pCommandToProcess + i)){

							}
							else{
								//invalid Command
								validCommand=FALSE;
							}
							i++;
						}
						if(validCommand==FALSE){
							printf("Comando Invalido.....\n");
						}
						else{
							numberOfSteps = commandProcessingConverterCaracterToDecimal((pCommandToProcess + 2),4);
							printf("numero de pasos:%d\n",numberOfSteps);
							xQueueSend(stepperMotorPulseQueue, &numberOfSteps,portMAX_DELAY);

						}
					break;
				case 'A': //establezco el angulo de giro del eje del motor
					break;
				case 'F': // establezo una cantidad d vueltas determinada
					break;

				default:
					break; // comndo invalido;
				}

				break;
			case 'S': if(*(pCommandToProcess + 1)=='P'){
						sp = commandProcessingConverterCaracterToDecimal((pCommandToProcess + 2),3);
						taskENTER_CRITICAL();
						printf("Set Point:%d\n",sp);
						taskEXIT_CRITICAL();
						switch(sp){
									case 0: 	pid.setPoint = 0.0000000;
									break;
									case 25: 	pid.setPoint = 0.277750;
									break;
									case 50: 	pid.setPoint = 0.43550;
									break;
									case 75: 	pid.setPoint = 0.653250;
									break;
									case 100: 	pid.setPoint = 0.97100;
									break;
									default:
										break;
									}
						//xQueueSend(pidSetPointQueue, &sp,portMAX_DELAY);
						//setingSetPiontPID(sp);
						//spAux = (float)(sp*0.00871);
						//setingSetPiontPID(&pid,(float) (sp*0.00871*));

						//setingSetPiontPID(&pid,0.4355);
						//printf("Set Point:%.5f %\n",spAux);
					}
			break;
			default: //command Invalid
				break;
			}
		}
	}
}

uint16_t commandProcessingConverterCaracterToDecimal(char * pointer, uint8_t length) {
	uint32_t num=0;
		    uint32_t digito = 0;
		    uint32_t tam = length;
		    uint32_t expo = 0;
		    uint32_t pot=1;
		    uint32_t i;

		    for(i=tam ;i>0;--i)
		    {
		        digito=(uint32_t) (*(pointer+(i-1))-48);

		        expo = tam-i;
		        pot=1;
		        while( expo > 0)//para sacar la potencia
		        {
		                pot  = pot*10  ;
		                expo = expo-1 ;
		        }
		    num = num+digito*pot;
		    }
		    return num;
	}
