#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <machine/int86.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "graphics.h"
#include "kbc.h"
#include "PS2.h"
#include "timer.h"
#include "i8254.h"

static uint8_t *video_mem;  // frame-buffer VM address
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */
static unsigned red_size, green_size, blue_size; // size of the colors so we can easily parse them
static unsigned red_pos, green_pos, blue_pos;     // bit where the colors start in the variable color
static unsigned memory_model;   // Indexed or Direct mode


int vbe_get_mode_info2(uint16_t mode, vbe_mode_info_t *vmi_p){
    mmap_t map;
    reg86_t reg86;
    memset(&reg86, 0, sizeof(reg86));	// zero the structure

    if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL){
        printf("lm_alloc failed\n");
        return 1;
    }
    phys_bytes phys_buffer = map.phys;

    reg86.ax = VBE_MODE_INFO;
    reg86.cx = mode;
    reg86.intno = VBE_BIOS_CALL;
    reg86.es = PB2BASE(phys_buffer);
    reg86.di = PB2OFF(phys_buffer);

    if (sys_int86(&reg86) != OK){
        printf("sys_int86 error\n");
        return 1;
    }
    
    memcpy(vmi_p, map.virt, map.size);

    if (!lm_free(&map)){
        printf("lm_free failed\n");
        return 1;
    }

    return 0;
}


void* (vg_init)(uint16_t mode){
    vbe_mode_info_t info;
    if (vbe_get_mode_info2(mode, &info) != OK){ // if it breaks, use vbe_get_mode_info instead
        printf("vbe_get_mode_info error\n");
        return NULL;
    }
    h_res = info.XResolution;   // probably needs more stuff in the future
    v_res = info.YResolution;
    red_size = info.RedMaskSize;
    green_size = info.GreenMaskSize;
    blue_size = info.BlueMaskSize;
    red_pos = info.RedFieldPosition;
    green_pos = info.GreenFieldPosition;
    blue_pos = info.BlueFieldPosition;
    bits_per_pixel = info.BitsPerPixel;
    memory_model = info.MemoryModel;

    bytes_per_pixel = (unsigned) ceil( (double) bits_per_pixel / 8);

    struct minix_mem_range mr;
    unsigned int vram_base = info.PhysBasePtr;  /* VRAM's physical addresss */
    unsigned int vram_size = bytes_per_pixel * (h_res*v_res);  /* VRAM's size, but you can use the frame-buffer size, instead */
    int r;

    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    
    /* Map memory */

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if(video_mem == MAP_FAILED)
        panic("couldn't map video memory");

    reg86_t reg86;
    memset(&reg86, 0, sizeof(reg86));	/* zero the structure */
    reg86.ax = SET_VBE_MODE;
    reg86.bx = VBE_LINEAR | mode;
    reg86.intno = VBE_BIOS_CALL;
    if (sys_int86(&reg86) != OK){
        printf("sys_int86 error\n");
        return NULL;
    }


    if (reg86.ah == VBE_ERR_CALL){
        printf("Function call failed\n");
        return NULL;
    }
    if (reg86.ah == VBE_ERR_HW){
        printf("Function is not supported in current HW configuration\n");
        return NULL;
    }
    if (reg86.ah == VBE_ERR_MODE){
        printf("Function is invalid in current video mode\n");
        return NULL;
    }
    if (reg86.ah == VBE_FINE)
        return video_mem;

    printf("Unknown Error\n");
    return NULL;
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color){
    if (x > h_res || y > v_res){
        printf("x or y exceeds the resolution\n");
        return 1;
    }
    for (unsigned i = 0; i < bytes_per_pixel; ++i){
        uint8_t byte = (uint8_t) color;
        video_mem[(h_res*y + x)*bytes_per_pixel + i] = byte;
        color = color >> 8;
    }
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    for (int i = x; i < x + len; ++i){
        if (vg_draw_pixel(i, y, color)) return 1;
    }
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    for (int i = y; i < y + height; ++i){
        if (vg_draw_hline(x, i, width, color)) return 1;
    }
    return 0;
}

void get_rgb(uint32_t color, uint32_t* red, uint32_t* green, uint32_t* blue){
    *red = (color >> red_pos) & ((1 << red_size) - 1);
    *green = (color >> green_pos) & ((1 << green_size) - 1);
    *blue = (color >> blue_pos) & ((1 << blue_size) - 1);
}

int vg_draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step){
    uint16_t width = h_res / no_rectangles;
    uint16_t height = v_res / no_rectangles;
    uint32_t color = first;
    uint32_t redf = 0, greenf = 0, bluef = 0;
    uint16_t x = 0, y = 0;

    get_rgb(first, &redf, &greenf, &bluef);

    for (int row = 0; row < no_rectangles; ++row){
        for (int col = 0; col < no_rectangles; ++col){
            if (memory_model == INDEXED_MODE) color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
            else{
                uint32_t red = (redf + col * step) % (1 << red_size);
                uint32_t green = (greenf + row * step) % (1 << green_size);
                uint32_t blue = (bluef + (col + row) * step) % (1 << blue_size);
                color = (red << red_pos) | (green << green_pos) | (blue << blue_pos);
            }
            if (vg_draw_rectangle(x, y, width, height, color) != OK) return 1;
            x += width;
        }
        y += height;
        x = 0;
    }
    return 0;
}

int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y, enum xpm_image_type type){
    xpm_image_t img;
    uint8_t* sprite = xpm_load(xpm, type, &img);
    int i = -1;
    for (int y2 = y; y2 < y + img.height; ++y2){
        for (int x2 = x; x2 < x + img.width; ++x2){
            int byte = (y2*h_res + x2)*bytes_per_pixel;
            for (unsigned j = 0; j < bytes_per_pixel; ++j){
                ++i;
                if (sprite[i] == 0) continue;
                video_mem[byte + j] = sprite[i];
            }
        }
    }
    return 0;
}

int draw_sprite(uint8_t* sprite, xpm_image_t* img, uint16_t x, uint16_t y){
    int i = -1;
    for (int y2 = y; y2 < y + img->height; ++y2){
        for (int x2 = x; x2 < x + img->width; ++x2){
            int byte = (y2*h_res + x2)*bytes_per_pixel;
            for (unsigned j = 0; j < bytes_per_pixel; ++j){
                ++i;
                if (sprite[i] == 0) continue;
                video_mem[byte + j] = sprite[i];
            }
        }
    }
    return 0;
}

int destroy_sprite(uint8_t* sprite, xpm_image_t* img, uint16_t x, uint16_t y){
    int i = -1;
    for (int y2 = y; y2 < y + img->height; ++y2){
        for (int x2 = x; x2 < x + img->width; ++x2){
            int byte = (y2*h_res + x2)*bytes_per_pixel;
            for (unsigned j = 0; j < bytes_per_pixel; ++j){
                ++i;
                if (sprite[i] == 0) continue;
                video_mem[byte + j] = 0;
            }
        }
    }
    return 0;
}

int animate_xpm(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate, enum xpm_image_type type){
    /*
    // Use timer 0's interruptions for the frame rate
    if (timer_set_frequency(TIMER0_IRQ, fr_rate) != OK){
        printf("timer_set_frequency failed\n");
        return 1;
    }
*/
    uint8_t bit_no_k;
    int hook_id;
    if (subscribe_kbc(&bit_no_k, &hook_id) != OK) return 1; 
    
    uint8_t bit_no_timer;
    if (timer_subscribe_int(&bit_no_timer) != OK){
        printf("timer_subscribe_int failed\n");
        return 1;
    }

    xpm_image_t img;
    uint8_t* sprite = xpm_load(xpm, type, &img);

    int ipc_status;
    message msg;
    uint32_t irq_set_timer = BIT(bit_no_timer);
    uint32_t irq_set_k = BIT(bit_no_k);
    uint8_t data = 0;

    uint16_t x = xi, y = yi;
    int frame_counter = 0;
    int int_counter = 0;

    draw_sprite(sprite, &img, x, y);

    while(data != ESC_KEY) { /* You may want to use a different condition */
        /* Get a request message. */
        int r;
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
            printf("driver_receive failed with: %d", r);
            continue;
        }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & irq_set_k) { /* subscribed interrupt */
                  if (read_out_buffer(&data) != OK) return 1;
                }
                if (msg.m_notify.interrupts & irq_set_timer){
                    ++int_counter;
                    if (int_counter % (60/fr_rate) != 0) continue;
                    if (x == xf && y == yf) continue;
                    if (speed >= 0){
                        destroy_sprite(sprite, &img, x, y);
                        if (xi == xf) y += speed;
                        else x += speed;
                        if (x > xf) x = xf;
                        if (y > yf) y = yf;
                    }
                    else{
                        ++frame_counter;
                        if (frame_counter >= -speed){
                            destroy_sprite(sprite, &img, x, y);
                            if (xi == xf) ++y;
                            else ++x;
                            frame_counter = 0;
                        }
                        else continue;
                    }
                    draw_sprite(sprite, &img, x, y);
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
 }

    if (timer_unsubscribe_int() != OK) return 1;

    if (unsubscribe_kbc(&hook_id) != OK) return 1;

    return 0;
}
