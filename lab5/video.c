
#include <lcom/lcf.h>
#include "video.h"
#include <stdint.h>

static void *video_mem;
vbe_mode_info_t vmi;

int set_mode(uint16_t mode){
    reg86_t r86;
  
    memset(&r86, 0, sizeof(r86));	/* zero the structure */

    r86.intno = 0x10; /* BIOS video services */
    r86.ah = 0x4F;    /* Set Video Mode function */
    r86.al = 0x02;    /* 80x25 text mode */
    r86.bx = mode | BIT(14);

    if(sys_int86(&r86)) return 1;
    return 0;
}

int (_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t* vm){
    reg86_t r;
    mmap_t map;

    lm_alloc(sizeof(vbe_mode_info_t), &map);

    memset(&r, 0, sizeof(r));	/* zero the structure */

    r.ax = 0x4F01;
    r.cx = mode;
    r.es = PB2BASE(map.phys);
    r.di = PB2OFF(map.phys);
    r.intno = 0x10;

    if( sys_int86(&r) != OK ) { 
      printf("\tvbe_get_mode_info(): sys_int86() failed \n");
      return -1;
    }

    *vm = *(vbe_mode_info_t*) map.virt;
    lm_free(&map);
    return 0;
}

int map_addr_spc(vbe_mode_info_t vmi_p){

    vmi = vmi_p;
    struct minix_mem_range mr;
    unsigned int vram_base = vmi.PhysBasePtr;
    unsigned int vram_size = (vmi.XResolution * vmi.YResolution) * vmi.BitsPerPixel/8;
    int r;
    

    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if( 0 != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED)
        panic("couldn't map video memory");
        
    return 0;
}

int (draw_rectangle)(uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color){
    for (uint16_t i=y; i<(y+height); i++){
        if (vg_draw_hline(x, i, width, color)) return 1;
    }

    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    uint8_t *pixel = video_mem;
    pixel += (vmi.XResolution *y + x)*vmi.BitsPerPixel/8;

    for(uint16_t i=x+1; i<=(x+len); i++){
        draw_pixel(pixel, color);
        pixel += vmi.BitsPerPixel/8;
    }

    return 0;
}

int (draw_pixel)(uint8_t *pixel, uint32_t color){
    memcpy(pixel, &color, vmi.BitsPerPixel/8);
    return 0;
}


int (draw_pixmap)(xpm_image_t img_info, uint8_t *pixmap, uint16_t x, uint16_t y){
    uint8_t *pixel = video_mem;
    uint8_t *color = pixmap;
    uint8_t bytes_per_pixel = vmi.BitsPerPixel/8;
    //uint8_t bytes_per_color_spc = img_info.size / (img_info.width*img_info.height);

    for (uint16_t i=y; i<(y+img_info.height); i++){
        pixel = video_mem;
        pixel += (vmi.XResolution*i + x)*vmi.BitsPerPixel/8;
        for(uint16_t j=0; j<img_info.width; j++){
            memcpy(pixel, &color, vmi.BitsPerPixel/8);
            pixel += bytes_per_pixel;
            color += bytes_per_pixel;
        }
    }
    return 0;
}

int (reset_terminal)(){
    if (draw_rectangle(0, 0, vmi.XResolution, vmi.YResolution, 0)) return 1;
    return 0;
}

uint32_t calculate_color(bool indexed, uint32_t first, uint8_t step, uint8_t no_rectangles, uint16_t row, uint16_t column){
    uint32_t color;
    uint8_t r, g, b, a;
    if (indexed)
        color = (first + (row*no_rectangles + column)*step) % (1 << vmi.BitsPerPixel);
    else {
        r = (((first & 0x00ff0000) >> 16) + column*step) % (1 << vmi.RedMaskSize);
        g = (((first & 0x0000ff00) >> 8) + row*step) % (1 << vmi.GreenMaskSize);
        b = ((first & 0x000000ff) + (column + row)*step) % (1 << vmi.BlueMaskSize);
        a = (first & 0xff000000);
        color = (r << 16) | (g << 8) | b;
    }
    return color;
}


int draw_rectangle_matrix(uint8_t no_rectangles, uint32_t first, uint8_t step, bool indexed){
    uint8_t width = vmi.XResolution/no_rectangles;
    uint8_t height = vmi.YResolution/no_rectangles;
    uint32_t color;
    uint16_t x, y;

    for(uint16_t row=0; row<no_rectangles; row++){
        for(uint16_t column=0; column<no_rectangles; column++){
            color = calculate_color(indexed, first, step, no_rectangles, row, column);
            x = column*width;
            y = row*height;
            if (draw_rectangle(x, y, width, height, color)) return 1;
        }
    }

    return 0;
}

