#include <stdint.h>

#include "../io_helpers.h"
#include "../simplos.h"
#include "../timers.h"
#include "test.h"

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

void verify_set_sp_to_os_sp_macro(void) {
  volatile uint16_t old_sp = 0, new_sp = 0;
  old_sp = SP;

  SET_SP_TO_OS_TASK_SP();
  new_sp = SP;

  ASSERT_EQ(new_sp, OS_STACK_START, "0x%X",
            "Unexpected new stack pointer after SET_SP_TO_OS_TASK");

  SP = old_sp;  // reset
}

void verify_save_sp_macro(void) {
  const uint16_t old_task_sp = task_sp;
  ASSERT(old_task_sp != SP,
         "Bad test setup. Expected task_sp not to be set to SP");

  SAVE_SP();

  ASSERT_EQ(task_sp, SP, "0x%X",
            "SAVE_SP macro failed to save stack pointer to task_sp variable");

  task_sp = old_task_sp;  // reset
}

void verify_set_sp_macro(void) {
  const uint16_t old_sp = SP;
  const uint16_t old_task_sp = task_sp;
  const uint16_t expected_new_sp = 0x123;

  task_sp = expected_new_sp;
  SET_SP();
  ASSERT_EQ(SP, expected_new_sp, "0x%X", "Failed to set SP using SET_SP macro");
  ASSERT_EQ(task_sp, expected_new_sp, "0x%X",
            "task_sp has changed unexpectedly");

  task_sp = old_sp;
  SET_SP();
  ASSERT_EQ(SP, old_sp, "0x%X", "Failed to reset stack pointer");
  ASSERT_EQ(task_sp, old_sp, "0x%X", "Failed to reset task_sp");

  task_sp = old_task_sp;  // Reset
}

void pre_scheduler_self_checks(void) {
  verify_enable_disable_mt_macros();
  verify_set_sp_to_os_sp_macro();
  verify_save_sp_macro();
  verify_set_sp_macro();
  cprint("Pre scheduler self checks ... PASSED\n");
}
