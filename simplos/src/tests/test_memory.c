#include "../memory.h"
#include "test.h"

volatile uint16_t at_task_sp __attribute__((section(".task_sp_location"))) = 0;
volatile uint16_t at_scheduler_task_sp
    __attribute__((section(".scheduler_task_sp_location"))) = 0;
volatile uint16_t at_next_task_sp
    __attribute__((section(".next_task_sp_location"))) = 0;
volatile uint16_t at_prev_task_sp
    __attribute__((section(".prev_task_sp_location"))) = 0;

volatile Kernel at_kernel_location
    __attribute__((section(".kernel_data_location"))) = {0};

bool test_location_of_task_sp_is_readable_at_0x2100_to_0x2101(void) {
  at_task_sp = 0x0123;
  CHECK_EQ((uint16_t)(&at_task_sp), (uint16_t)0x2100, "0x%X");
  CHECK_EQ(*(uint8_t *)(0x2100), 0x23, "0x%X");
  CHECK_EQ(*(uint8_t *)(0x2101), 0x01, "0x%X");
  return TEST_PASSED;
}
bool test_location_of_scheduler_task_sp_is_readable_at_0x20FE_to_0x20FF(void) {
  at_scheduler_task_sp = 0x4567;
  CHECK_EQ((uint16_t)(&at_scheduler_task_sp), (uint16_t)0x20FE, "0x%X");
  CHECK_EQ(*(uint8_t *)(0x20FE), 0x67, "0x%X");
  CHECK_EQ(*(uint8_t *)(0x20FF), 0x45, "0x%X");
  return TEST_PASSED;
}
bool test_location_of_next_task_sp_is_readable_at_0x20FC_to_0x20FD(void) {
  at_next_task_sp = 0x89AB;
  CHECK_EQ((uint16_t)(&at_next_task_sp), (uint16_t)0x20FC, "0x%X");
  CHECK_EQ(*(uint8_t *)(0x20FC), 0xAB, "0x%X");
  CHECK_EQ(*(uint8_t *)(0x20FD), 0x89, "0x%X");
  return TEST_PASSED;
}
bool test_location_of_prev_task_sp_is_readable_at_0x20FA_to_0x20FB(void) {
  at_prev_task_sp = 0xCDEF;
  CHECK_EQ((uint16_t)(&at_prev_task_sp), (uint16_t)0x20FA, "0x%X");
  CHECK_EQ(*(uint8_t *)(0x20FA), 0xEF, "0x%X");
  CHECK_EQ(*(uint8_t *)(0x20FB), 0xCD, "0x%X");
  return TEST_PASSED;
}
bool test_location_of_kernel_data_starts_at_0x2102(void) {
  /*
   * Note: this order of elements in the struct is not what is important here,
   * just that the kernel data is located where it's supposed to be.
   * */
  const uint32_t given_first_data_in_struct = 0x12345678;
  at_kernel_location.schedule.queue.tasks[0].time_counter =
      given_first_data_in_struct;
  CHECK_EQ((uint16_t)(&at_kernel_location), (uint16_t)0x2102, "0x%X");
  CHECK_EQ(*(uint32_t *)(0x2102), given_first_data_in_struct, "0x%X");
  return TEST_PASSED;
}
bool test_size_of_kernel_does_not_lead_to_stack_overflow(void) {
  const uint16_t start_adr = 0x2102;  // see other test

  const uint16_t next_section = 0x21FF;  // End of SRAM
  CHECK_TRUE(start_adr + sizeof(Kernel) < next_section);
  return TEST_PASSED;
}

bool test_task_zero_low_is_at_end_of_availabel_task_ram(void) {
  CHECK_EQ(task_sp_range_low(0), TASK_RAM_END, "0x%X");

  return TEST_PASSED;
}
bool test_tasks_are_allocated_TASKS_MEMORY_SIZE_bytes_stack(void) {
  CHECK_EQ((uint16_t)(task_sp_range_high(0U) - task_sp_range_low(0U)),
           (uint16_t)TASK_MEMORY_SIZE, "0x%X");

  CHECK_EQ((uint16_t)(task_sp_range_high(1U) - task_sp_range_low(1U)),
           (uint16_t)TASK_MEMORY_SIZE, "0x%X");

  CHECK_EQ((uint16_t)(task_sp_range_high(2U) - task_sp_range_low(2U)),
           (uint16_t)TASK_MEMORY_SIZE, "0x%X");

  CHECK_EQ((uint16_t)(task_sp_range_high(3U) - task_sp_range_low(3U)),
           (uint16_t)TASK_MEMORY_SIZE, "0x%X");

  CHECK_EQ((uint16_t)(task_sp_range_high(4U) - task_sp_range_low(4U)),
           (uint16_t)TASK_MEMORY_SIZE, "0x%X");
  return TEST_PASSED;
}


struct TestStatistics unit_test_memory(void) {
  struct TestStatistics results = {0};
  RUN_TEST(test_location_of_task_sp_is_readable_at_0x2100_to_0x2101, &results);

  RUN_TEST(test_location_of_scheduler_task_sp_is_readable_at_0x20FE_to_0x20FF,
           &results);

  RUN_TEST(test_location_of_next_task_sp_is_readable_at_0x20FC_to_0x20FD,
           &results);

  RUN_TEST(test_location_of_prev_task_sp_is_readable_at_0x20FA_to_0x20FB,
           &results);

  RUN_TEST(test_location_of_kernel_data_starts_at_0x2102, &results);

  RUN_TEST(test_size_of_kernel_does_not_lead_to_stack_overflow, &results);

  RUN_TEST(test_task_zero_low_is_at_end_of_availabel_task_ram, &results);

  RUN_TEST(test_tasks_are_allocated_TASKS_MEMORY_SIZE_bytes_stack, &results);

  return results;
}
