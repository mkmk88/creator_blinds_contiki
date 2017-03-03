#include <dev/leds.h>
#include "blinds_controller.h"

blinds_ctrl_ret_t blinds_controller_update(int new_position)
{
    leds_toggle(LED1); 

    return BLINDS_OK;
} 
