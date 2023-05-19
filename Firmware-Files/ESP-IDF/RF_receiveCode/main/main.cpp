#include <stdio.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
int pin; // Create variable for receive pin.

RCSwitch mySwitch = RCSwitch();
extern "C" void app_main(void)
{
  pin = 9; // GDO2 pin

  ELECHOUSE_cc1101.setSpiPin(13, 11, 12, 14); // Set custom pins: (SCK, MISO, MOSI, CSN). Must be BEFORE Init()!
  ELECHOUSE_cc1101.setGDO(10, 9); // Set custom GDO pins: (GDO0, GDO2). Gdo0 (tx) and Gdo2 (rx). Must be BEFORE Init();
  ELECHOUSE_cc1101.Init(); // initialize the cc1101
  //ELECHOUSE_cc1101.setRxBW(812.50);  // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  //ELECHOUSE_cc1101.setPA(10); // set TxPower. The following settings are possible depending on the frequency band. Possible values: (-30, -20, -15, -10, -6, 0, 5, 7, 10, 11, 12). Default is max!
  ELECHOUSE_cc1101.setMHZ(433.92); // Set basic frequency. The lib calculates the frequency automatically (default: 433.92). CC1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
  
  mySwitch.enableReceive(pin);  // Receiver on interrupt 0

  ELECHOUSE_cc1101.SetRx();  // set Receive on

  // Check the CC1101 Spi connection.
  if (ELECHOUSE_cc1101.getCC1101()) { 
   printf("CC1101: Connection OK.");
  }
  else {
   printf("CC1101: Connection error.");
  }

while(1){
      if (mySwitch.available()) {
    printf("Received value:%lu\n",mySwitch.getReceivedValue());
 
    printf("Received value Bit Length:%u\n",mySwitch.getReceivedBitlength());
    mySwitch.resetAvailable();
  }
  vTaskDelay(10/portTICK_PERIOD_MS);
}
}
