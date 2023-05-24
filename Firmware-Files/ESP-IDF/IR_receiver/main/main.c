#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"

#define maxLen 800
#define rxPinIR 18 //Receiver pin
volatile unsigned int irBuffer[maxLen]; //stores timings - volatile because changed by ISR
volatile unsigned int x = 0; //Pointer thru irBuffer - volatile because changed by ISR

static void IRAM_ATTR gpio_interrupt_handler(void *args) {
  if (x > maxLen) return; //ignore if irBuffer is already full
  irBuffer[x++] = esp_timer_get_time(); //just continually record the time-stamp of signal transitions
}

void app_main(void)
{
  gpio_set_intr_type(rxPinIR,GPIO_INTR_ANYEDGE);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(rxPinIR, gpio_interrupt_handler, (void *)rxPinIR);
  while(1){
  printf("Press the button on the remote now - once only\n");
  vTaskDelay(5000/portTICK_PERIOD_MS); // pause 5 secs
  if (x) { //if a signal is captured

    printf("Raw: (%d)",x-1); //dump raw header format - for library
    for (int i = 1; i < x; i++) { //now dump the times
      // if (!(i & 0x1)) {
      // printf("-");
      // }
      printf(" %d, ",irBuffer[i] - irBuffer[i - 1]);
    }
    x = 0;
    printf("\n");
    gpio_isr_handler_add(rxPinIR, gpio_interrupt_handler, (void *)rxPinIR);
}
}
}

