#include <stdio.h>
#include <RCSwitch.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include <string.h>
#include <cstring>


#define ECHO_TEST_TXD 43
#define ECHO_TEST_RXD 44
#define ECHO_TEST_RTS -1
#define ECHO_TEST_CTS -1

#define ECHO_UART_PORT_NUM      0
#define ECHO_UART_BAUD_RATE     115200
#define ECHO_TASK_STACK_SIZE    2048
 float freq_set;
// receivedData_t received;
ELECHOUSE_CC1101 cc1101;
static const char *TAG = "CC1101";
RCSwitch mySwitch1 = RCSwitch();
bool p_mode=false;
unsigned long a=0,b=0,c=0,d=0;
int track=0;
#define BUF_SIZE (1024)

static void getData(void *arg){
       ESP_LOGI(TAG,"%f\n",freq_set);
      ESP_LOGI(TAG,"GET DATA STARTED!");
      int pin=9;
       cc1101.setMHZ(freq_set);
      mySwitch1.enableReceive(pin);  // Receiver on interrupt 0
      // cc1101.SetRx();  // set Receive on
      ESP_LOGI(TAG,"RX MODE");
      // Check the CC1101 Spi connection.
      if (cc1101.getCC1101()) { 
       ESP_LOGI(TAG,"CC1101: Connection OK.");
      }
      else {
       ESP_LOGI(TAG,"CC1101: Connection error.");
      }
  while(1){
      vTaskDelay(75/portTICK_PERIOD_MS);
   if (mySwitch1.available()) {
      if(p_mode){
            if(track==1){
                  a=mySwitch1.getReceivedValue();
                  printf("value stored in a\n");
            }
            else if(track==2){
                  b=mySwitch1.getReceivedValue(); 
                  printf("Value Stored in b\n");
            }
            else if(track==3){
                  c=mySwitch1.getReceivedValue();
                  printf("Value Stored in c\n");
            }
            else if(track==4){
                  d=mySwitch1.getReceivedValue(); 
                  printf("Value Stored in d\n");
                  // p_mode=false;
                  // printf("Exited Programming Mode\n");
            }

            }
            ESP_LOGI(TAG,"Received value:%lu\n",mySwitch1.getReceivedValue());
 
     ESP_LOGI(TAG,"Received value Bit Length:%u\n",mySwitch1.getReceivedBitlength());
     mySwitch1.resetAvailable();
      }
     vTaskDelay(10/portTICK_PERIOD_MS);
      }
    
  }



void sweepFrequencyTask(void *args){
      float start_freq = 300;    //start scan frequency set.
      float stop_freq = 450;    //stop scan frequency set.
      int rssi_threshold = -55; // set the threshold to react. the value should be between -20 and -90. recommended -75.
      float freq = start_freq; //nun
      long compare_freq=0;
      float mark_freq=0;
      int rssi;
      int mark_rssi=-100;

 //ELECHOUSE_cc1101.Init();
    // cc1101.setRxBW(58);
      cc1101.SetRx();
      ESP_LOGI(TAG,"Frequency scanner Ready");

while(1){
      cc1101.setMHZ(freq);
      rssi = cc1101.getRssi();
      if (rssi>rssi_threshold){
            if (rssi > mark_rssi)
            {
                  mark_rssi = rssi;  
                  mark_freq = freq;
            }
      }
//  vTaskDelay(10/portTICK_PERIOD_MS);
freq+=0.1;

      if (freq>stop_freq){
            freq=start_freq;

      if (mark_rssi>rssi_threshold){

            long fr = mark_freq*100;

      if (fr == compare_freq){
            ESP_LOGI(TAG,"Freq: %f ",mark_freq);
            ESP_LOGI(TAG,"Rssi: %d ",mark_rssi);
            freq_set=mark_freq;
            mark_rssi=-100;
            compare_freq = 0;
            mark_freq = 0;
            
            xTaskCreate(getData, "get_data_task", 2048*2, NULL, 12, NULL);
            freq=start_freq;
            vTaskDelete(NULL);
      }
      else{
         compare_freq = mark_freq*100;
            freq = mark_freq -0.10;
            mark_freq=0;
            mark_rssi=-100;
}
}
}
}
}
static void echo_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_RATE_MS);
        // Write data back to the UART
        uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) data, len);
        if (len) {
            if(*data=='p'){
            data[len] = '\0';
            ESP_LOGI(TAG, "Programming mode");
            p_mode=true;
            // xTaskCreate(sweepFrequencyTask, "sweep_freq_task", 2*2048, NULL, 11, NULL);
            if(a>0 || b>0 || c>0 || d>0)
            cc1101.SetRx();
            }
            else if(*data=='r'){
             a=0,b=0,c=0,d=0;
             ESP_LOGI(TAG, "All codes have been reset");
            }
            else if(*data=='a'){
                  ESP_LOGI(TAG,"the code stored in a= %lu",a);
                  if(p_mode){
                  track=1;
                  vTaskDelay(5/portTICK_PERIOD_MS);
                  }
                  else
                  cc1101.Transmitting(a);
            }
            else if(*data=='b'){
                  ESP_LOGI(TAG,"the code stored in b= %lu",b);
                  if(p_mode){
                  track=2;
                  vTaskDelay(5/portTICK_PERIOD_MS);
                  }
                  else
                  cc1101.Transmitting(b);
            }
            else if(*data=='c'){
                  ESP_LOGI(TAG,"the code stored in c= %lu",c);
                  if(p_mode){
                  track=3;
                  vTaskDelay(5/portTICK_PERIOD_MS);
                  }
                  else
                  cc1101.Transmitting(c);
            }
            else if(*data=='d'){
                  ESP_LOGI(TAG,"the code stored in d= %lu",d);
                  if(p_mode){
                  track=4;
                  vTaskDelay(5/portTICK_PERIOD_MS);
                  }
                  else
                  cc1101.Transmitting(d);
            }
            else{
                  if(p_mode){
                        ESP_LOGI(TAG,"Exited Programming Mode");
                        p_mode=false;
                  }
                  else{
                        data[len] = '\0';
                        ESP_LOGI(TAG, "Recv str: %s", (char *) data);
                  }
           
            }
        }
    }
}




extern "C" void app_main(void)
{
      //  cc1101.setup();
      // xTaskCreatePinnedToCore(sweepFrequencyTask, "sweep_freq_task", 2*2048, NULL, 11, NULL,1);
      xTaskCreatePinnedToCore(echo_task, "uart_echo_task", 2*ECHO_TASK_STACK_SIZE, NULL, 9, NULL,0);
      cc1101.Init(); // initialize the cc1101
      cc1101.setSpiPin(13, 11, 12, 14); // Set custom pins: (SCK, MISO, MOSI, CSN). Must be BEFORE Init()!
      cc1101.setGDO(10, 9); // Set custom GDO pins: (GDO0, GDO2). Gdo0 (tx) and Gdo2 (rx). Must be BEFORE Init();
      // cc1101.setMHZ(433.800598); // Set basic frequency. The lib calculates the frequency automatically (default: 433.92). CC1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
      xTaskCreatePinnedToCore(sweepFrequencyTask, "sweep_freq_task", 2*2048, NULL, 11, NULL,1);
      // xTaskCreatePinnedToCore(getData, "get_data_task", 2048*2, NULL, 9, NULL,0);
      
}
