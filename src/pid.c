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
/** @file 	pid.c
 ** @brief 	Realiza el procesamiento del un sistema de control PID
 **
 **| REV | YYYY.MM.DD | Autor               | Descripción de los cambios                              |
 **|-----|------------|---------------------|---------------------------------------------------------|
 **|   1 | 2020.10.06 | luis Mariano Campos | Version inicial 									      |
 **|     |            |                     | 				 									      |
 ** @addtogroup aplicacion principal
 ** @{ */
/*==================[inlcusiones]============================================*/
#include "pid.h"

void PdiCreateQueue(void){
	pidSetPointQueue = xQueueCreate(SIZEQUEUEPIDSETPOINT,sizeof(uint16_t));
	if (pidSetPointQueue == NULL) {/*Si devolvio NULL es muy probable que no haya suficiente memoria para crear la cola*/
			gpioWrite(LED3, ON);
			while (1)
				;/*Se queda bloquedo el sistema hasta que venga el personal de mantenimiento*/
		}
}


/*==================[API]============================================*/
void startPid(Pid_t *pid,uint16_t sPoint, uint16_t sensorPresion,float setPoint, float inPut,float error,float iTerm,float outMax,float outMin,float kp, float ki, float outPut )
{
	pid->sPoint = sPoint;
	pid->sensorPresion = sensorPresion;
	pid->setPoint = setPoint;
	pid->inPut = inPut;
	pid->error = error;
	pid->iTerm =iTerm;
	pid->outMax = outMax;
	pid->outMin = outMin;
	pid->kp = kp;
	pid->ki = ki;
	pid->outPut = outPut;
}
/*=====[Prototypes (declarations) of task ]=============*/
void setingSetPiontPIDTask(void * taskParmPtr){
	uint16_t porcent;
	while(TRUE){

		if (xQueueReceive(pidSetPointQueue, &porcent,portMAX_DELAY) == pdTRUE)	{
			switch(porcent){
			case 25: pid.setPoint = 0.277750;
			break;
			case 50: pid.setPoint = 0.43550;
			break;
			case 75: pid.setPoint = 0.653250;
			break;
			case 100: pid.setPoint=0.87100;
			break;
			default:
				break;
			}

		}



	}

}



