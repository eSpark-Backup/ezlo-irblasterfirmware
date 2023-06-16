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

// receivedData_t received;
ELECHOUSE_CC1101 cc1101;
static const char *TAG = "UART";
RCSwitch mySwitch1 = RCSwitch();
bool p_mode=false;
unsigned long a=0,b=0,c=0,d=0;
int track=0;
// #define BUF_SIZE (1024)

// static void echo_task(void *arg)
// {
//     /* Configure parameters of an UART driver,
//      * communication pins and install the driver */
//     uart_config_t uart_config = {
//         .baud_rate = ECHO_UART_BAUD_RATE,
//         .data_bits = UART_DATA_8_BITS,
//         .parity    = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//         .source_clk = UART_SCLK_APB,
//     };
//     int intr_alloc_flags = 0;

// #if CONFIG_UART_ISR_IN_IRAM
//     intr_alloc_flags = ESP_INTR_FLAG_IRAM;
// #endif

//     ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
//     ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
//     ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

//     // Configure a temporary buffer for the incoming data
//     uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

//     while (1) {
//         // Read data from the UART
//         int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_RATE_MS);
//         // Write data back to the UART
//         uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) data, len);
//         if (len) {
//             if(*data=='p' && *(data+1)=='\n'){
//             data[len] = '\0';
//             ESP_LOGI(TAG, "Programming mode");
//             p_mode=true;
//             if(a>0 || b>0 || c>0 || d>0)
//             ELECHOUSE_cc1101.SetRx();
//             }
//             else if(*data=='r'){
//              a=0,b=0,c=0,d=0;
//              ESP_LOGI(TAG, "All codes have been reset");
//             }
//             else if(*data=='a'){
//                   ESP_LOGI(TAG,"the code stored in a= %lu",a);
//                   if(p_mode)
//                   track=1;
//                   else
//                   cc1101.Transmitting(a);
//             }
//             else if(*data=='b'){
//                   ESP_LOGI(TAG,"the code stored in b= %lu",b);
//                   if(p_mode)
//                   track=2;
//                   else
//                   cc1101.Transmitting(b);
//             }
//             else if(*data=='c'){
//                   ESP_LOGI(TAG,"the code stored in c= %lu",c);
//                   if(p_mode)
//                   track=3;
//                   else
//                   cc1101.Transmitting(c);
//             }
//             else if(*data=='d'){
//                   ESP_LOGI(TAG,"the code stored in d= %lu",d);
//                   if(p_mode)
//                   track=4;
//                   else
//                   cc1101.Transmitting(d);
//             }
//             else{
//                   if(p_mode){
//                         ESP_LOGI(TAG,"Exited Programming Mode");
//                         p_mode=false;
//                   }
//                   else{
//                         data[len] = '\0';
//                         ESP_LOGI(TAG, "Recv str: %s", (char *) data);
//                   }
           
//             }
//         }
//     }
// }

// static void getData(void *arg){
    
//   while(1){
//    if (mySwitch1.available()) {
//       if(p_mode){
//             if(track==1){
//                   a=mySwitch1.getReceivedValue();
//                   printf("value stored in a\n");
//             }
//             if(track==2){
//                   b=mySwitch1.getReceivedValue(); 
//                   printf("Value Stored in b\n");
//             }
//             else if(track==3){
//                   c=mySwitch1.getReceivedValue();
//                   printf("Value Stored in c\n");
//             }
//             else if(track==4){
//                   d=mySwitch1.getReceivedValue(); 
//                   printf("Value Stored in d\n");
//                   p_mode=false;
//                   printf("Exited Programming Mode\n");
//             }

//             }
//             printf("Received value:%lu\n",mySwitch1.getReceivedValue());
 
//      printf("Received value Bit Length:%u\n",mySwitch1.getReceivedBitlength());
//      mySwitch1.resetAvailable();
//       }
//      vTaskDelay(10/portTICK_PERIOD_MS);
//       }
    
//   }

extern "C" void app_main(void)
{
float start_freq = 315;    //start scan frequency set.
float stop_freq = 915;    //stop scan frequency set.
int rssi_threshold = -50; // set the threshold to react. the value should be between -20 and -90. recommended -75.
float freq = start_freq; //nun
long compare_freq=0;
float mark_freq=0;
int rssi;
int mark_rssi=-100;
bool condition=true;

 ELECHOUSE_cc1101.Init();
ELECHOUSE_cc1101.setRxBW(58);
 ELECHOUSE_cc1101.SetRx();
ESP_LOGI(TAG,"Frequency scanner Ready");


while(condition){
ELECHOUSE_cc1101.setMHZ(freq);
rssi = ELECHOUSE_cc1101.getRssi();
if (rssi>rssi_threshold){
if (rssi > mark_rssi){
mark_rssi = rssi;  
mark_freq = freq;
}
}
// vTaskDelay(10/portTICK_PERIOD_MS);
freq+=0.1;

if (freq>stop_freq){
freq=start_freq;

if (mark_rssi>rssi_threshold){

long fr = mark_freq*100;

if (fr == compare_freq){
ESP_LOGI(TAG,"Freq: %f ",mark_freq);
ESP_LOGI(TAG,"Rssi: %d ",mark_rssi);
mark_rssi=-100;
compare_freq = 0;
mark_freq = 0;
// condition=false;
freq=start_freq;
}else{
compare_freq = mark_freq*100;
freq = mark_freq -0.10;
mark_freq=0;
mark_rssi=-100;
}
}
}
}
}
  

