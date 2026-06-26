; boot.asm - 32-bit entry point for Adiyogi OS
bits 32
global _start
extern kernel_main

; Multiboot header flags
MODULEALIGN equ  1 << 0
MEMINFO     equ  1 << 1
FLAGS       equ  MODULEALIGN | MEMINFO
MAGIC       equ  0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .text
_start:
    cli             ; Clear interrupts
    call kernel_main ; Jump into your C code!
.halt:
    hlt             ; Halt the CPU if kernel returns
    jmp .halt

section .note.GNU-stack noalloc noexec nowrite progbits