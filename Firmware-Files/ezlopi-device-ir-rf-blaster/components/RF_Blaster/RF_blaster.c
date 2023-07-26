#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include "driver/gpio.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>
#include "driver/spi_slave.h"
#include <string.h>
#include <driver/spi_master.h>
#include "esp_log.h"
#include "esp_check.h"
#include "RCswitch.h"
#include "ezlopi_spi_master.h"
#include "ezlopi_uart.h"
#include "cJSON.h"
#include "RF_blaster.h"

const char* TAG="RF_blaster";

void RF_Generic_API(void *data)
{
  ESP_LOGI(TAG, "API FUNCTION");
  cc1101_data *pData = (cc1101_data *)data;

  if (pData->event == SET_BUTTON)
  {
    setButton(pData);
  }
  else if (pData->event == FREQ_SWEEP)
  {
    pData->freq = sweepFrequency();
  }
  else if (pData->event == USER_INPUT)
  {
    setMHZ(pData->freq);
    ESP_LOGI(TAG, "Frequency set to %f MHz", pData->freq);
  }
  else if (pData->event == TRANSMITTING)
  {
    Transmitting(pData);
    ESP_LOGI(TAG, "Transmitted");
  }
  setSidle();
}

