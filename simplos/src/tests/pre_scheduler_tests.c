#include <stdint.h>

#include "../io_helpers.h"
#include "../simplos.h"
#include "../timers.h"
#include "test.h"

void verify_enable_disable_mt_macros(void) {
  // Setup
  const uint8_t original_timsk1_content = TIMSK1;
  cli();

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

  // Teardown
  TIMSK1 = original_timsk1_content;
  sei();
}

void verify_set_sp_to_os_sp_macro(void) {
  volatile uint16_t old_sp = 0, new_sp = 0;
  old_sp = SP;

  SET_SP_TO_OS_TASK_SP();
  new_sp = SP;

  ASSERT_EQ(new_sp, OS_STACK_START, "0x%X",
            "Unexpected new stack pointer after SET_SP_TO_OS_TASK");

  SP = old_sp;  // reset
}

void pre_scheduler_self_checks(void) {
  verify_enable_disable_mt_macros();
  verify_set_sp_to_os_sp_macro();
  cprint("Pre scheduler self checks ... PASSED\n");
}
