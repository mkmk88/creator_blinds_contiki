#ifndef __BLINDS_CONTROLLER_H__
#define __BLINDS_CONTROLLER_H__

typedef enum {
    BLINDS_OK = 0,
    BLINDS_ERROR
} blinds_ctrl_ret_t;

blinds_ctrl_ret_t blinds_controller_update(int new_position);

#endif
