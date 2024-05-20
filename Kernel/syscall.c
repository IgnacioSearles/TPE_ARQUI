#include <stdint.h>
#include <syscall.h>
#include <interrupts.h>
#include <drivers/videoDriver.h>
#include <drivers/timeDriver.h>
#include <drivers/keyboardDriver.h>

uint64_t sys_read(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9) {
    _sti();
    char c = 0;
    int i = 0;
    char* out_buffer = (char*) rdi;
    while(c != '\n' && i + 1 < rsi){
        if(keys_pending()){
            c = get_pressed_character();
            write_to_video_text_buffer(&c, 1, HEX_WHITE);
            out_buffer[i++] = c;
        }
    }
    return i;
}

uint64_t sys_write(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9) {
    switch (rdi) {
        case 1:
            write_to_video_text_buffer((char*)rsi, rdx, HEX_WHITE);
            return 0;
        case 2:
            write_to_video_text_buffer((char*)rsi, rdx, HEX_RED);
            return 0;
        default:
            return 1;
    }

    return 1;
}

uint64_t sys_set_font_size(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9) {
    set_font_size(rdi);
    return 0;
}

uint64_t sys_put_text(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9) {
    draw_string((char*) rdi, rsi, rdx, r10, r8);
    return 0;
}

uint64_t sys_draw_square(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9) {
    draw_square(rdi, rsi, rdx, r10);
    return 0;
}

uint64_t sys_get_screen_width(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9) {
    return get_video_buffer_width();
}

uint64_t sys_get_screen_height(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9) {
    return get_video_buffer_height();
}

uint64_t sys_get_time(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9){
    return sys_write(rdi, getTime(), 5, r10, r8, r9);
}

uint64_t sys_get_key_pressed(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9) {
    return get_key_pending();
}

uint64_t (*syscalls[])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = {
    sys_read, sys_write, sys_put_text, 
    sys_set_font_size, sys_draw_square, sys_get_screen_width, 
    sys_get_screen_height, sys_get_time, sys_get_key_pressed
};

uint64_t syscall_handler(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9){
    if (rax >= sizeof(syscalls) / sizeof(syscalls[0]))
            return 0;

    return syscalls[rax](rdi, rsi, rdx, r10, r8, r9);
}
