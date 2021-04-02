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
 ** @brief 	Lectura de los canales correspondientes al ADC CH1 y CH2
 **
 **| REV | YYYY.MM.DD | Autor               | Descripción de los cambios                              |
 **|-----|------------|---------------------|---------------------------------------------------------|
 **|   1 | 2020.08.13 | luis Mariano Campos | Version inicial 									      |
 **|     |            |                     | 				 									      |
 ** @addtogroup aplicacion principal
 ** @{ */

/*==================[inlcusiones]============================================*/
#include "adc.h"
#include "pid.h"

void adcStart() {
	adcConfig(ADC_ENABLE); /* ADC */
}

void adcScanChannelTask(void * taskParmPtr) {
	portTickType xPeriodicity = 10 / portTICK_RATE_MS;// cada 10mseg escanea los canales 1 y 2 del ADC
	portTickType xLastWakeTime = xTaskGetTickCount();

	uint16_t executePid = 0;

	uint16_t channel2 = 0;
	uint16_t index = 0;

	servo.flagCompA = TRUE;
	servo.flagCompB = TRUE;

      //channel 1 --> Sensor de presión
	  //channel 2 --> Potenciometro del EJE
	while (TRUE) {
		// pregunto si se cumplio un segundo para ejecutar el PID
		executePid++;
		if(executePid >= UNSEGUNDO){
			for(index=0; index<FILTRO_PROMEDIO_MOVIL; index++){
				pid.sensorPresion += adcRead(CH1);
			}
			pid.sensorPresion = pid.sensorPresion/FILTRO_PROMEDIO_MOVIL;
			pid.inPut = pid.sensorPresion * 0.00322;// convierto a tensión y multiplico por 100
			pid.sensorPresion = 0;

			pid.error= pid.setPoint - pid.inPut;
			if((pid.error <= 0.00322) && (pid.error >= -0.00322)) pid.error = 0.00000;
			//printf("Set-Point:%.5f\n",pid.setPoint);


			pid.iTerm +=(pid.ki*pid.error);
			if (pid.iTerm > pid.outMax)pid.iTerm = pid.outMax;
			if (pid.iTerm < pid.outMin)pid.iTerm = pid.outMin;
            //pid.iTerm=0.0000;
			pid.outPut =  pid.kp * pid.error + pid.iTerm;

			if (pid.outPut > pid.outMax)pid.outPut = pid.outMax;
			if (pid.outPut < pid.outMin)pid.outPut = pid.outMin;
			index=0;

			//taskENTER_CRITICAL();
				printf("Error:%.5f\n",pid.error);
				printf("Salida OutPut:%.5f\n", pid.outPut);
				printf("Potenciometro del eje:%.5f\n", servo.potentiometer);
				printf("Sensor de presion:%.5f\n", pid.inPut);
				//fflush(stdout);
			//taskEXIT_CRITICAL();

			executePid = 0;

		}


		channel2 = channel2 + adcRead(CH2);//potenciometro del eje
		index++;
		if (index == FILTRO_PROMEDIO_MOVIL) {// siendo el Filtro_promedio Movil =20, entonces cada 2 segundos tengo un valor de Voltaje, presión y altura
			channel2 = channel2 / FILTRO_PROMEDIO_MOVIL;

			servo.potentiometer = channel2 *0.00322;//convierto a tensión
			           //
			if(pid.outPut -2*0.00322> servo.potentiometer){//Abre la valvula
				if(servo.flagCompA == TRUE){
					//detengo al motor
					servo.flagPulse=FALSE;
					xQueueSend(stepperMotorPulseServoQueue, &servo.flagPulse,
												portMAX_DELAY);
					//establezco el sentido de giro
					servo.direction = STEPPER_LEFT_CLOSE;
					xQueueSend(stepperMotorDirectionQueue, &servo.direction,portMAX_DELAY);
					//habilito la generación de pulsos
					servo.flagPulse = TRUE;
					xQueueSend(stepperMotorPulseServoQueue, &servo.flagPulse,
																	portMAX_DELAY);
					servo.flagCompA = FALSE;
					servo.flagCompB = TRUE;
				}

			}else{     //
				if(pid.outPut+2*0.00322<servo.potentiometer){ //Cierra la valvula
					if(servo.flagCompB == TRUE){
						//detengo al motor
							servo.flagPulse=FALSE;
							xQueueSend(stepperMotorPulseServoQueue, &servo.flagPulse,
														portMAX_DELAY);
							//establezco el sentido de giro
							servo.direction = STEPPER_RIGHT_OPEN;
							xQueueSend(stepperMotorDirectionQueue, &servo.direction,portMAX_DELAY);
							//habilito la generación de pulsos
							servo.flagPulse=TRUE;
							xQueueSend(stepperMotorPulseServoQueue, &servo.flagPulse,
																			portMAX_DELAY);
							servo.flagCompA = TRUE;
							servo.flagCompB = FALSE;
					}

				}
				else{ // mantiene en ete valor o posición a la valvula
					//if((servo.setPoint<= servo.potentiometer+5)&&(servo.setPoint>= servo.potentiometer-5) ){
						servo.flagPulse=FALSE;
					    xQueueSend(stepperMotorPulseServoQueue, &servo.flagPulse,
							portMAX_DELAY);
					    servo.flagCompA = TRUE;
						servo.flagCompB = TRUE;

					//}

				}

			}

			channel2 = 0;
			index = 0;
		}
		vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
	}
}

char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) {
		*result = '\0';
		return result;
	}

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ =
				"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35
						+ (tmp_value - value * base)];
	} while (value);

	// Apply negative sign
	if (tmp_value < 0)
		*ptr++ = '-';
	*ptr-- = '\0';
	while (ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}
