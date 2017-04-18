#include <dev/leds.h>
#include "blinds_controller.h"

blinds_ctrl_ret_t blinds_controller_update(int direction, int time)
{
    leds_toggle(LED1); 

    return BLINDS_OK;
} 
