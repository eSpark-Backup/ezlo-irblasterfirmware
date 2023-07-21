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

void app_main(void)
{
      Init(); // initialize the cc1101
}
