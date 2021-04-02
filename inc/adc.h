/*=====[Module Name]===========================================================
 * Copyright 2020 Luis Mariano Campos <lmarianocampos@gmail.com>
 * All rights reserved.
 * License: license text or at least name and link
         (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 1.0.0
 * Creation Date: 2020/08/13
 */

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef _ADC_H_
#define _ADC_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"
#include "bibliotecasfreeRTOS.h"
#include "steppermotor.h"
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/
#define FILTRO_PROMEDIO_MOVIL   10
#define UNSEGUNDO 50
/*=====[Public function-like macros]=========================================*/
/*=====[Definitions of public data types]====================================*/

typedef struct{
	uint16_t setPoint;
	float potentiometer;
	bool_t flagPulse;
	stepperMotorDirection_t direction;
	bool_t flagCompA;
	bool_t flagCompB;
}servo_t;
servo_t servo;

/*=====[Prototypes (declarations) of public functions]=======================*/
void adcStart();
char* itoa(int value, char* result, int base);
//task
void adcScanChannelTask(void * taskParmPtr);
/*=====[Prototypes (declarations) of public interrupt functions]=============*/



/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _ADC_H_ */
