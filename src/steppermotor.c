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
/** @file 	steppermotor.c
 ** @brief 	Configurción del puerto serie USB y rs232
 **
 **| REV | YYYY.MM.DD | Autor               | Descripción de los cambios                              |
 **|-----|------------|---------------------|---------------------------------------------------------|
 **|   1 | 2020.07.1 | luis Mariano Campos | Version inicial 									      |
 **|     |            |                     | 				 									      |
 ** @addtogroup aplicacion principal
 ** @{ */

/*==================[inlcusiones]============================================*/
#include"steppermotor.h"
/*************************Creación de colas*****************************************/
void stepperMotorQueueCreate(void) {
	/*******COLA - Habilita Deshabilita el Motor***********/
	stepperMotorEnableQueue = xQueueCreate(SIZEQUEUEMOTORENABLE,
			sizeof(stepperMotorEnable_t));

	if (stepperMotorEnableQueue == NULL) {/*Si devolvio NULL es muy probable que no haya suficiente memoria para crear la cola*/
		gpioWrite(LED2, ON);
		while (1)
			;/*Se queda bloquedo el sistema hasta que venga el personal de mantenimiento*/
	}
	/*******COLA - Establece los Micropasos del Motor***********/
	stepperMotorMicroStepsQueue = xQueueCreate(SIZEQUEUEMICROSTEPS,
			sizeof(stepperMotorMicroSteps_t));

	if (stepperMotorMicroStepsQueue == NULL) {/*Si devolvio NULL es muy probable que no haya suficiente memoria para crear la cola*/
		gpioWrite(LED2, ON);
		while (1)
			;/*Se queda bloquedo el sistema hasta que venga el personal de mantenimiento*/
	}
	/*******COLA - Establece el sentido de Giro del eje del Motor***********/
	stepperMotorDirectionQueue = xQueueCreate(SIZEQUEUEDIRECTION,
			sizeof(stepperMotorDirection_t));

	if (stepperMotorDirectionQueue == NULL) {/*Si devolvio NULL es muy probable que no haya suficiente memoria para crear la cola*/
		gpioWrite(LED2, ON);
		while (1)
			;/*Se queda bloquedo el sistema hasta que venga el personal de mantenimiento*/
	}
	/*******COLA - Numero de Pulsos***********/
		stepperMotorPulseQueue = xQueueCreate(SIZEQUEUEPULSE,
				sizeof(uint16_t));

		if (stepperMotorPulseQueue == NULL) {/*Si devolvio NULL es muy probable que no haya suficiente memoria para crear la cola*/
			gpioWrite(LED2, ON);
			while (1)
				;/*Se queda bloquedo el sistema hasta que venga el personal de mantenimiento*/
		}
		/***************COLA - FlagPulse*******/
		 stepperMotorPulseServoQueue = xQueueCreate(SIZEQUEUEPULSESERVO,
						sizeof(bool_t));
		   if (stepperMotorPulseServoQueue == NULL) {/*Si devolvio NULL es muy probable que no haya suficiente memoria para crear la cola*/
		   			gpioWrite(LED2, ON);
		   			while (1)
		   				;/*Se queda bloquedo el sistema hasta que venga el personal de mantenimiento*/
		   		}

}




/****************************Declaración de Temporizador del Kernel FreeRTOS******************************************************/
void stepperMotorTimerInit(){
	xAutoReloadTimer=xTimerCreate("AutoReload", AUTO_RELOAD_TIMER_PERIODO,pdTRUE,0,stepperMotorAutoReloadTimer);
	if (xAutoReloadTimer== NULL) {/*Si devolvio NULL es porqe no se creo el temporizador*/
				gpioWrite(LED2, ON);
				while (1)
					;/*Se queda bloquedo el sistema hasta que venga el personal de mantenimiento*/
			}
	xAutoReloadTimerServo = xTimerCreate("AutoReloadServo", AUTO_RELOAD_TIMER_PERIODO_SERVO,pdTRUE,0,stepperMotorAutoReloadTimerServo);
		if (xAutoReloadTimerServo == NULL) {/*Si devolvio NULL es porqe no se creo el temporizador*/
					gpioWrite(LED2, ON);
					while (1)
						;/*Se queda bloquedo el sistema hasta que venga el personal de mantenimiento*/
				}

}
/**************Funciones correspondientes a la API's del Motor*********************/
void stepperMotorInit(stepperMotor_t *stepper, uint32_t stepsPerRevolution,
/*		GPIO2				GPIO1	 ,	          GPIO0   ,         	GPIO3	*/
gpioMap_t pulsePin, gpioMap_t directionPin, gpioMap_t enablePin,
		gpioMap_t microStepsM0Pin,
		/*                  GPIO4        ,		GPIO5				*/
		gpioMap_t microStepsM1Pin, gpioMap_t microStepsM2Pin, float stepAngle) {

	stepper->stepsPerRevolution = stepsPerRevolution;
	stepper->pulsePin = pulsePin;
	stepper->directionPin = directionPin;
	stepper->enablePin = enablePin;
	stepper->microStepsM0Pin = microStepsM0Pin;
	stepper->microStepsM1Pin = microStepsM1Pin;
	stepper->microStepsM2Pin = microStepsM2Pin;
	stepper->stepAngle = stepAngle;

	// configuro el sentidos de los GPIO'S
	gpioConfig(stepper->pulsePin, GPIO_OUTPUT);
	gpioConfig(stepper->directionPin, GPIO_OUTPUT);
	gpioConfig(stepper->enablePin, GPIO_OUTPUT);
	gpioConfig(stepper->microStepsM0Pin, GPIO_OUTPUT);
	gpioConfig(stepper->microStepsM1Pin, GPIO_OUTPUT);
	gpioConfig(stepper->microStepsM2Pin, GPIO_OUTPUT);
}

void stepperMotorEnableTask(void * taskParmPtr) {
	stepperMotorEnable_t ena;
	while (TRUE) {
		if (xQueueReceive(stepperMotorEnableQueue, &ena,
		portMAX_DELAY) == pdTRUE)
			switch (ena) {
			case STEPPER_ENABLE:
				gpioWrite(stepper.enablePin, TRUE); //activo en bajo, en el hardware implemente una compuerta NOT
				stepper.isEnable = ena;
				printf("Motor Habilitado\n");
				break;
			case STEPPER_DISABLE:
				gpioWrite(stepper.enablePin, FALSE);
				stepper.isEnable = ena;
				printf("Motor Deshabilitado\n");
			}
	}
}
void stepperMotorMicroStepsTask(void * taskParmPtr) {
	stepperMotorMicroSteps_t mSteps;
	while (TRUE) {
		if (xQueueReceive(stepperMotorMicroStepsQueue, &mSteps,
		portMAX_DELAY) == pdTRUE) {
			//controlar que el eje del motor no este girando para poder establecer un microsteps....
			switch (mSteps) {
			case RESOLUTION_FULL_STEP:
				stepperMotorSetMicroSteps( FALSE, FALSE, FALSE);
				printf("FULL_STEP\n");
				break;
			case RESOLUTION_HALF_STEP:
				stepperMotorSetMicroSteps(TRUE, FALSE, FALSE);
				printf("HALF_STEP\n");
				break;
			case RESOLUTION_4_STEP:
				stepperMotorSetMicroSteps(FALSE, TRUE, FALSE);
				printf("4_STEP\n");
				break;
			case RESOLUTION_8_STEP:
				stepperMotorSetMicroSteps(TRUE, TRUE, FALSE);
				printf("8_STEP\n");
				break;
			case RESOLUTION_16_STEP:
				stepperMotorSetMicroSteps(FALSE, FALSE, TRUE);
				printf("16_STEP\n");
				break;
			case RESOLUTION_32_STEP:
				stepperMotorSetMicroSteps(TRUE, TRUE, TRUE);
				printf("32_STEP\n");
				break;

			}

		}
	}
}
void stepperMotorSetDirectionTask(void * taskParmPtr){
	stepperMotorDirection_t directionMotor;
	while(TRUE){
	if(xQueueReceive(stepperMotorDirectionQueue, &directionMotor,
			portMAX_DELAY)==pdTRUE){
		switch (directionMotor) {
			case STEPPER_RIGHT_OPEN:
				gpioWrite(stepper.directionPin, TRUE);
				stepper.direction = directionMotor;
				printf("Sentido Horario\n");
				break;
			case STEPPER_LEFT_CLOSE:
				gpioWrite(stepper.directionPin, FALSE);
				stepper.direction = directionMotor;
				printf("Sentido AntiHorario\n");
				break;
			}
	}
	}

}
void stepperMotorSetPulseTask(void * taskParmPtr) {
	uint16_t nOfSteps =0;
	while (TRUE) {
		if (xQueueReceive(stepperMotorPulseQueue, &nOfSteps,
				portMAX_DELAY)== pdTRUE) {
			//aca debo controlar el estado del motor, que el eje no se encuentre girando y el motor habilitado.
			//si no se encuentra en ese estado descarta este  comando
			printf("numero de pasos:%d\n", nOfSteps);
			stepper.lastNumberOfSteps = nOfSteps;
			numOfStepes = stepper.lastNumberOfSteps;
			gpioWrite(stepper.pulsePin, OFF);
			if (xTimerStart(xAutoReloadTimer,0) == pdFALSE) {
				printf(
						"No se pudo escribir el comando en la cola del temporizador\n");
			}
		}
	}
}

static void stepperMotorAutoReloadTimer(TimerHandle_t xTimer){

	static bool_t flag=TRUE;

	//gpioToggle( LED1 );
	if(numOfStepes>0){
		if(flag==TRUE){
			gpioWrite(stepper.pulsePin,ON);
			flag=FALSE;

		}
		else{
			gpioWrite(stepper.pulsePin,OFF);
			flag=TRUE;
			numOfStepes--;
		}

	}
	else {
		xTimerStop(xTimer,0);
		gpioWrite( LED1,OFF );
	}


}
void stepperMotorSetPulseServoTask(void * taskParmPtr){
	bool_t flagStep;
	while(TRUE){
			if (xQueueReceive(stepperMotorPulseServoQueue, &flagStep,
							portMAX_DELAY)== pdTRUE) {
					stepper.flagPulse = flagStep;
					gpioWrite(stepper.pulsePin, OFF);
					if (xTimerStart(xAutoReloadTimerServo,0) == pdFALSE) {
									printf(
											"No se pudo escribir el comando en la cola del temporizador\n");
								}
		}
	}

}

static void stepperMotorAutoReloadTimerServo(TimerHandle_t xTimer) {
	static bool_t flagDutyCycle = TRUE;

	if (stepper.flagPulse == TRUE) {
		if (flagDutyCycle == TRUE) {
			gpioWrite(stepper.pulsePin, ON);
			flagDutyCycle = FALSE;
		} else {
			gpioWrite(stepper.pulsePin, OFF);
			flagDutyCycle = TRUE;
		}

	}else
	{
		xTimerStop(xTimer,0);
	}

}


void stepperMotorSetMicroSteps(bool_t m0MicroStep, bool_t m1MicroStep,
		bool_t m2MicroStep) {
	//controlar que el eje del motor no este girando para poder establecer un microsteps....
	//podemos preguntar si pulseCount=0
	gpioWrite(stepper.microStepsM0Pin, m0MicroStep);
	gpioWrite(stepper.microStepsM1Pin, m1MicroStep);
	gpioWrite(stepper.microStepsM2Pin, m2MicroStep);
}
