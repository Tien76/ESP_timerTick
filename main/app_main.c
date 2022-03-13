#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "input_iot.h"


#define BIT_PRESS_SHORT     ( 1 << 0 )
#define BIT_PRESS_NORMAL    ( 1 << 1 )
#define BIT_PRESS_LONG      ( 1 << 2 )

#define BLINK_GPIO      CONFIG_BLINK_GPIO
#define BLINK_LED_12    GPIO_NUM_12

static EventGroupHandle_t xCreatedEventGroup;
static BaseType_t xHigherPriorityTaskWoken;
static TaskHandle_t xHandle = NULL;

uint8_t status_pin;

void input_event_callback(int pin, uint64_t tick)
{
    if(pin == GPIO_NUM_0){
        xHigherPriorityTaskWoken = pdFALSE;
        int press_ms = tick*portTICK_PERIOD_MS;
        if (press_ms < 1000)
        {
            // press short
            xEventGroupSetBitsFromISR(xCreatedEventGroup, BIT_PRESS_SHORT, &xHigherPriorityTaskWoken);
        } else if ( press_ms <= 3000 )
        {
            // press normal
            xEventGroupSetBitsFromISR(xCreatedEventGroup, BIT_PRESS_NORMAL, &xHigherPriorityTaskWoken);
        } else if (press_ms > 3000)
        {
            // press long
            // xEventGroupSetBitsFromISR(xCreatedEventGroup, BIT_PRESS_LONG, &xHigherPriorityTaskWoken);
        }
    }
}

void button_timeout_callback(int pin)
{
    if(pin == GPIO_NUM_0)
    {
        gpio_set_level(BLINK_LED_12, 1 - status_pin);

        status_pin = 1 - status_pin;
        printf("TIMER_OUT\n");  
    }
}

void vTask_code(void * pvParameters)
{
    for ( ;; )
    {
        EventBits_t  uxBits = xEventGroupWaitBits(
                                                xCreatedEventGroup, 
                                                BIT_PRESS_SHORT | BIT_PRESS_NORMAL | BIT_PRESS_LONG,
                                                pdTRUE,       
                                                pdFALSE,    
                                                portMAX_DELAY );

        if(  uxBits & BIT_PRESS_SHORT )
        {
           printf("Press Short!\n");
        }
        else if( uxBits & BIT_PRESS_NORMAL )
        {
            printf("Press Normal!\n");
        }
        else if( uxBits & BIT_PRESS_LONG )
        {
            printf("Press Long!\n");
        }
    }

}

void app_main(void)
{
    xCreatedEventGroup = xEventGroupCreate();
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(BLINK_LED_12);
    gpio_set_direction(BLINK_LED_12, GPIO_MODE_OUTPUT);

    gpio_set_level(BLINK_LED_12, 0);

    status_pin = 0;
    

    input_io_create(GPIO_NUM_0, ANY_EDLE);
    input_set_callback(input_event_callback);
    input_set_timeout_callback(button_timeout_callback);
    xTaskCreate(
                vTask_code,      
                "Task1",      
                2048,     
                ( void * ) 1,  
                5,
                &xHandle );

}
