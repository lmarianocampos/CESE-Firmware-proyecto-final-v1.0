#include "heartbeat.h"
#include "bibliotecasfreeRTOS.h"


// Implementacion de funcion de la tarea heartbeat
void heartBeat( void* taskParmPtr )
{
   // ---------- CONFIGURACIONES ------------------------------
   // Tarea periodica cada 500 ms
   portTickType xPeriodicity =  500 / portTICK_RATE_MS;
   portTickType xLastWakeTime = xTaskGetTickCount();

   // ---------- REPETIR POR SIEMPRE --------------------------
   while(TRUE) {
      gpioToggle( LEDB );

      // Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
      vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
   }
}
