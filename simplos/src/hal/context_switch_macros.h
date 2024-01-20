#ifndef HAL_CONTEXT_SWITCH_MACROS_H_
#define HAL_CONTEXT_SWITCH_MACROS_H_

// offset caused by registers.
// NOTE: avoids exposing avr architecture macros
// Macro for context switch compatability.
#define CLEAR_IO_REG(io_reg)                                  \
  asm volatile("sts  %[reg], __zero_reg__  \n\t" ::[reg] "M"( \
      ((uint16_t)(&(io_reg)-0x20))));

#define RESET_TIMER()   \
  CLEAR_IO_REG(TCNT1L); \
  CLEAR_IO_REG(TCNT1H);

#define SCILENT_ENABLE_MT()                                             \
  asm volatile(                                                         \
      "push r16                         \n\t "                          \
      "lds r16, %[timer_adr]            \n\t "                          \
      "ori r16, (1 << %[enable_bit])    \n\t "                          \
      "sts %[timer_adr], r16            \n\t "                          \
      "pop r16                          \n\t " ::[timer_adr] "i"(0x6F), \
      [enable_bit] "I"(OCIE1A));  // Set enable bit for TIMSK1

#define SCILENT_DISABLE_MT()                                            \
  asm volatile(                                                         \
      "push r16                         \n\t "                          \
      "lds r16, %[timer_adr]            \n\t "                          \
      "andi r16, ~(1 << %[enable_bit])  \n\t "                          \
      "sts %[timer_adr], r16            \n\t "                          \
      "pop r16                          \n\t " ::[timer_adr] "i"(0x6F), \
      [enable_bit] "I"(OCIE1A));  // Unset enable bit for TMSK1

#define SAVE_CONTEXT()                    \
  asm volatile(                           \
      "push  r0                     \n\t" \
      "in    r0, __SREG__           \n\t" \
      "push  r0                     \n\t" \
      "push  r1                     \n\t" \
      "clr   r1                     \n\t" \
      "push  r2                     \n\t" \
      "push  r3                     \n\t" \
      "push  r4                     \n\t" \
      "push  r5                     \n\t" \
      "push  r6                     \n\t" \
      "push  r7                     \n\t" \
      "push  r8                     \n\t" \
      "push  r9                     \n\t" \
      "push  r10                    \n\t" \
      "push  r11                    \n\t" \
      "push  r12                    \n\t" \
      "push  r13                    \n\t" \
      "push  r14                    \n\t" \
      "push  r15                    \n\t" \
      "push  r16                    \n\t" \
      "push  r17                    \n\t" \
      "push  r18                    \n\t" \
      "push  r19                    \n\t" \
      "push  r20                    \n\t" \
      "push  r21                    \n\t" \
      "push  r22                    \n\t" \
      "push  r23                    \n\t" \
      "push  r24                    \n\t" \
      "push  r25                    \n\t" \
      "push  r26                    \n\t" \
      "push  r27                    \n\t" \
      "push  r28                    \n\t" \
      "push  r29                    \n\t" \
      "push  r30                    \n\t" \
      "push  r31                    \n\t" \
      "ldi   r26, lo8(%0)           \n\t" \
      "ldi   r27, hi8(%0)           \n\t" \
      "in    r0, __SP_L__           \n\t" \
      "st    x+, r0                 \n\t" \
      "in    r0, __SP_H__           \n\t" \
      "st    x+, r0                 \n\t" \
      : /* No outputs*/                   \
      : "i"(&task_sp));

#define SAVE_SP()                 \
  asm volatile(                   \
      "ldi  r26, lo8(%0)    \n\t" \
      "ldi  r27, hi8(%0)    \n\t" \
      "in   r16, __SP_L__   \n\t" \
      "st   x+, r16         \n\t" \
      "in   r16, __SP_H__   \n\t" \
      "st   x+, r16         \n\t" \
      : /* No outputs*/           \
      : "i"(&task_sp)             \
      : "r16", "r17", "r26", "r27", "memory");

// REWORK to not use r28, r29. They are needed as frame pointers.
#define SELECT_SCHEDULED_TASK_OR_SCHEDULER()                   \
  asm volatile(                                                \
      "ldi r26, lo8(%[next_sp])         \n\t"                  \
      "ldi r27, hi8(%[next_sp])         \n\t"                  \
      "ld  r16, x+                      \n\t"                  \
      "ld  r17, x                       \n\t"                  \
      "mov r18, r16                     \n\t"                  \
      "or  r18, r17                     \n\t"                  \
      "cpse r18, __zero_reg__           \n\t"                  \
      "rjmp .+8                         \n\t"                  \
      "ldi r30, lo8(%[scheduler_sp])    \n\t"                  \
      "ldi r31, hi8(%[scheduler_sp])    \n\t"                  \
      "ld  r16, z+                      \n\t"                  \
      "ld  r17, z                       \n\t"                  \
      "ldi r30, lo8(%[task_sp])         \n\t"                  \
      "ldi r31, hi8(%[task_sp])         \n\t"                  \
      "st  z+, r16                      \n\t"                  \
      "st  z,  r17                      \n\t"                  \
      "st  x, __zero_reg__              \n\t"                  \
      "st  -x, __zero_reg__             \n\t"                  \
      : /* No outputs */                                       \
      : [next_sp] "i"(&next_task_sp), [task_sp] "i"(&task_sp), \
        [scheduler_sp] "i"(&scheduler_task_sp)                 \
      : "r16", "r17", "r18", "r19", "r20", "r26", "r27", "r30", "r31");

#define SET_SP()                  \
  asm volatile(                   \
      "ldi  r26, lo8(%0)    \n\t" \
      "ldi  r27, hi8(%0)    \n\t" \
      "ld   r28, x+         \n\t" \
      "ld   r29, x+         \n\t" \
      "out  __SP_L__, r28   \n\t" \
      "out  __SP_H__, r29   \n\t" \
      : /*No outputs*/            \
      : "i"(&task_sp)             \
      : "r16", "r17", "r26", "r27", "memory");

#define RESTORE_CONTEXT()                \
  asm volatile(                          \
      "ldi  r26, lo8(%0)           \n\t" \
      "ldi  r27, hi8(%0)           \n\t" \
      "ld   r28, x+                \n\t" \
      "out  __SP_L__, r28          \n\t" \
      "ld   r29, x+                \n\t" \
      "out  __SP_H__, r29          \n\t" \
      "pop  r31                    \n\t" \
      "pop  r30                    \n\t" \
      "pop  r29                    \n\t" \
      "pop  r28                    \n\t" \
      "pop  r27                    \n\t" \
      "pop  r26                    \n\t" \
      "pop  r25                    \n\t" \
      "pop  r24                    \n\t" \
      "pop  r23                    \n\t" \
      "pop  r22                    \n\t" \
      "pop  r21                    \n\t" \
      "pop  r20                    \n\t" \
      "pop  r19                    \n\t" \
      "pop  r18                    \n\t" \
      "pop  r17                    \n\t" \
      "pop  r16                    \n\t" \
      "pop  r15                    \n\t" \
      "pop  r14                    \n\t" \
      "pop  r13                    \n\t" \
      "pop  r12                    \n\t" \
      "pop  r11                    \n\t" \
      "pop  r10                    \n\t" \
      "pop  r9                     \n\t" \
      "pop  r8                     \n\t" \
      "pop  r7                     \n\t" \
      "pop  r6                     \n\t" \
      "pop  r5                     \n\t" \
      "pop  r4                     \n\t" \
      "pop  r3                     \n\t" \
      "pop  r2                     \n\t" \
      "pop  r1                     \n\t" \
      "pop  r0                     \n\t" \
      "out  __SREG__, r0           \n\t" \
      "pop  r0                     \n\t" \
      : /*No outputs*/                   \
      : "i"(&task_sp));

#define CONTEXT_SWTICH()                \
  SAVE_CONTEXT();                       \
  SCILENT_DISABLE_MT();                 \
  SELECT_SCHEDULED_TASK_OR_SCHEDULER(); \
  RESET_TIMER();                        \
  RESTORE_CONTEXT();                    \
  SCILENT_ENABLE_MT();

#endif  // HAL_CONTEXT_SWITCH_MACROS_H_
