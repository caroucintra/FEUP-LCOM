#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"
#include <lcom/timer.h>

int global_counter = 0;
extern uint32_t cnt;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (kbd_test_scan)(){
  uint8_t r;
  uint8_t bit_no;
 
  if (kbd_subscribe_int(&bit_no)) return -1;
    
  uint8_t irq_set = BIT(bit_no);

  int ipc_status;
  message msg;
  while(!(scancode[0] == ESC && !secondbyte)) {
     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:			
                if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                  kbc_ih();
                  uint8_t size = 1;
                  if (secondbyte) size = 2;
                  if (kbd_print_scancode(make, size, scancode)) return 1;
                }
                
                break;
            default:
                break;
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
 }

  if (kbd_unsubscribe_int()) return -1;

  if (kbd_print_no_sysinb(cnt)) return 1;

  return 0;
}


int (kbd_test_poll)(){
  uint8_t st;
    while (scancode[0] != ESC){
      if (util_sys_inb(STAT_REG, &st)) return 1;
      if (st & TO_ERR) return 1;
      if (st & PAR_ERR) return 1;
      if ((st & AUX) || !(st & OBF)) continue;
      kbc_read_keycode();

      uint8_t size = 1;
      if (secondbyte) size = 2;
      if (kbd_print_scancode(make, size, scancode)) return 1;

      tickdelay(micros_to_ticks(POLL_DELAY));
    }

    kbc_interrupts_enable();
    return 0;
}


int (kbd_test_timed_scan)(uint8_t n){
  uint8_t r;
  uint8_t bit_no_timer;
  uint8_t bit_no_kbd;
  uint8_t idle = 0;
 
  if (kbd_subscribe_int(&bit_no_kbd)) return -1;
    
  uint8_t irq_set_kbd = BIT(bit_no_kbd);

  if (timer_subscribe_int(&bit_no_timer)) return -1;
    
  uint8_t irq_set_timer = BIT(bit_no_timer);

  int ipc_status;
  message msg;
  while(scancode[0] != ESC && scancode[1] != ESC && idle < n) {
     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:			
                if (msg.m_notify.interrupts & irq_set_kbd) { /* subscribed interrupt */
                  idle = 0;
                  global_counter = 0;
                  kbc_ih();
                  uint8_t size = 1;
                  if (secondbyte) size = 2;
                  if (kbd_print_scancode(make, size, scancode)) return 1;
                }
                else if (msg.m_notify.interrupts & irq_set_timer) { /* subscribed interrupt */
                  timer_int_handler();

                  if (global_counter%60 == 0){
                    idle++;
                  }
                }
                break;
            default:
                break;
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
 }

  if (kbd_unsubscribe_int()) return -1;

  if (timer_unsubscribe_int()) return -1;

  if (kbd_print_no_sysinb(cnt)) return 1;

  return 0;
}
