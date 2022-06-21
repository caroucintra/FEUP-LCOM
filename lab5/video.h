
int set_mode(uint16_t mode);

int _vbe_get_mode_info(uint16_t mode, vbe_mode_info_t* vm);

int map_addr_spc(vbe_mode_info_t vmi);

int (draw_rectangle)(uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int (draw_pixel)(uint8_t *pixel, uint32_t color);

int (draw_pixmap)(xpm_image_t img_info, uint8_t *pixmap, uint16_t x, uint16_t y);

int (do_animation)(Game game);

int (reset_terminal)();

int draw_rectangle_matrix(uint8_t no_rectangles, uint32_t first, uint8_t step, bool indexed);

uint32_t calculate_color(bool indexed, uint32_t first, uint8_t step, uint8_t no_rectangles, uint16_t row, uint16_t column);
