/*=====[Module Name]===========================================================
 * Copyright 2020 Luis Mariano Campos <lmarianocampos@gmail.com>
 * All rights reserved.
 * License: license text or at least name and link
         (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 1.0.0
 * Creation Date: 2020/07/1
 */

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef _STEPPERMOTOR_H_
#define _STEPPERMOTOR_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"
#include "bibliotecasfreeRTOS.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/
#define SIZEQUEUEMOTORENABLE 3
#define SIZEQUEUEMICROSTEPS  3
#define SIZEQUEUEDIRECTION   3
#define SIZEQUEUEPULSE   3
#define SIZEQUEUEPULSESERVO 5
#define AUTO_RELOAD_TIMER_PERIODO pdMS_TO_TICKS(100)//T=2*10=20mseg
#define AUTO_RELOAD_TIMER_PERIODO_SERVO pdMS_TO_TICKS(10)//T=2*100 = 200mseg
/*=====[Public function-like macros]=========================================*/
/*=====[Definitions of public data types]====================================*/
typedef enum{
	STEPPER_RIGHT_OPEN, STEPPER_LEFT_CLOSE
}stepperMotorDirection_t;

typedef enum {
	STEPPER_DISABLE, STEPPER_ENABLE
} stepperMotorEnable_t;

typedef enum {
	STEPPER_AXIS_NO_MOVE, STEPPER_AXIS_LAST_MOVE, STEPPER_AXIS_IN_MOVE
} stepperMotorMove_t;
typedef enum {
	RESOLUTION_FULL_STEP,
	RESOLUTION_HALF_STEP,
	RESOLUTION_4_STEP,
	RESOLUTION_8_STEP,
	RESOLUTION_16_STEP,
	RESOLUTION_32_STEP
} stepperMotorMicroSteps_t;

typedef struct{
		uint32_t stepsPerRevolution;
		uint16_t lastNumberOfSteps;
		float LastAnglePosition;

		gpioMap_t pulsePin;
		gpioMap_t directionPin;
		gpioMap_t enablePin;

		gpioMap_t microStepsM0Pin;
		gpioMap_t microStepsM1Pin;
		gpioMap_t microStepsM2Pin;

		stepperMotorDirection_t direction;

		stepperMotorEnable_t isEnable;
		stepperMotorMove_t isMoveAxis;

		bool_t flagPulse;

		float rpm;
		float stepAngle;


}stepperMotor_t;
stepperMotor_t  stepper;
uint16_t numOfStepes;
bool_t flagStep;
/*****Declaración de Colas**********/

QueueHandle_t stepperMotorEnableQueue;
QueueHandle_t stepperMotorMicroStepsQueue;
QueueHandle_t stepperMotorDirectionQueue;
QueueHandle_t stepperMotorPulseQueue;
QueueHandle_t stepperMotorPulseServoQueue;
/*******Declaración del Temporizador de Software del Kernel FreeRTOS***********************/
TimerHandle_t xAutoReloadTimer;
TimerHandle_t xAutoReloadTimerServo;


/*=====[Prototypes (declarations) of public functions]=======================*/
// esta función inicializa la estructura del motor paso a paso
void stepperMotorInit(stepperMotor_t *stepper, uint32_t stepsPerRevolution,
		gpioMap_t pulsePin, gpioMap_t directionPin, gpioMap_t enablePin,
		gpioMap_t microStepsM0Pin, gpioMap_t microStepsM1Pin,
		gpioMap_t microStepsM2Pin, float stepAngle);
void stepperMotorTimerInit(void);
//task
//void serialPortTaskStorageCommand(void * taskParmPtr);
//esta función crea colas para comunicar tareas correspondientes a la API del Motor PaP
void stepperMotorQueueCreate(void);
//esta función  establece la velocidad del eje del motro PaP
void stepperMotorSetSpeed(stepperMotor_t * stepper, float rpm);

//esta función permite leer la velocidad del eje del motor PaP
float stepperMotorGetSpeed(stepperMotor_t * stepper);

//esta función establece la resolución de los microspasos
void stepperMotorSetMicroSteps( bool_t m0MicroStep,
		bool_t m1MicroStep, bool_t m2MicroStep);

//esta función permite leer el microsteps que tiene configurado el motor.
stepperMotorMicroSteps_t stepperMotorGetMicroStepes(stepperMotor_t * stepper);

// esta función permite establecer el sentido de giro del motor PaP
void stepperMotorSetDirection(stepperMotor_t * stepper,
		stepperMotorDirection_t dir);

//esta función permite leer el sentido de giro del eje del motor PaP
stepperMotorDirection_t stepperMotorGetDirection(stepperMotor_t * stepper);

//esta función permite Habilitar el motor PaP
void stepperMotorEnable(stepperMotor_t * stepper, stepperMotorEnable_t enable);

//esta función permite leer si el motor PaP está habilitado
stepperMotorEnable_t stepperMotorGetEnable(stepperMotor_t * stepper);

//esta función permite mover el eje del motor una cierta cantidad de pasos
void stepperMotorMoveSteps(stepperMotor_t * stepper, uint32_t numberOfSteps);

//esta función permite mover el eje del motor PaP una cierta cantidad vueltas completas
void stepperMotorMoveTurns(stepperMotor_t * stepper, uint32_t numberOfTurns);

//Esta función permite girar el eje del motor un determinado ángulo
void stepperMotorMoveAngle(stepperMotor_t * stepper, float angle);

//Esta función detiene el movimiento del eje del motor
void stepperMotorStopMoveSteps(stepperMotor_t * stepper);

// Esta función permite establecer el angulo o la ultima cantidad de pulsos que se proporciono al Motor PaP
void stepperMotorSetAngle(stepperMotor_t* stepper,stepperMotorDirection_t dir );

// Esta función retorna el angulo de apertura de la valvula
float stepperMotorGetAngle(stepperMotor_t* stepper);

/*=====[Prototypes (declarations) of task ]=============*/
void stepperMotorEnableTask(void * taskParmPtr);
void stepperMotorMicroStepsTask(void * taskParmPtr);
void stepperMotorSetDirectionTask(void * taskParmPtr);
void stepperMotorSetPulseTask(void * taskParmPtr);
void stepperMotorSetPulseServoTask(void * taskParmPtr);
/*=====[Prototypes (declarations) of public interrupt functions]=============*/
static void stepperMotorAutoReloadTimer(TimerHandle_t xTimer);
static void stepperMotorAutoReloadTimerServo(TimerHandle_t xTimer);


/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _STEPPERMOTOR_H_ */
