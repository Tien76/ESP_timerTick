#ifndef INPUT_IOT_H
#define INPUT_IOT_H
#include "esp_err.h"
#include "hal/gpio_types.h"

#define BUTTON0     GPIO_NUM_0
typedef void (*input_callback_t) (int, uint64_t);
typedef void (*timeoutButton_t) (int);

typedef enum{
    LO_TO_HI = 1,
    HI_TO_LO = 2,
    ANY_EDLE = 3
} interrupt_type_edle_t;

void input_io_create(gpio_num_t gpio_num, interrupt_type_edle_t type);
int input_io_get_level(gpio_num_t gpio_num);
void input_set_callback(void *cb);
void input_set_timeout_callback(void *cb);


#endif