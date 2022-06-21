#ifndef _LCOM_MOUSE_C_
#define _LCOM_MOUSE_C_
#include "mouse.h"

#include <lcom/lcf.h>

int byte_no = 0;
bool pack_done = false;
uint8_t lineXLen = 0;
uint8_t lineYLen = 0;
uint16_t x = 30;
uint16_t y = 200;
/*
enum state_t state = INIT;
enum pressed_t last_pressed = NONE;
*/

int (mouse_subscribe_int)(uint8_t *bit_no){
  mouse_hookid = 2;
  *bit_no = mouse_hookid;

  if (sys_irqsetpolicy(MOUSE_IRQ,  IRQ_REENABLE|IRQ_EXCLUSIVE , &mouse_hookid)) return -1;

  return 0;
}


int (mouse_unsubscribe_int)(){

  return sys_irqrmpolicy(&mouse_hookid);

}


int (write_kbc_reg)(uint8_t port, uint8_t command){
    int i = 0;
    uint8_t st;
    while (i < 5){
        if (util_sys_inb(KBC_CMD_REG, &st)) return 1;//MUDAR PARA PORT

        if (!(st & TO_ERR) && !(st & PAR_ERR) && !(st & IBF)) {
                sys_outb(port, command);
                return 0;
            }

        i++;
        tickdelay(micros_to_ticks(25));
    }
    return -1;
}

int (get_answer)(uint8_t * answer){
    int i = 0;
    uint8_t st;
    while (i < 5){
        printf("%d\n",i);
        if (util_sys_inb(KBC_CMD_REG, &st)) return 1;//MUDAR PARA PORT
        //if ((st & OBF)) {
                util_sys_inb(KBC_IN_BUF, answer);

                if ((st & TO_ERR) || (st & PAR_ERR)){
                    continue;
                }

                return 0;
            //}

        i++;
        tickdelay(micros_to_ticks(25));
    }
    return -1;
}


int my_mouse_enable_data_reporting(){
    uint8_t answer;

    int i = 0;

    do{
        write_kbc_reg(KBC_CMD_REG, BYTE_TO_MOUSE); 

        
        if (write_kbc_reg(KBC_OUT_BUF, ENABLE_DATA_REPORTING)){
            printf("erro\n");
            return -1;
        };
        get_answer(&answer);
        i++;
        

    }while(answer != ACK & i<5);

    return 0;
}


int disable_data_reporting(){
    uint8_t answer;

    int i = 0;

    do{
        write_kbc_reg(KBC_CMD_REG, BYTE_TO_MOUSE); 

        
        if (write_kbc_reg(KBC_OUT_BUF, DISABLE_DATA_REPORTING)){
            printf("erro\n");
            return -1;
        };
        get_answer(&answer);
        i++;
        

    }while(answer != ACK & i<1);

    return 0;
}


void (mouse_ih)(){
    uint8_t st;
    if (util_sys_inb(STAT_REG, &st)) return;
    if (st & TO_ERR) return ;
    if (st & PAR_ERR) return;

    if (st & OBF){
        if (st & AUX){
            handle_byte();
        }
    }

}

int (handle_byte)(){
    uint8_t outp;
    if (util_sys_inb(KBC_OUT_BUF, &outp)) return 1;

    bytes[byte_no] = outp;

    byte_no ++;

    if (byte_no == 3){
        return handle_mouse_package();
    }
    
    return 0;
}

int handle_mouse_package(){
    byte_no = 0;

    packet.bytes[0] = bytes[0]; 
    packet.bytes[1] = bytes[1]; 
    packet.bytes[2] = bytes[2]; 
    packet.rb = ( bytes[0] & RIGHT);
    packet.lb = ( bytes[0] & LEFT);
    packet.mb = ( bytes[0] & MIDDLE);
    packet.x_ov = ( bytes[0] & X_OVF);
    packet.y_ov = ( bytes[0] & Y_OVF);

    if ( bytes[0] & X_SIGN){
        packet.delta_x = ( 0xff00 | bytes[1]);
    }
    else{
        packet.delta_x = bytes[1];
    }
    if ( bytes[0] & Y_SIGN){
        packet.delta_y = ( 0xff00 | bytes[2]);
    }
    else{
        packet.delta_y = bytes[2];
    }

    pack_done = true;
    return 1;

}   

void mouse_handle_pos(){

    x += packet.delta_x;
    y += packet.delta_y;

    printf(" x %d ", x);
    printf(" y %d\n", y);

    if (true) // se o estado for carinho 

    if (x>20 && x < 200 && y>50 && y < 200 && packet.lb) {
        printf("NA VIOLETCHI\n");
    }

    // eu só quero que o rato se mexa posso fazer delta x > tolerance 


    
    if ( x > 400){
        printf("PASSEI DOS 400");
    }

    // dois estados going right going left e tem de acontecer 6 vezes 



}
    


void mouse_check_pattern(uint8_t x_len, uint8_t tolerance) {
    
   /* switch (state) {
        case INIT:
            if ( last_pressed = NONE && packet.lb && !packet.mb && !packet.rb){
                lineXLen = 0;
                lineYLen = 0;
                state = DRAW_UP;

            } //else we stay on INIT
            break;
        case DRAW_UP:
            if (last_pressed = LB && packet.lb && !packet.mb && !packet.rb
                && !( packet.delta_x & 0xff00) && !( packet.delta_y & 0xff00)){
                    lineXLen += packet.delta_x;
                    lineYLen += packet.delta_y;
                }
            else{
                if (last_pressed = LB && !packet.lb && !packet.mb && !packet.rb && (lineXLen >= x_len) && ((lineYLen / lineXLen) > MIN_SLOPE)){
                    state = DRAW_DOWN;
                }
                else{
                    state = INIT;
                }
                lineXLen = 0;
                lineYLen = 0;
            }
            break;
        case DRAW_DOWN:
            if (last_pressed = LB && packet.lb && !packet.mb && !packet.rb
                && !( packet.delta_x & 0xff00) && !( packet.delta_y & 0xff00)){
                    lineXLen += packet.delta_x;
                    lineYLen += packet.delta_y;
                }
            else{
                if (last_pressed = LB && !packet.lb && !packet.mb && !packet.rb && (lineXLen >= x_len) && ((lineYLen / lineXLen) > MIN_SLOPE)){
                    state = DRAW_DOWN;
                }
                else{
                    state = INIT;
                }
                lineXLen = 0;
                lineYLen = 0;
            }
            break;
                                   
    }*/

}

#endif /* _LCOM_MOUSE_C */
