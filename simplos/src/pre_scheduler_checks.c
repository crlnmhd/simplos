#include <stdint.h>

#include "hal/context_switch_macros.h"
#include "io_helpers.h"
#include "simplos.h"
#include "timers.h"

/*
 * Push set some registers with values and check that they have not changed.
 * */
void run_with_stack_integrity_check(void (*test_fn)(void)) {
  const uint8_t r16_canary = 0;
  const uint8_t r17_canary = 33;
  const uint8_t r18_canary = 123;

  asm volatile(
      "push r16                     \n\t"
      "push r17                     \n\t"
      "push r18                     \n\t"
      "ldi  r16, %[r16_canary]      \n\t"
      "ldi  r17, %[r17_canary]      \n\t"
      "ldi  r18, %[r18_canary]      \n\t"
      "push r16                     \n\t"
      "push r17                     \n\t"
      "push r18                     \n\t"
      : /* No outputs */
      : [r16_canary] "M"(r16_canary), [r17_canary] "M"(r17_canary),
        [r18_canary] "M"(r18_canary)
      : "r16", "r17", "r18");

  test_fn();

  asm goto(
      "pop r18                      \n\t"
      "pop r17                      \n\t"
      "pop r16                      \n\t"
      "cpi r16, %[r16_canary]       \n\t"
      "brne %l[stack_error]         \n\t"
      "cpi r17, %[r17_canary]       \n\t"
      "brne %l[stack_error]         \n\t"
      "cpi r18, %[r18_canary]       \n\t"
      "brne %l[stack_error]         \n\t"
      "pop r18                      \n\t"
      "pop r17                      \n\t"
      "pop r16                      \n\t"
      "jmp %l[return_point]"  // Don't return before letting the compiler pop
                              // the funtions variables (depending on
                              // optimization).
      :                       /* No outputs */
      : [r16_canary] "M"(r16_canary), [r17_canary] "M"(r17_canary),
        [r18_canary] "M"(r18_canary)
      : "r16", "r17", "r18"
      : return_point, stack_error);

stack_error:
  FATAL_ERROR("Stack integrity check failed!");
return_point:
  return;
}

void verify_enable_disable_mt_macros(void) {
  // Setup
  const uint8_t original_timsk1_content = TIMSK1;
  // Test
  TIMSK1 = 0;
  SCILENT_ENABLE_MT();
  ASSERT_EQ(TIMSK1, 1 << OCIE1A, "0x%X",
            "Unexpeced status of TIMSK1 register after enabling MT.");

  SCILENT_DISABLE_MT();
  ASSERT_EQ(TIMSK1, 0, "0x%X",
            "Unexpeced status of TIMSK1 register after disabling MT.");

  TIMSK1 = 0xFF;
  SCILENT_DISABLE_MT();
  ASSERT_EQ(TIMSK1, 0xFF ^ 1 << OCIE1A, "0x%X",
            "Unexpeced status of TIMSK1 register after disabling MT.");

  TIMSK1 = original_timsk1_content;  // Reset
}

void pre_scheduler_self_checks(void) {
  cprint("Running pre sheduler tests...\n");

  run_with_stack_integrity_check(verify_enable_disable_mt_macros);

  cprint("Pre scheduler self checks ... PASSED\n");
}
