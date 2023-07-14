#include <stdio.h>
#include <RCSwitch.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include <string.h>
#include "ezlopi_spi_master.h"
#include "ezlopi_uart.h"

Variables var = {
  .nReceivedValue = 0,
  .nReceivedValue = 0,
  .nReceivedBitlength = 0,
  .nReceivedDelay = 0,
  .nReceivedProtocol = 0,
  .nReceiveTolerance = 60,
  .nSeparationLimit = 4300,
};

extern bool wFlag;

void app_main(void)
{
      RCSwitchInit(&var);
       Init(); // initialize the cc1101
      cc1101_data data={
            .freq=433.92,
            .event=freq_sweep,
            .code=12330472
      };
      API_function(&data,&var); 
      while(wFlag!=false);
      data.event=set_button;
      API_function(&data,&var);
      data.event=transmitting;
      vTaskDelay(5000/portTICK_PERIOD_MS);
      API_function(&data,&var);
}
