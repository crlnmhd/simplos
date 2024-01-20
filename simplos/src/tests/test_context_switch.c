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

struct TestStatistics unit_test_context_switch(void) {
  struct TestStatistics results = {0};

  RUN_TEST(
      test_select_scheduled_task_or_reschedule_selecte_schedled_task_when_available,
      &results);
  return results;
}
