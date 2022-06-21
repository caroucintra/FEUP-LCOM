#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "mouse.h"

uint32_t global_mouse_counter = 0;
uint32_t global_counter = 0;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {


  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    
    uint8_t r;
    uint8_t bit_no;

    my_mouse_enable_data_reporting(); 

 // implement for project

    if (mouse_subscribe_int(&bit_no)) return -1;

    uint8_t irq_set = BIT(bit_no);

    int ipc_status;
    message msg;
    while( cnt > global_mouse_counter) {
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:			
                if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                    mouse_ih();
                    if (pack_done){
                        global_mouse_counter++;
                        mouse_print_packet(&packet);
                        pack_done = false;
                    }
                }
                
                break;
            default:
                break;
        }
    } else { 
    }
    }

    if (mouse_unsubscribe_int()) return -1;

    disable_data_reporting();
    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    uint8_t r;
    uint8_t bit_no_timer;
    uint8_t bit_no_mouse;

    mouse_enable_data_reporting(); // implement for project

    if (mouse_subscribe_int(&bit_no_mouse)) return -1;

    uint8_t irq_set_mouse = BIT(bit_no_mouse);

    if (timer_subscribe_int(&bit_no_timer)) return -1;

    uint8_t irq_set_timer = BIT(bit_no_timer);

    int ipc_status;
    message msg;
    while( global_counter <= idle_time * sys_hz()) {
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:			
                if (msg.m_notify.interrupts & irq_set_mouse) { /* subscribed interrupt */
                    mouse_ih();
                    if (pack_done){
                        global_mouse_counter++;
                        mouse_print_packet(&packet);
                        mouse_handle_pos();
                        pack_done = false;
                        global_counter = 0;
                    }
                }

                if (msg.m_notify.interrupts & irq_set_timer) { /* subscribed interrupt */
                    timer_int_handler();
                }
                
                break;
            default:
                break;
        }
    } else { 
    }
    }

    if (mouse_unsubscribe_int()) return -1;

    if (timer_unsubscribe_int()) return -1;


    disable_data_reporting();
    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    
    uint8_t r;
    uint8_t bit_no;


    my_mouse_enable_data_reporting(); 

    if (mouse_subscribe_int(&bit_no)) return -1;

    uint8_t irq_set = BIT(bit_no);

    int ipc_status;
    message msg;
    while( true) {
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:			
                if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                    mouse_ih();
                    if (pack_done){
                        global_mouse_counter++;
                        mouse_print_packet(&packet);
                        mouse_handle_pos();

                        pack_done = false;
                    }
                    mouse_check_pattern( x_len, tolerance);
                }
                
                break;
            default:
                break;
        }
    } else { 
    }
    }

    if (mouse_unsubscribe_int()) return -1;

    disable_data_reporting();
    return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {

    uint8_t r;
    uint8_t bit_no_timer;
    uint8_t bit_no_mouse;

    mouse_enable_data_reporting(); // implement for project

    if (mouse_subscribe_int(&bit_no_mouse)) return -1;

    uint8_t irq_set_mouse = BIT(bit_no_mouse);

    if (timer_subscribe_int(&bit_no_timer)) return -1;

    uint8_t irq_set_timer = BIT(bit_no_timer);
    uint16_t idle_time = 2;


    int ipc_status;
    message msg;
    while( global_counter <= idle_time * sys_hz()) {
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:			
                if (msg.m_notify.interrupts & irq_set_mouse) { /* subscribed interrupt */
                    mouse_ih();
                    if (pack_done){
                        global_mouse_counter++;
                        mouse_handle_pos();
                        pack_done = false;
                        global_counter = 0;
                    }
                }

                if (msg.m_notify.interrupts & irq_set_timer) { /* subscribed interrupt */
                    timer_int_handler();
                }
                
                break;
            default:
                break;
        }
    } else { 
    }
    }

    if (mouse_unsubscribe_int()) return -1;

    if (timer_unsubscribe_int()) return -1;


    disable_data_reporting();
    return 0;
    
}
