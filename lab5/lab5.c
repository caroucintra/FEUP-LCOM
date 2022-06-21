// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "keyboard.h"
#include <lcom/timer.h>
#include "video.h"
#include <lcom/pixmap.h>
#include <lcom/xpm.h>

int global_counter = 0;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (set_mode(mode)) return 1;
  sleep(delay);
  if (vg_exit()) return 1;

  return 0;
}


int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

  vbe_mode_info_t vmi_p;

  vbe_get_mode_info(mode, &vmi_p);

  if (map_addr_spc(vmi_p)) return 1;

  if (set_mode(mode)) return 1;

  if (draw_rectangle(x, y, width, height, color)) return 1;

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

  if (vg_exit()) return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  
  vbe_mode_info_t vmi_p;

  _vbe_get_mode_info(mode, &vmi_p);

  if (map_addr_spc(vmi_p)) return 1;

  if (set_mode(mode)) return 1;

  bool indexed = false;
  if (mode == VBE_MODE) indexed = true;

  if (draw_rectangle_matrix(no_rectangles, first, step, indexed)) return 1;

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

  if (vg_exit()) return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img_info;
  uint8_t *pixmap = xpm_load(xpm, type, &img_info);	

  vbe_mode_info_t vmi_p;
  vbe_get_mode_info(VBE_MODE, &vmi_p);
  if (map_addr_spc(vmi_p)) return 1;
  if (set_mode(VBE_MODE)) return 1;
  
  if (draw_pixmap(img_info, pixmap, x, y)) return 1;

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

  if (vg_exit()) return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img_info;
  uint8_t *pixmap = xpm_load(xpm, type, &img_info);	

  vbe_mode_info_t vmi_p;
  if (set_mode(VBE_MODE)) return 1;
  vbe_get_mode_info(VBE_MODE, &vmi_p);

  if (map_addr_spc(vmi_p)) return 1;
 
  uint8_t r;
  uint8_t bit_no_timer;
  uint8_t bit_no_kbd;
 
  if (kbd_subscribe_int(&bit_no_kbd)) return -1;
    
  uint8_t irq_set_kbd = BIT(bit_no_kbd);

  if (timer_subscribe_int(&bit_no_timer)) return -1;
    
  uint8_t irq_set_timer = BIT(bit_no_timer);

  int ipc_status;
  message msg;

  uint8_t rate = fr_rate*60;
  bool sameX = false;
  if (xi == xf) sameX = true;
  bool done = false;

  if (draw_pixmap(img_info, pixmap, xi, yi)) return 1;

  while(scancode[0] != ESC && scancode[1] != ESC && !done) {
     if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:			
                if (msg.m_notify.interrupts & irq_set_kbd) { /* subscribed interrupt */
                  kbc_ih();
                  uint8_t size = 1;
                  if (secondbyte) size = 2;
                }
                else if (msg.m_notify.interrupts & irq_set_timer) { /* subscribed interrupt */
                  timer_int_handler();
                  if (global_counter%rate == 0){
                    if (speed > 0){
                      if (sameX){
                        if (yi > yf)
                          yi -= speed;
                        else
                          yi += speed;
                      }
                      else {
                        if (xi > xf)
                          xi -= speed;
                        else
                          xi += speed;
                      }
                    }
                    else{
                      if (global_counter%((-1)*speed) == 0){
                      if (sameX){
                        if (yi > yf)
                          yi --;
                        else
                          yi ++;
                      }
                      else {
                        if (xi > xf)
                          xi --;
                        else
                          xi ++;
                      }
                      }
                    }
                    if (reset_terminal()) return 1;
                    if (draw_pixmap(img_info, pixmap, xi, yi)) return 1;
                    if (yi == yf && xi == xf) done = true;
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

  if (vg_exit()) return 1;

  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
