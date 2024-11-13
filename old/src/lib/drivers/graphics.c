#include "graphics.h"

#define SVGA_BASE_ADDRESS 0xD0000000
#define SVGA_CMD_SET_MODE 0x01

// Get capabilities
#define SVGA_CMD_GET_CAPS 0x00

typedef struct{
    uint32 cmd;
    uint32 status;
    uint32 framebuffer;
} svga_regs_t;

volatile svga_regs_t* svga = (volatile svga_regs_t*) SVGA_BASE_ADDRESS;

void svga_set_mode(uint32 mode){
    svga->cmd = SVGA_CMD_SET_MODE;
    svga->status = mode;
}

uint32 svga_get_caps(){
    svga->cmd = SVGA_CMD_GET_CAPS;
    return svga->status;
}

void* svga_get_fb(){
    return (void*)(svga->framebuffer);
}

void svga_set_framebuffer(uint32 addr){
    svga->cmd = 0x02;      // Must be changed
    svga->status = addr;
}
