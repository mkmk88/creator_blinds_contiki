#include <dev/leds.h>
#include <sys/ctimer.h>
#include "blinds_controller.h"

static struct ctimer ct;

static void set_blinds_down()
{
    leds_on(LED1);
    leds_off(LED2);
}

static void set_blinds_up()
{
    leds_on(LED2);
    leds_off(LED1);
}

static void set_blinds_idle(void *ptr)
{
    leds_off(LED1);
    leds_off(LED2);

    ctimer_reset(&ct);
}

blinds_ctrl_ret_t blinds_controller_update(int direction, int time)
{
    if (direction > 0 ) {
        set_blinds_up();
    } else {
        set_blinds_down();
    }

    ctimer_set(&ct, CLOCK_SECOND * time, set_blinds_idle, NULL);

    return BLINDS_OK;
} 
