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

#ifndef _COMMANDPROCESSING_H_
#define _COMMANDPROCESSING_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"
#include "bibliotecasfreeRTOS.h"
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/
//Tamaño de la cola que enviará puntero
#define SIZECOMMANDQUEUE 100
/*=====[Public function-like macros]=========================================*/
/*=====[Definitions of public data types]====================================*/
QueueHandle_t processingComandQueue;

/*=====[Prototypes (declarations) of public functions]=======================*/
//task
void commandProcessingQueueCreate(void);
uint16_t commandProcessingConverterCaracterToDecimal(char * pointer, uint8_t length);
//esta tarea realiza el procesamiento o identifica qué comando se recibio para lego llamar a una función de la API del motor PaP
void commandProcessingTask(void * taskParmPtr);
/*=====[Prototypes (declarations) of public interrupt functions]=============*/



/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _COMMANDPROCESSING_H_ */
