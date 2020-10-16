/*=====[Module Name]===========================================================
 * Copyright 2020 Luis Mariano Campos <lmarianocampos@gmail.com>
 * All rights reserved.
 * License: license text or at least name and link
 (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 1.0.0
 * Creation Date: 2020/10/06
 */

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef _PID_H_
#define _PID_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"
#include "bibliotecasfreeRTOS.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/
//#define FILTRO_PROMEDIO_MOVIL   5
#define SIZEQUEUEPIDSETPOINT 3
/*=====[Public function-like macros]=========================================*/
/*=====[Definitions of public data types]====================================*/
/*****Decaración de Colas************/
QueueHandle_t pidSetPointQueue;

typedef struct {

	uint16_t sPoint;
	uint16_t sensorPresion;

	float inPut;

	float error;
	float iTerm;
	float outMax;
	float outMin;
	float kp;
	float ki;

	float outPut;
	float setPoint;

} Pid_t;

Pid_t pid;

double setPoint;
/*=====[Prototypes (declarations) of public functions]=======================*/
void startPid(Pid_t *pid,uint16_t sPoint, uint16_t sensorPresion, float setPoint,
		float inPut, float error, float iTerm, float outMax, float outMin,
		float kp, float ki, float outPut);
void PdiCreateQueue(void);
/*********************TASK*****************************/
void setingSetPiontPIDTask(void * taskParmPtr);

//task
/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _ADC_H_ */
