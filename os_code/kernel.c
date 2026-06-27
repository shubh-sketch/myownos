// kernel.c - Adiyogi OS Main Kernel Entry
#include <stdint.h>

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("inb %1, %0": "=a"(result): "Nd"(port) );
    return result;
}

void outb(uint16_t port, uint8_t data) {
    asm volatile("outb %1, %0":: "Nd"(port), "a"(data));
    return;
}

void update_hardware_cursor(int* cursor_index) {
    uint16_t cursor = (uint16_t)(*cursor_index / 2);
    outb((uint16_t)0x3D4, (uint8_t)14);
    outb((uint16_t)0x3D5, (uint8_t)((cursor >> 8) & 0xFF));
    outb((uint16_t)0x3D4, (uint8_t)15);
    outb((uint16_t)0x3D5, (uint8_t)(cursor & 0xFF));
    return;
}

char scancode_to_ascii(uint8_t scancode) {
    switch(scancode) {
        case 0x1E: return 'a'; case 0x30: return 'b'; case 0x2E: return 'c';
        case 0x20: return 'd'; case 0x12: return 'e'; case 0x21: return 'f';
        case 0x22: return 'g'; case 0x23: return 'h'; case 0x17: return 'i';
        case 0x24: return 'j'; case 0x25: return 'k'; case 0x26: return 'l';
        case 0x32: return 'm'; case 0x31: return 'n'; case 0x18: return 'o';
        case 0x19: return 'p'; case 0x10: return 'q'; case 0x13: return 'r';
        case 0x1F: return 's'; case 0x14: return 't'; case 0x16: return 'u';
        case 0x2F: return 'v'; case 0x11: return 'w'; case 0x2D: return 'x';
        case 0x15: return 'y'; case 0x2C: return 'z';
        case 0x39: return ' '; // Spacebar
        case 0x02: return '1'; case 0x03: return '2'; case 0x04: return '3';
        case 0x05: return '4'; case 0x06: return '5'; case 0x07: return '6';
        case 0x08: return '7'; case 0x09: return '8'; case 0x0A: return '9';
        case 0x0B: return '0'; case 0x1C: return '\n'; case 0x0E: return '\b';
        default:   return 0;   // Ignore keys we haven't mapped yet
    }
}

void clear_screen(volatile char* video_memory, int* cursor_index) {
    // Clear screen with a dark blue background / gray text pattern
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';     // Blank character
        video_memory[i+1] = 0x0F;  // Blue background, Light Gray text
    }
    (*cursor_index) = 0;
}

void print_char(char c, volatile char* video_memory, int* cursor_index) {
    if((*cursor_index) > 4000) {
        return;
    }
    video_memory[*cursor_index] = c;
    video_memory[(*cursor_index)+1] = 0x0F;
    (*cursor_index) += 2;
    update_hardware_cursor(cursor_index);
}

void backspace(volatile char* video_memory, int* cursor_index) {
    if(video_memory[(*cursor_index)-2] == '>') {
        return;
    }
    (*cursor_index) -= 2;
    video_memory[(*cursor_index)] = ' ';
    video_memory[(*cursor_index) + 1] = 0x0F;
    update_hardware_cursor(cursor_index);
}

void enter(int* cursor_index) {
    int current_row = (*cursor_index) / 160;
    current_row += 1;
    *cursor_index = (current_row * 160); 
    update_hardware_cursor(cursor_index);
}

void kernel_main(void) {
    // Pointer to raw VGA text-mode video memory
    volatile char* video_memory = (volatile char*)0xB8000;
    int cursor_index = 0;
    
    clear_screen(video_memory, &cursor_index);
    print_char('-', video_memory, &cursor_index);
    print_char('>', video_memory, &cursor_index);
    
    while (1) {
        if(inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);
            
            if (scancode & 0x80) {
                continue;
            }
            
            char ascii = scancode_to_ascii(scancode);
            if(ascii != 0) {
                if(ascii == '\b') {
                    backspace(video_memory, &cursor_index);
                    continue;
                }
                if(ascii == '\n') {
                    enter(&cursor_index);
                    print_char('-', video_memory, &cursor_index);
                    print_char('>', video_memory, &cursor_index);
                    continue;
                }
                print_char(ascii, video_memory, &cursor_index);
            }
        }
    }
}