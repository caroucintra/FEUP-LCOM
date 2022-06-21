#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include "i8042.h"

int mouse_hookid;
uint8_t bytes[3];
extern int byte_no;
struct packet packet;
extern bool pack_done;
extern uint16_t x;
extern uint16_t y;

/*
extern enum state_t state;
extern enum pressed_t last_pressed;
extern uint8_t lineXLen;
extern uint8_t lineYLen;*/


void mouse_handle_pos();

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

int my_mouse_enable_data_reporting();

int disable_data_reporting();

int (get_answer)(uint8_t * answer);

int (write_kbc_reg)(uint8_t port, uint8_t command);

int (handle_byte)();

int handle_mouse_package();

void mouse_check_pattern(uint8_t x_len, uint8_t tolerance);

#endif /* _LCOM_MOUSE_H */
