#include "../hal/context_switch_macros.h"
#include "../hal/hal.h"
#include "test.h"
#include "test_suite.h"
/*
 * Pushes three canaries (0xAA) onto the stack. See CHECK_STACK_CANARIES.
 * */
#define SETUP_STACK_CANARIES() \
  asm volatile(                \
      "push r16         \n\t"  \
      "push r17         \n\t"  \
      "push r26         \n\t"  \
      "clr r16          \n\t"  \
      "subi r16, -0xAA  \n\t"  \
      "clr r17          \n\t"  \
      "subi r17, -0xAA  \n\t"  \
      "clr r26          \n\t"  \
      "subi r26, -0xAA  \n\t"  \
      "push r16         \n\t"  \
      "push r17         \n\t"  \
      "push r26         \n\t"  \
      : /* No outputs*/        \
      : /* No inputs*/         \
      : "memory");
/*
 * Complements SETUP_STACK_CANARIES. Reads three canary bytes from the
 * stack. Places a non-zero value in num_failuress if the read bytes do not
 * match the expected canary values. Otherwise, num_failures is set to zero.
 * */
#define CHECK_STACK_CANARIES(num_failures) \
  asm volatile(                            \
      "pop r26                      \n\t"  \
      "pop r17                      \n\t"  \
      "pop r16                      \n\t"  \
      "clr %0                       \n\t"  \
      "cpse r26, r17                \n\t"  \
      "inc %0                       \n\t"  \
      "cpse r26, r16                \n\t"  \
      "inc %0                       \n\t"  \
      "clr r16                      \n\t"  \
      "subi r16, -0xAA              \n\t"  \
      "cpse r26, r16                \n\t"  \
      "inc %0                       \n\t"  \
      "pop 26                       \n\t"  \
      "pop r17                      \n\t"  \
      "pop r16                      \n\t"  \
      : "+r"(num_failures)                 \
      : /* No (pure) inputs */             \
      : "memory");

// Tests the test helpers
bool test_setup_and_check_of_stack_canaries_do_not_trigger_when_called_directly(
    void) {
  uint8_t num_failiures = 0;
  SETUP_STACK_CANARIES();
  CHECK_STACK_CANARIES(num_failiures);

  return num_failiures == 0;
  // Tests the test helpers
}

bool test_setup_and_check_of_stack_canaries_detect_additional_bytes_pushed(
    void) {
  uint8_t num_failiures = 0;
  SETUP_STACK_CANARIES();
  asm volatile("push r18" ::: "r18");
  CHECK_STACK_CANARIES(num_failiures);

  asm volatile("pop r18" ::
                   : "r16", "r17", "r26",
                     "memory");  // reset the offset from the pushed register
  return num_failiures != 0;
}

bool test_setup_and_check_of_stack_canaries_detect_changes_to_first_canary_byte(
    void) {
  // First canary byte
  uint8_t num_failiures = 0;

  SETUP_STACK_CANARIES();
  asm volatile(
      "pop r18         \n\t"
      "subi r18,1      \n\t"
      "push r18        \n\t"
      : /* No outputs*/
      :);
  CHECK_STACK_CANARIES(num_failiures);
  asm volatile("" ::: "r18");
  return num_failiures != 0;
}

bool test_setup_and_check_of_stack_canaries_detect_changes_to_second_canary_byte(
    void) {
  uint8_t num_failiures = 0;
  SETUP_STACK_CANARIES();
  asm volatile(
      "pop r18         \n\t"
      "pop r18         \n\t"
      "subi r18,1      \n\t"
      "push r18        \n\t"
      "push r18        \n\t"
      : /* No outputs*/
      :);
  CHECK_STACK_CANARIES(num_failiures);
  asm volatile("" ::: "r18");
  return num_failiures != 0;
}
bool test_setup_and_check_of_stack_canaries_detect_changes_to_third_canary_byte(
    void) {
  uint8_t num_failiures = 0;
  SETUP_STACK_CANARIES();
  asm volatile(
      "pop r18         \n\t"
      "pop r18         \n\t"
      "pop r18         \n\t"
      "subi r18,1      \n\t"
      "push r18        \n\t"
      "push r18        \n\t"
      "push r18        \n\t"
      : /* No outputs*/
      :);
  CHECK_STACK_CANARIES(num_failiures);
  asm volatile("" ::: "r18");
  return num_failiures != 0;
}

bool test_enable_mt_sets_enable_bit_for_tmsk1(void) {
  uint8_t num_failiures = 0;
  SETUP_STACK_CANARIES();

  const uint8_t original_timsk1_content = TIMSK1;
  TIMSK1 = 0;
  SCILENT_ENABLE_MT();
  CHECK_EQ(TIMSK1, 1 << OCIE1A, "0x%X");

  TIMSK1 = original_timsk1_content;
  CHECK_STACK_CANARIES(num_failiures);
  CHECK_EQ(num_failiures, 0, "%u");
  return TEST_PASSED;
}

bool test_disable_mt_clears_enable_bit_for_tmsk1(void) {
  uint8_t num_failiures = 0;
  SETUP_STACK_CANARIES();

  const uint8_t original_timsk1_content = TIMSK1;
  TIMSK1 = 0;
  SCILENT_ENABLE_MT();
  CHECK_EQ(TIMSK1, 1 << OCIE1A, "0x%X");
  SCILENT_DISABLE_MT();
  CHECK_EQ(TIMSK1, 0, "0x%X");
  TIMSK1 = 0xFF;
  SCILENT_DISABLE_MT();
  CHECK_EQ(TIMSK1, 0xFF ^ 1 << OCIE1A, "0x%X");
  TIMSK1 = original_timsk1_content;

  CHECK_STACK_CANARIES(num_failiures);
  return TEST_PASSED;
}
bool test_select_scheduled_task_or_reschedule_selecte_schedled_task_when_available(
    void) {
  const uint16_t expected_scheduled_task_sp = 0x1234;
  task_sp = 0xFFFF;
  next_task_sp = expected_scheduled_task_sp;

  SELECT_SCHEDULED_TASK_OR_SCHEDULER();

  CHECK_EQ(task_sp, expected_scheduled_task_sp, "0x%X");
  return true;
}

bool test_select_scheduled_task_or_reschedule_does_not_modify_scheduler_task_when_task_is_scheduled(
    void) {
  const uint16_t expected_scheduler_task_sp = 0xABCD;
  scheduler_task_sp = expected_scheduler_task_sp;
  next_task_sp = 0x1111;  // anything not 0

  SELECT_SCHEDULED_TASK_OR_SCHEDULER();

  CHECK_EQ(scheduler_task_sp, expected_scheduler_task_sp, "0x%X");
  return true;
}

bool test_select_scheduled_task_or_reschedule_clears_next_task_sp_when_sheduled_task_is_selected(
    void) {
  const uint16_t expected_scheduler_task_sp = 0xABCD;
  scheduler_task_sp = expected_scheduler_task_sp;
  next_task_sp = 0x1111;  // anything not 0

  SELECT_SCHEDULED_TASK_OR_SCHEDULER();

  CHECK_EQ(next_task_sp, 0, "0x%X");
  return true;
}

bool test_select_scheduled_task_or_reschedule_selects_scheduler_task_when_no_task_is_scheduled(
    void) {
  const uint16_t expected_scheduler_task_sp = 0xABCD;
  scheduler_task_sp = expected_scheduler_task_sp;
  next_task_sp = 0;  // no task scheduled

  SELECT_SCHEDULED_TASK_OR_SCHEDULER();

  CHECK_EQ(scheduler_task_sp, expected_scheduler_task_sp, "0x%X");
  CHECK_EQ(task_sp, expected_scheduler_task_sp, "0x%X");
  return true;
}

bool test_stack_variable_is_kept_after_saving_and_restoring_context(void) {
  const uint32_t given_data = 0xAAAAAAAAU;
  volatile uint32_t on_stack_variable = given_data;

  SAVE_CONTEXT();
  RESTORE_CONTEXT();
  // Stack and registers should be the same as before.

  CHECK_EQ(on_stack_variable, given_data, "0x%u");

  return true;
}

struct TestStatistics unit_test_context_switch(void) {
  struct TestStatistics results = {};

  RUN_TEST(test_disable_mt_clears_enable_bit_for_tmsk1, &results);

  RUN_TEST(
      test_select_scheduled_task_or_reschedule_selecte_schedled_task_when_available,
      &results);

  RUN_TEST(
      test_select_scheduled_task_or_reschedule_does_not_modify_scheduler_task_when_task_is_scheduled,
      &results);

  RUN_TEST(
      test_select_scheduled_task_or_reschedule_clears_next_task_sp_when_sheduled_task_is_selected,
      &results);

  RUN_TEST(
      test_select_scheduled_task_or_reschedule_selects_scheduler_task_when_no_task_is_scheduled,
      &results);

  RUN_TEST(
      test_setup_and_check_of_stack_canaries_do_not_trigger_when_called_directly,
      &results);

  RUN_TEST(
      test_setup_and_check_of_stack_canaries_detect_additional_bytes_pushed,
      &results);

  RUN_TEST(
      test_setup_and_check_of_stack_canaries_detect_changes_to_first_canary_byte,
      &results);

  RUN_TEST(
      test_setup_and_check_of_stack_canaries_detect_changes_to_second_canary_byte,
      &results);

  RUN_TEST(
      test_setup_and_check_of_stack_canaries_detect_changes_to_third_canary_byte,
      &results);

  RUN_TEST(test_stack_variable_is_kept_after_saving_and_restoring_context,
           &results);

  return results;
}
