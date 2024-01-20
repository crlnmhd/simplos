#include "../hal/hal.h"
#include "../scheduler.h"
#include "test.h"
#include "test_suite.h"

bool test_select_next_task_sets_queue_positio_to_next_task_when_tasks_are_available(
    void) {
  Kernel_type given_kernel = {0};
  given_kernel.schedule.queue.queue_position = 1;
  const uint8_t expected_queue_position = 0;

  select_next_task(&given_kernel);
  CHECK_EQ(given_kernel.schedule.queue.queue_position, expected_queue_position,
           "%u");
  return true;
}

bool test_select_next_task_sets_next_task_sp_to_next_available_task(void) {
  next_task_sp = 0xFFFF;
  Kernel_type given_kernel = {0};
  given_kernel.schedule.queue.queue_position = 1;
  const uint8_t expected_queue_position = 0;

  const uint8_t given_index_of_queued_task = 3;
  given_kernel.schedule.queue.task_index_queue[expected_queue_position] =
      given_index_of_queued_task;
  const uint16_t expected_scheduled_task_sp_adr = 0x1234;
  given_kernel.schedule.queue.tasks[given_index_of_queued_task].task_sp_adr =
      expected_scheduled_task_sp_adr;

  select_next_task(&given_kernel);
  CHECK_EQ(next_task_sp, expected_scheduled_task_sp_adr, "0x%X");
  return true;
}

bool test_get_num_active_tasks_does_not_find_any_active_tasks_amongs_those_whos_status_is_not_ready(
    void) {
  Kernel_type given_kernel = {0};
  Scheduler *schedule_with_no_active_tasks = &given_kernel.schedule;

  // Cover all non-ready task types
  schedule_with_no_active_tasks->queue.tasks[0].status = SLEEPING;
  schedule_with_no_active_tasks->queue.tasks[1].status = RUNNING;
  schedule_with_no_active_tasks->queue.tasks[2].status = EMPTY;
  schedule_with_no_active_tasks->queue.tasks[3].status = SCHEDULING;
  schedule_with_no_active_tasks->queue.tasks[4].status = PAUSED_SCHEDULER;

  uint8_t prioritized_task_indices[5] = {0};

  const uint8_t recived =
      get_active_tasks(prioritized_task_indices, 5, &given_kernel);

  CHECK_EQ(recived, 0U, "%u");

  return TEST_PASSED;
}

struct TestStatistics unit_test_scheduler(void) {
  struct TestStatistics results = {0};
  RUN_TEST(
      test_select_next_task_sets_queue_positio_to_next_task_when_tasks_are_available,
      &results);

  RUN_TEST(test_select_next_task_sets_next_task_sp_to_next_available_task,
           &results);

  RUN_TEST(
      test_get_num_active_tasks_does_not_find_any_active_tasks_amongs_those_whos_status_is_not_ready,
      &results);

  return results;
}
