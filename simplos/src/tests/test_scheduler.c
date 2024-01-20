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

bool test_get_active_tasks_does_not_find_any_active_tasks_amongs_those_whos_status_is_not_ready(
    void) {
  Kernel_type given_kernel = {0};
  Scheduler *schedule_with_no_active_tasks = &given_kernel.schedule;

  // Cover all non-ready task types
  schedule_with_no_active_tasks->queue.tasks[0].status = SLEEPING;
  schedule_with_no_active_tasks->queue.tasks[1].status = RUNNING;
  schedule_with_no_active_tasks->queue.tasks[2].status = EMPTY;
  schedule_with_no_active_tasks->queue.tasks[3].status = SCHEDULING;
  schedule_with_no_active_tasks->queue.tasks[4].status = PAUSED_SCHEDULER;

  const uint8_t recived = get_active_tasks_mask(&given_kernel);

  CHECK_EQ(recived, 0U, "0x%X");

  return TEST_PASSED;
}

bool test_get_active_tasks_counts_all_tasks_marked_as_ready_as_active(void) {
  Kernel_type given_kernel = {0};
  Scheduler *schedule_with_three_active_tasks = &given_kernel.schedule;

  schedule_with_three_active_tasks->queue.tasks[0].status = SLEEPING;
  schedule_with_three_active_tasks->queue.tasks[1].status = READY;
  schedule_with_three_active_tasks->queue.tasks[2].status = EMPTY;
  schedule_with_three_active_tasks->queue.tasks[3].status = READY;
  schedule_with_three_active_tasks->queue.tasks[4].status = READY;

  const uint8_t recived = get_active_tasks_mask(&given_kernel);

  CHECK_EQ(recived, 0x1A, "0x%X");

  return TEST_PASSED;
}

bool test_prioritze_tasks_prioritizes_the_tasks_in_ascending_order(void) {
  Kernel_type given_kernel = {0};
  Scheduler *schedule_with_three_active_tasks = &given_kernel.schedule;

  schedule_with_three_active_tasks->queue.tasks[0].priority = 4;
  schedule_with_three_active_tasks->queue.tasks[1].priority = 0;
  schedule_with_three_active_tasks->queue.tasks[2].priority = 1;
  schedule_with_three_active_tasks->queue.tasks[3].priority = 10;
  schedule_with_three_active_tasks->queue.tasks[4].priority = 7;

  uint8_t prioritized_task_indices[5] = {0};

  prioritize_tasks(schedule_with_three_active_tasks->queue.tasks, 0x1F,
                   prioritized_task_indices);

  // expected_index_order: {1,2, 0, 4, 3};
  CHECK_EQ(prioritized_task_indices[0], 1U, "%u");
  CHECK_EQ(prioritized_task_indices[1], 2U, "%u");
  CHECK_EQ(prioritized_task_indices[2], 0U, "%u");
  CHECK_EQ(prioritized_task_indices[3], 4U, "%u");
  CHECK_EQ(prioritized_task_indices[4], 3U, "%u");

  return TEST_PASSED;
}

bool test_prioritze_tasks_only_consideres_the_tasks_specifed(void) {
  Kernel_type given_kernel = {0};
  Scheduler *schedule_with_three_active_tasks = &given_kernel.schedule;

  schedule_with_three_active_tasks->queue.tasks[0].priority = 4;  // Ignored
  schedule_with_three_active_tasks->queue.tasks[1].priority = 0;
  schedule_with_three_active_tasks->queue.tasks[2].priority = 1;  // Ignored
  schedule_with_three_active_tasks->queue.tasks[3].priority = 10;
  schedule_with_three_active_tasks->queue.tasks[4].priority = 7;

  uint8_t prioritized_task_indices[5] = {0, 0, 0, 0xFF, 0xAA};

  prioritize_tasks(schedule_with_three_active_tasks->queue.tasks, 0x1A,
                   prioritized_task_indices);

  // expected_index_order: {1,2, 0, 4, 3};
  CHECK_EQ(prioritized_task_indices[0], 1, "%u");
  CHECK_EQ(prioritized_task_indices[1], 4U, "%u");
  CHECK_EQ(prioritized_task_indices[2], 3U, "%u");
  CHECK_EQ(prioritized_task_indices[3], 0xFF, "%u");
  CHECK_EQ(prioritized_task_indices[4], 0xAA, "%u");

  return TEST_PASSED;
}

bool test_reschedule_prioritizes_the_active_tasks_in_ascending_order_of_priority(
    void) {
  Kernel_type given_kernel = {0};
  Scheduler *schedule_with_three_active_tasks = &given_kernel.schedule;

  schedule_with_three_active_tasks->queue.tasks[0].priority = 4;
  schedule_with_three_active_tasks->queue.tasks[1].priority = 0;  // A
  schedule_with_three_active_tasks->queue.tasks[2].priority = 1;
  schedule_with_three_active_tasks->queue.tasks[3].priority = 10;  // C
  schedule_with_three_active_tasks->queue.tasks[4].priority = 7;   // B

  schedule_with_three_active_tasks->queue.tasks[0].status = PAUSED_SCHEDULER;
  schedule_with_three_active_tasks->queue.tasks[1].status = READY;  // A
  schedule_with_three_active_tasks->queue.tasks[2].status = EMPTY;
  schedule_with_three_active_tasks->queue.tasks[3].status = READY;  // C
  schedule_with_three_active_tasks->queue.tasks[4].status = READY;  // B

  // Should not be touched.
  given_kernel.schedule.queue.task_index_queue[3] = (uint8_t)0xAA;
  given_kernel.schedule.queue.task_index_queue[4] = (uint8_t)0xFF;

  reschedule(&given_kernel);

  CHECK_EQ(given_kernel.schedule.queue.task_index_queue[0], 1U, "%u");  // A
  CHECK_EQ(given_kernel.schedule.queue.task_index_queue[1], 4U, "%u");  // B
  CHECK_EQ(given_kernel.schedule.queue.task_index_queue[2], 3U, "%u");  // C
  CHECK_EQ(given_kernel.schedule.queue.task_index_queue[3], 0xAA, "%u");
  CHECK_EQ(given_kernel.schedule.queue.task_index_queue[4], 0xFF, "%u");

  CHECK_EQ(given_kernel.schedule.queue.queue_position, 2,
           "%u");  // Three tasks sheduled

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
      test_get_active_tasks_does_not_find_any_active_tasks_amongs_those_whos_status_is_not_ready,
      &results);

  RUN_TEST(test_get_active_tasks_counts_all_tasks_marked_as_ready_as_active,
           &results);

  RUN_TEST(test_prioritze_tasks_prioritizes_the_tasks_in_ascending_order,
           &results);

  RUN_TEST(test_prioritze_tasks_only_consideres_the_tasks_specifed, &results);

  RUN_TEST(
      test_reschedule_prioritizes_the_active_tasks_in_ascending_order_of_priority,
      &results);
  return results;
}
