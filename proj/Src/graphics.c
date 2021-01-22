#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "graphics.h"
#include "kbc.h"
#include "mouse.h"
#include "timer.h"
#include "i8254.h"

static uint8_t *video_mem;  // frame-buffer VM address
static uint8_t* aux_buff;   // auxiliar frame-buffer
static uint8_t* background; // Buffer storing the background
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */
static unsigned red_size, green_size, blue_size; // size of the colors so we can easily parse them
static unsigned red_pos, green_pos, blue_pos;     // bit where the colors start in the variable color
static unsigned memory_model;   // Indexed or Direct mode
static const enum xpm_image_type type = XPM_5_6_5; // type used in the project
static bool main_buffer = true; // use this to help in page flipping

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
    mr.mr_limit = mr.mr_base + 2*vram_size;

    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    
    /* Map memory */

    video_mem = vm_map_phys(SELF, (void*) mr.mr_base, vram_size);
    aux_buff = vm_map_phys(SELF, (void*) (mr.mr_base + vram_size), vram_size);
    background = (uint8_t*) malloc(h_res*v_res*bytes_per_pixel);

    if(video_mem == MAP_FAILED || aux_buff == MAP_FAILED)
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

int vg_draw_pixel(uint16_t x, uint16_t y, uint16_t color){
    if (x >= h_res || y >= v_res || x < 0 || y < 0){
        return 1;
    }
    uint8_t msb, lsb;
    util_get_MSB(color, &msb);
    util_get_LSB(color, &lsb);
    size_t byte = (h_res*y + x)*bytes_per_pixel;
    if (main_buffer){
        aux_buff[byte] = lsb;
        aux_buff[byte + 1] = msb;
    }
    else{
        video_mem[byte] = lsb;
        video_mem[byte + 1] = msb;
    }
    return 0;
}

int vg_get_pixel(uint16_t x, uint16_t y, uint16_t* color){
    if (x >= h_res || y >= v_res || x < 0 || y < 0){
        return 1;
    }
    uint8_t msb, lsb;
    size_t byte = (h_res*y + x)*bytes_per_pixel;
    if (main_buffer){
        lsb = aux_buff[byte];
        msb = aux_buff[byte + 1];
    }
    else{
        lsb = video_mem[byte];
        msb = video_mem[byte + 1];
    }
    (*color) = (uint16_t) (((uint16_t) msb) << 8) | (uint16_t) lsb;
    return 0;
}

int vg_draw_from_background(uint16_t x, uint16_t y){
    if (x >= h_res || y >= v_res || x < 0 || y < 0){
        return 1;
    }
    uint8_t msb, lsb;
    size_t byte = (h_res*y + x)*bytes_per_pixel;
    lsb = background[byte];
    msb = background[byte + 1];
    uint16_t color = (uint16_t) (((uint16_t) msb) << 8) | (uint16_t) lsb;
    if (vg_draw_pixel(x, y, color) != OK) return 1;
    return 0;
}

void buffer_screen(){
    reg86_t reg86;
    memset(&reg86, 0, sizeof(reg86));	/* zero the structure */
    reg86.ax = VBE_PAGE_FLIPPING;
    reg86.bh = 0x00;  // reserved
    reg86.bl = VBE_SET_VTRACE;
    reg86.cx = 0; // x position, always 0
    reg86.intno = VBE_BIOS_CALL;
    if (main_buffer) reg86.dx = v_res;  // y position, in order to change buffers
    else reg86.dx = 0;
    if (sys_int86(&reg86) != OK){
        printf("sys_int86 error\n");
        return;
    }
}

void flip_page(){
    main_buffer = !main_buffer;
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

enum xpm_image_type get_type(){
    return type;
}

unsigned get_h_res(){
    return h_res;
}

unsigned get_v_res(){
    return v_res;
}

int vg_exit_program(){
    if (vg_exit()) return 1;
    memset(aux_buff, 0, v_res*h_res*bytes_per_pixel);
    free(background);
    return 0;
}

void load_background(){
    if (main_buffer)
      memcpy(background, aux_buff, v_res*h_res*bytes_per_pixel);
    else
      memcpy(background, video_mem, v_res*h_res*bytes_per_pixel);
}

void erase_everything(){
    for (unsigned int x = 0; x < h_res; ++x){
        for (unsigned int y = 0; y < v_res; ++y){
            uint16_t color;
            vg_get_pixel(x, y, &color);
            if (color != 0)
                vg_draw_pixel(x, y, 0);
        }
    }
}
