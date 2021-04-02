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
/** @file 	main.c
 ** @brief 	Trabajo Final de Carrerar RTOSII
 **
 **| REV | YYYY.MM.DD | Autor               | Descripción de los cambios                              |
 **|-----|------------|---------------------|---------------------------------------------------------|
 **|   1 | 2020.06.29 | luis Mariano Campos | Version inicial 									      |
 **|     |            |                     | 				 									      |
 ** @addtogroup aplicacion principal
 ** @{ */

/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS

#include "serialport.h"
#include "heartbeat.h"
#include "commandprocessing.h"
#include "sapi.h"
#include "steppermotor.h"
#include "adc.h"
#include "pid.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/


/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/



/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma
   boardConfig();
   serialPortForInterruptInit();
   // inicializo el motor PaP
   stepperMotorInit(&stepper,400,GPIO2,GPIO1,GPIO0,GPIO3,GPIO4,GPIO5,0.9 );
   startPid(&pid,0, 0,0.00000, 0.00000,0.00000,0.00000,1.20000,0.00000,1.000, 0.05555, 0.00000);
   adcStart();
   printf( "Trabajo Final con freeRTOS y sAPI.\n" );

   serialPortQueueCreate();
   stepperMotorQueueCreate();
   commandProcessingQueueCreate();
   PdiCreateQueue();
   stepperMotorTimerInit();
   // Crear tarea en freeRTOS
   xTaskCreate(
      heartBeat,                     // Funcion de la tarea a ejecutar
      (const char *)"heartbeat",     // Nombre de la tarea como String amigable para el usuario
      configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
      0,                          // Parametros de tarea
      tskIDLE_PRIORITY+1,         // Prioridad de la tarea
      0                           // Puntero a la tarea creada en el sistema
   );
   xTaskCreate(
		   serialPortTaskStorageCommand,                     // Funcion de la tarea a ejecutar
         (const char *)"Tarea que almacena el comando",     // Nombre de la tarea como String amigable para el usuario
         configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
         0,                          // Parametros de tarea
         tskIDLE_PRIORITY+3,         // Prioridad de la tarea
         0                           // Puntero a la tarea creada en el sistema
      );
   xTaskCreate(
		   commandProcessingTask,                     // Funcion de la tarea a ejecutar
            (const char *)"Tarea que procesa el comando recibido",     // Nombre de la tarea como String amigable para el usuario
            configMINIMAL_STACK_SIZE*6, // Cantidad de stack de la tarea
            0,                          // Parametros de tarea
            tskIDLE_PRIORITY+3,         // Prioridad de la tarea
            0                           // Puntero a la tarea creada en el sistema
         );

   xTaskCreate(
		   stepperMotorEnableTask,                     // Funcion de la tarea a ejecutar
               (const char *)"Tarea, Habilita o Deshabilita el Motor",     // Nombre de la tarea como String amigable para el usuario
               configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
               0,                          // Parametros de tarea
               tskIDLE_PRIORITY+3,         // Prioridad de la tarea
               0                           // Puntero a la tarea creada en el sistema
            );

   xTaskCreate(
		   stepperMotorMicroStepsTask,                     // Funcion de la tarea a ejecutar
                  (const char *)"Tarea, establece los micropasos",     // Nombre de la tarea como String amigable para el usuario
                  configMINIMAL_STACK_SIZE*3, // Cantidad de stack de la tarea
                  0,                          // Parametros de tarea
                  tskIDLE_PRIORITY+3,         // Prioridad de la tarea
                  0                           // Puntero a la tarea creada en el sistema
               );
   xTaskCreate(
		   stepperMotorSetDirectionTask,                     // Funcion de la tarea a ejecutar
                  (const char *)"Tarea, establece el sentido de giro del eje del Motor",     // Nombre de la tarea como String amigable para el usuario
                  configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
                  0,                          // Parametros de tarea
                  tskIDLE_PRIORITY+3,         // Prioridad de la tarea
                  0                           // Puntero a la tarea creada en el sistema
               );
   xTaskCreate(
		   stepperMotorSetPulseTask,                     // Funcion de la tarea a ejecutar
                     (const char *)"Tarea, establece la cantidad de pulsos",     // Nombre de la tarea como String amigable para el usuario
                     configMINIMAL_STACK_SIZE*3, // Cantidad de stack de la tarea
                     0,                          // Parametros de tarea
                     tskIDLE_PRIORITY+4,         // Prioridad de la tarea
                     0                           // Puntero a la tarea creada en el sistema
                  );

   xTaskCreate(
		   stepperMotorSetPulseServoTask,                     // Funcion de la tarea a ejecutar
                        (const char *)"Tarea, hace trabajar al motor PaP como Servo",     // Nombre de la tarea como String amigable para el usuario
                        configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
                        0,                          // Parametros de tarea
                        tskIDLE_PRIORITY+4,         // Prioridad de la tarea
                        0                           // Puntero a la tarea creada en el sistema
                     );

   xTaskCreate(
		   setingSetPiontPIDTask,                     // Funcion de la tarea a ejecutar
                           (const char *)"Tarea,establece el set-point",     // Nombre de la tarea como String amigable para el usuario
                           configMINIMAL_STACK_SIZE*4, // Cantidad de stack de la tarea
                           0,                          // Parametros de tarea
                           tskIDLE_PRIORITY+4,         // Prioridad de la tarea
                           0                           // Puntero a la tarea creada en el sistema
                        );



   xTaskCreate(
		   adcScanChannelTask,                     // Funcion de la tarea a ejecutar
                        (const char *)"Tarea, escanea los canales CH1 y CH2 del ADC",     // Nombre de la tarea como String amigable para el usuario
                        configMINIMAL_STACK_SIZE*4, // Cantidad de stack de la tarea
                        0,                          // Parametros de tarea
                        tskIDLE_PRIORITY+1,         // Prioridad de la tarea
                        0                           // Puntero a la tarea creada en el sistema
                     );


   //Iniciar scheduler
   vTaskStartScheduler();

   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE ) {
      // Si cae en este while 1 significa que no pudo iniciar el scheduler
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/


/*==================[fin del archivo]========================================*/
