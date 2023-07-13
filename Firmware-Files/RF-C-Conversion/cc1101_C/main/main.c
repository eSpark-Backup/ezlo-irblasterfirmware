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
float freq_set;
static const char *TAG = "CC1101";
int track=0;
bool task=true;
TaskHandle_t sweepTask=NULL;
TaskHandle_t getDataTask=NULL;
esp_err_t ret=ESP_FAIL;
#define BUF_SIZE (1024)

void sweepFrequencyTask(void *args)
{
      float start_freq = 300;    //start scan frequency set.
      float stop_freq = 450;    //stop scan frequency set.
      int rssi_threshold = -55; // set the threshold to react. the value should be between -20 and -90. recommended -75.
      float freq = start_freq; //nun
      long compare_freq=0;
      float mark_freq=0;
      int rssi;
      int mark_rssi=-100;
      ESP_LOGI(TAG,"Frequency scanner Ready");

      while(1)
      {
            setMHZ(freq);
            rssi = getRssi();
            if (rssi>rssi_threshold)
            {
                  if (rssi > mark_rssi)
                  {
                        mark_rssi = rssi;  
                        mark_freq = freq;
                  }
            }
            freq+=0.1;

            if (freq>stop_freq)
            {
                  freq=start_freq;
                  if (mark_rssi>rssi_threshold)
                  {
                  long fr = mark_freq*100;
                        if (fr == compare_freq)
                        {
                              ESP_LOGI(TAG,"Freq: %f ",mark_freq);
                              ESP_LOGI(TAG,"Rssi: %d ",mark_rssi);
                              freq_set=mark_freq;
                              mark_rssi=-100;
                              compare_freq = 0;
                              mark_freq = 0;
                              freq=start_freq;
                              task=true;
                              sweepTask = NULL;
                              wFlag=false;
                              vTaskDelete(NULL);
                        }
                        else
                        {
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
      if(event==programming)
      {
            if(pData->mode==set_button)
            {
                   ESP_LOGI(TAG, "Programming mode");
                   wFlag=true;
                  //  SetRx();
                  if(task)
                        setMHZ(freq_set);
                  if(ret!=ESP_OK)
                        ret=enableReceive(9,&var);  // Receiver on interrupt 0
                  ESP_LOGI(TAG,"RX MODE");
                  // Check the CC1101 Spi connection.
                  if (getCC1101())
                  { 
                        ESP_LOGI(TAG,"CC1101: Connection OK.");
                  }
                  else 
                  {
                        ESP_LOGI(TAG,"CC1101: Connection error.");
                  }
                   while(wFlag)
                  {
                        vTaskDelay(75/portTICK_PERIOD_MS);
                        if (available(&var)) 
                        {
                              btn[pData->button]=getReceivedValue(&var);   
                              ESP_LOGI(TAG,"Value stored in button %d is %lu",pData->button,btn[pData->button]);
                              wFlag=false;
                        }
                  }
            }
            else
            {
                  if(pData->setting==freq_sweep)
                  {
                        xTaskCreatePinnedToCore(sweepFrequencyTask, "sweep_freq_task", 2*2048, NULL, 11, &sweepTask,1);
                  }
                  else
                  {
                        setMHZ(pData->freq);
                        task=false;
                        wFlag=false;
                  }
            }
      }
      else
      {
            Transmitting(btn[pData->button],&var);
            ESP_LOGI(TAG,"Transmitted");
      }
}

void app_main(void)
{
      RCSwitchInit(&var);
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
      event=transmitting;
      vTaskDelay(10000/portTICK_PERIOD_MS);
      API_function(event,&data);
}
