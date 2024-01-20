#include "../hal/context_switch_macros.h"
#include "test.h"
#include "test_suite.h"

#define SETUP_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL() \
  asm volatile(                                        \
      "push r16         \n\t"                          \
      "push r17         \n\t"                          \
      "push r26         \n\t"                          \
      "push r27         \n\t"                          \
      "push r28         \n\t"                          \
      "push r29         \n\t"                          \
      : /* No outputs*/                                \
      :);

#define TEARDOWN_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL() \
  asm volatile(                                           \
      "pop r29         \n\t"                              \
      "pop r28         \n\t"                              \
      "pop r27         \n\t"                              \
      "pop r26         \n\t"                              \
      "pop r17         \n\t"                              \
      "pop r16         \n\t"                              \
      : /* No outputs*/                                   \
      :);

bool test_select_scheduled_task_or_reschedule_selecte_schedled_task_when_available(
    void) {
  const uint16_t expected_scheduled_task_sp = 0x1234;
  task_sp = 0xFFFF;
  next_task_sp = expected_scheduled_task_sp;

  SETUP_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL();
  SELECT_SCHEDULED_TASK_OR_SCHEDULER();
  TEARDOWN_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL();

  CHECK_EQ(task_sp, expected_scheduled_task_sp, "0x%X");
  return true;
}

bool test_select_scheduled_task_or_reschedule_does_not_modify_scheduler_task_when_task_is_scheduled(
    void) {
  const uint16_t expected_scheduler_task_sp = 0xABCD;
  scheduler_task_sp = expected_scheduler_task_sp;
  next_task_sp = 0x1111;  // anything not 0

  SETUP_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL();
  SELECT_SCHEDULED_TASK_OR_SCHEDULER();
  TEARDOWN_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL();

  CHECK_EQ(scheduler_task_sp, expected_scheduler_task_sp, "0x%X");
  return true;
}

bool test_select_scheduled_task_or_reschedule_clears_next_task_sp_when_sheduled_task_is_selected(
    void) {
  const uint16_t expected_scheduler_task_sp = 0xABCD;
  scheduler_task_sp = expected_scheduler_task_sp;
  next_task_sp = 0x1111;  // anything not 0

  SETUP_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL();
  SELECT_SCHEDULED_TASK_OR_SCHEDULER();
  TEARDOWN_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL();

  CHECK_EQ(next_task_sp, 0, "0x%X");
  return true;
}

bool test_select_scheduled_task_or_reschedule_selects_scheduler_task_when_no_task_is_scheduled(
    void) {
  const uint16_t expected_scheduler_task_sp = 0xABCD;
  scheduler_task_sp = expected_scheduler_task_sp;
  next_task_sp = 0;  // no task scheduled

  SETUP_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL();
  SELECT_SCHEDULED_TASK_OR_SCHEDULER();
  TEARDOWN_SELECT_SCHEDULD_TASK_OR_SCHEDULER_CALL();

  CHECK_EQ(scheduler_task_sp, expected_scheduler_task_sp, "0x%X");
  CHECK_EQ(task_sp, expected_scheduler_task_sp, "0x%X");
  return true;
}

struct TestStatistics unit_test_context_switch(void) {
  struct TestStatistics results = {0};

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

  return results;
}
