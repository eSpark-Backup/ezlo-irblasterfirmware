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

#define ECHO_TEST_TXD 43
#define ECHO_TEST_RXD 44
#define ECHO_TEST_RTS -1
#define ECHO_TEST_CTS -1

#define ECHO_UART_PORT_NUM      0
#define ECHO_UART_BAUD_RATE     115200
#define ECHO_TASK_STACK_SIZE    2048

Variables var = {
  .nReceivedValue = 0,
  .nReceivedValue = 0,
  .nReceivedBitlength = 0,
  .nReceivedDelay = 0,
  .nReceivedProtocol = 0,
  .nReceiveTolerance = 60,
  .nSeparationLimit = 4300,
  // .timings[RCSWITCH_MAX_CHANGES];
};

typedef enum events
{
    transmitting= 0,
    programming=1,
    reset=2
} cc1101_events;

typedef enum prog_modes
{
    set_freq=0,
    set_button=1
} prog_modes;

typedef enum prog_settings
{
      user_input=0,
      freq_sweep=1
}prog_settings;

typedef enum Button{
      a=0,
      b=1,
      c=2,
      d=4
}Button;

typedef struct cc1101_data
{
    float freq;
    Button button;
    prog_modes mode;
    prog_settings setting;
} cc1101_data;



unsigned long btn[4];
bool wFlag=true;
// static void echo_task_upcall(uint8_t* buffer, s_ezlopi_uart_object_handle_t uart_object_handle, void* user_args);
 float freq_set;
// receivedData_t received;
// ELECHOUSE_CC1101 cc1101;
static const char *TAG = "CC1101";
// RCSwitch mySwitch1 = RCSwitch();
bool p_mode=false;
// unsigned long a=0,b=0,c=0,d=0;
int track=0;
bool task=true;
TaskHandle_t sweepTask=NULL;
TaskHandle_t getDataTask=NULL;
esp_err_t ret=ESP_FAIL;
#define BUF_SIZE (1024)


static void getData(void *arg){
      ESP_LOGI(TAG,"GET DATA STARTED!");
      // unsigned long* btnPtr= (unsigned long*) arg;
      int pin=9;
      if(task)
      setMHZ(freq_set);
      if(ret!=ESP_OK)
      ret=enableReceive(pin,&var);  // Receiver on interrupt 0

      // cc1101.SetRx();  // set Receive on
      ESP_LOGI(TAG,"RX MODE");
      // Check the CC1101 Spi connection.
      if (getCC1101()) { 
       ESP_LOGI(TAG,"CC1101: Connection OK.");
      }
      else {
       ESP_LOGI(TAG,"CC1101: Connection error.");
      }
  while(1){
      vTaskDelay(75/portTICK_PERIOD_MS);
   if (available(&var)) 
   {

      // *btnPtr=getReceivedValue(&var);
      // if(p_mode){
      //       if(track==1){
      //             a=getReceivedValue(&var);
      //             printf("value stored in a\n");
      //       }
      //       else if(track==2){
      //             b=getReceivedValue(&var); 
      //             printf("Value Stored in b\n");
      //       }
      //       else if(track==3){
      //             c=getReceivedValue(&var);
      //             printf("Value Stored in c\n");
      //       }
      //       else if(track==4){
      //             d=getReceivedValue(&var); 
      //             printf("Value Stored in d\n");
      //             // p_mode=false;
      //             // printf("Exited Programming Mode\n");
      //       }

      //       }
            ESP_LOGI(TAG,"Received value:%lu\n",getReceivedValue(&var));
 
     ESP_LOGI(TAG,"Received value Bit Length:%u\n",getReceivedBitlength(&var));
     resetAvailable(&var);
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
      // cc1101.SetRx();
      ESP_LOGI(TAG,"Frequency scanner Ready");

while(1){
      setMHZ(freq);
      rssi = getRssi();
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
            // xTaskCreate(getData, "get_data_task", 2048*2, NULL, 12, &getDataTask);
            freq=start_freq;
            task=true;
            // cc1101.setMHZ(freq_set);
            sweepTask = NULL;
            wFlag=false;
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
static void API_function(cc1101_events event,void* data)
{
      ESP_LOGI(TAG,"API FUNCTION");
   
    cc1101_data* pData=(cc1101_data*) data;
    if(event==programming){
            if(pData->mode==set_button){
                   ESP_LOGI(TAG, "Programming mode");
                   p_mode=true;
                   wFlag=true;
                  //  SetRx();
                  if(task)
                  setMHZ(freq_set);
                  if(ret!=ESP_OK)
                  ret=enableReceive(9,&var);  // Receiver on interrupt 0

                  // cc1101.SetRx();  // set Receive on
                  ESP_LOGI(TAG,"RX MODE");
                  // Check the CC1101 Spi connection.
                  if (getCC1101()) { 
                        ESP_LOGI(TAG,"CC1101: Connection OK.");
                  }
                  else {
                  ESP_LOGI(TAG,"CC1101: Connection error.");
                  }
                   while(wFlag){
                        vTaskDelay(75/portTICK_PERIOD_MS);
                        if (available(&var)) 
                              {
                              btn[pData->button]=getReceivedValue(&var);   
                              ESP_LOGI(TAG,"Value stored in button %d is %lu",pData->button,btn[pData->button]);
                              wFlag=false;
                   }
                   }
                  //  btn[pData->button]=getReceivedValue(&var);   
                  //  ESP_LOGI(TAG,"Value stored in %lu",btn[pData->button]);
                  //  if(getDataTask!=NULL){
                  //        vTaskDelete(getDataTask);
                  //        getDataTask=NULL;
                  //        } 
                  // xTaskCreate(getData, "get_data_task", 2048*2, &btn[pData->button], 12, &getDataTask);
                  }
            else{
                  if(pData->setting==freq_sweep){
                        xTaskCreatePinnedToCore(sweepFrequencyTask, "sweep_freq_task", 2*2048, NULL, 11, &sweepTask,1);
                        }
                  else{
                        setMHZ(pData->freq);
                        task=false;
                        wFlag=false;
                        // xTaskCreate(getData, "get_data_task", 2048*2, NULL, 12, &getDataTask);
                        }
            }
      }
      else
      {
            Transmitting(btn[pData->button],&var);
      }
}



// static void echo_task_upcall(uint8_t* buffer, s_ezlopi_uart_object_handle_t uart_object_handle, void* user_args)
// {
   
//     /* Configure parameters of an UART driver,
//      * communication pins and install the driver */
// //     uart_config_t uart_config = {
// //         .baud_rate = ECHO_UART_BAUD_RATE,
// //         .data_bits = UART_DATA_8_BITS,
// //         .parity    = UART_PARITY_DISABLE,
// //         .stop_bits = UART_STOP_BITS_1,
// //         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
// //         .source_clk = UART_SCLK_APB,
// //     };
// //     int intr_alloc_flags = 0;

// // #if CONFIG_UART_ISR_IN_IRAM
// //     intr_alloc_flags = ESP_INTR_FLAG_IRAM;
// // #endif

// //     ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
// //     ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
// //     ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

// //     // Configure a temporary buffer for the incoming data
// //     uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

// //         // Read data from the UART
// //         int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_RATE_MS);
// //         // Write data back to the UART
// //         uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) data, len);
// //         if (len) {
//             if(*buffer=='p'){
//                    ESP_LOGI(TAG, "Programming mode");
//                    p_mode=true;
//                    // xTaskCreate(sweepFrequencyTask, "sweep_freq_task", 2*2048, NULL, 11, NULL);
//                    if(a>0 || b>0 || c>0 || d>0){
//                    SetRx();
//                    }
//             }
//             else if(*buffer=='r'){
//                    a=0,b=0,c=0,d=0;
//                    if(sweepTask!=NULL){
//                    vTaskDelete(sweepTask);
//                    sweepTask=NULL;
//                    }
//                    if(getDataTask!=NULL){
//                    vTaskDelete(getDataTask);
//                    getDataTask=NULL;
//                    }
//                    ESP_LOGI(TAG, "All codes have been reset");
//             }
//             else if(*buffer=='a'){
//                   ESP_LOGI(TAG,"the code stored in a= %lu",a);
//                   if(p_mode){
//                   track=1;
//                   vTaskDelay(5/portTICK_PERIOD_MS);
//                   }
//                   else
//                   Transmitting(a,&var);
//             }
//             else if(*buffer=='b'){
//                   ESP_LOGI(TAG,"the code stored in b= %lu",b);
//                   if(p_mode){
//                   track=2;
//                   vTaskDelay(5/portTICK_PERIOD_MS);
//                   }
//                   else
//                   Transmitting(b,&var);
//             }
//             else if(*buffer=='c'){
//                   ESP_LOGI(TAG,"the code stored in c= %lu",c);
//                   if(p_mode){
//                   track=3;
//                   vTaskDelay(5/portTICK_PERIOD_MS);
//                   }
//                   else
//                   Transmitting(c,&var);
//             }
//             else if(*buffer=='d'){
//                   ESP_LOGI(TAG,"the code stored in d= %lu",d);
//                   if(p_mode){
//                   track=4;
//                   vTaskDelay(5/portTICK_PERIOD_MS);
//                   }
//                   else
//                   Transmitting(d,&var);
//             }
//             else if(*buffer=='u'){
//                   // data[len] = '\0';
//                   if(getDataTask != NULL){
//                    vTaskDelete(getDataTask);
//                    getDataTask = NULL;

//                   }
//                   ESP_LOGI(TAG, "Please input the frequency");
//                   if(sweepTask!=NULL){
//                    vTaskDelete(sweepTask);
//                    sweepTask=NULL;
//                   }
//                   int leng=0;
//                   while(leng<1)
//                   leng=uart_read_bytes(ECHO_UART_PORT_NUM, buffer, (BUF_SIZE - 1), 20 / portTICK_RATE_MS);
//                   ESP_LOGI(TAG,"length %d\n",leng);
//                   bool condition=true;
//                   while(condition){
//                         if(leng){
//                         // memset(data,0,BUF_SIZE-1);
                        
//                         float recv=atof((char*)buffer);
//                         // cc1101.SetRx();
//                         setMHZ(recv);
//                         ESP_LOGI(TAG, "Frequency has been set to: %f", recv);
//                         xTaskCreate(getData, "get_data_task", 2048*2, NULL, 12, &getDataTask);
//                         condition=false;
//                         task=false;
//                         }
//                   }
                  
//             }
//             else if(*buffer=='f'){
//                   if(getDataTask != NULL){
//                   vTaskDelete(getDataTask);
//                   getDataTask=NULL;

//             }
//                   xTaskCreatePinnedToCore(sweepFrequencyTask, "sweep_freq_task", 2*2048, NULL, 11, &sweepTask,1);
//             }
//               else if(*buffer=='t'){
//                   SetTx();
//             }
//             else{
//                   if(p_mode){
//                         ESP_LOGI(TAG,"Exited Programming Mode");
//                         p_mode=false;
//                   }
//                   else{
//                         // data[len] = '\0';
//                         // float recv=atof((char*)data);
//                         ESP_LOGI(TAG, "Recv str: %s", (char *) buffer);
//                         // ESP_LOGI(TAG, "Recv value: %f", recv);
//                   }
           
//             }
//         }





void app_main(void)
{
      RCSwitchInit(&var);
      //  cc1101.setup();
      // xTaskCreatePinnedToCore(sweepFrequencyTask, "sweep_freq_task", 2*2048, NULL, 11, NULL,1);
      // xTaskCreatePinnedToCore(echo_task, "uart_echo_task", 2*ECHO_TASK_STACK_SIZE, NULL, 9, NULL,0);
      // ezlopi_uart_init(ECHO_UART_BAUD_RATE, ECHO_TEST_TXD, ECHO_TEST_RXD, echo_task_upcall, NULL);
      Init(); // initialize the cc1101
      setSpiPin(13, 11, 12, 14); // Set custom pins: (SCK, MISO, MOSI, CSN). Must be BEFORE Init()!
      setGDO(10, 9); // Set custom GDO pins: (GDO0, GDO2). Gdo0 (tx) and Gdo2 (rx). Must be BEFORE Init();
      SetRx();
      ESP_LOGI(TAG,"%u",getPartnum());
      ESP_LOGI(TAG,"%u",getVersion());
      cc1101_events event=programming;
      cc1101_data data={
            .freq=433.92,
            .button = 1,
	      .mode = set_freq,
	      .setting = user_input
      };
      API_function(event, &data);
      while(wFlag!=false);
      data.mode=set_button;
      API_function(event,&data);

      // setMHZ(433.800598); // Set basic frequency. The lib calculates the frequency automatically (default: 433.92). CC1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
      // xTaskCreate(getData, "get_data_task", 2048*2, NULL, 12, &getDataTask);
      // xTaskCreatePinnedToCore(getData, "get_data_task", 2048*2, NULL, 9, NULL,0);
      
}
