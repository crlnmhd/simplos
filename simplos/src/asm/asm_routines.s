; Assembly routines are needed for context switches.
; Naked interrupts are needed in context switches, however writing those in C is not supported.
; Even though it can appear to be working, subtle and hard to diagnose issues can appear.
; As can be attested to by the author.

.section .text

.global asm_test_function
asm_test_function:
push r17
nop
pop r17
ret


