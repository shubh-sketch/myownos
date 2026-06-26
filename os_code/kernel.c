// kernel.c - Adiyogi OS Main Kernel Entry
#include <stdint.h>

void kernel_main(void) {
    // Pointer to raw VGA text-mode video memory
    volatile char* video_memory = (volatile char*)0xB8000;
    
    // Clear screen with a dark blue background / gray text pattern
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';     // Blank character
        video_memory[i+1] = 0x17;  // Blue background, Light Gray text
    }

    // Write your OS Name right into the center of the first row
    const char* greeting = "--- Welcome to Adiyogi OS ---";
    int i = 0;
    while (greeting[i] != '\0') {
        video_memory[i * 2] = greeting[i];
        video_memory[i * 2 + 1] = 0x14; // Blue background, Yellow text!
        i++;
    }
}