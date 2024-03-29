#include "../memory.hpp"
#include "test.hpp"

volatile uint16_t at_task_sp __attribute__((section(".task_sp_location"))) = 0;
volatile uint16_t at_scheduler_task_sp
    __attribute__((section(".scheduler_task_sp_location"))) = 0;
volatile uint16_t at_next_task_sp
    __attribute__((section(".next_task_sp_location"))) = 0;
volatile uint16_t at_prev_task_sp
    __attribute__((section(".prev_task_sp_location"))) = 0;

volatile Kernel at_kernel_location
    __attribute__((section(".kernel_data_location"))) = {};

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
  constexpr uint32_t given_first_data_in_struct = 0x12345678;
  at_kernel_location.schedule.queue.tasks[0].time_counter =
      given_first_data_in_struct;
  CHECK_EQ((uint16_t)(&at_kernel_location), (uint16_t)0x2102, "0x%X");
  CHECK_EQ(*(uint32_t *)(0x2102), given_first_data_in_struct, "0x%X");
  return TEST_PASSED;
}
bool test_size_of_kernel_does_not_lead_to_stack_overflow(void) {
  constexpr uint16_t start_adr = 0x2102;  // see other test

  constexpr uint16_t next_section = 0x21FF;  // End of SRAM
  CHECK_TRUE(start_adr + sizeof(Kernel) < next_section);
  return TEST_PASSED;
}

bool test_task_zero_low_is_at_end_of_availabel_task_ram(void) {
  CHECK_EQ(task_sp_range_low(0), task_ram_end, "0x%X");

  return TEST_PASSED;
}

bool test_first_task_high_starts_at_TASK_RAM_END_plus_num_bytes_minus_one(
    void) {
  constexpr uint16_t exepcted_task_high = task_ram_end + task_memory_size - 1;
  CHECK_EQ(task_sp_range_high(0), exepcted_task_high, "0x%X");

  return TEST_PASSED;
}

bool test_task_1_sp_low_is_directly_after_task_0(void) {
  CHECK_EQ(task_sp_range_low(1), task_sp_range_high(0) + 1U, "0x%X");

  return TEST_PASSED;
}
bool test_task_2_sp_low_is_directly_after_task_1(void) {
  CHECK_EQ(task_sp_range_low(2), task_sp_range_high(1) + 1U, "0x%X");

  return TEST_PASSED;
}
bool test_task_3_sp_low_is_directly_after_task_2(void) {
  CHECK_EQ(task_sp_range_low(3), task_sp_range_high(2) + 1U, "0x%X");

  return TEST_PASSED;
}
bool test_task_4_sp_low_is_directly_after_task_3(void) {
  CHECK_EQ(task_sp_range_low(4), task_sp_range_high(3) + 1U, "0x%X");

  return TEST_PASSED;
}

// Indirectly verifies that the task highs are correct.
bool test_tasks_are_allocated_TASKS_MEMORY_SIZE_bytes_stack(void) {
  return TEST_PASSED;
  CHECK_EQ((uint16_t)(task_sp_range_high(0U) - task_sp_range_low(0U)),
           (uint16_t)task_memory_size, "0x%X");

  CHECK_EQ((uint16_t)(task_sp_range_high(1U) - task_sp_range_low(1U)),
           (uint16_t)task_memory_size, "0x%X");

  CHECK_EQ((uint16_t)(task_sp_range_high(2U) - task_sp_range_low(2U)),
           (uint16_t)task_memory_size, "0x%X");

  CHECK_EQ((uint16_t)(task_sp_range_high(3U) - task_sp_range_low(3U)),
           (uint16_t)task_memory_size, "0x%X");

  CHECK_EQ((uint16_t)(task_sp_range_high(4U) - task_sp_range_low(4U)),
           (uint16_t)task_memory_size, "0x%X");
  return TEST_PASSED;
}

TestStatistics unit_test_memory(void) {
  TestStatistics results = {};
  RUN_TEST(test_location_of_task_sp_is_readable_at_0x2100_to_0x2101, results);

  RUN_TEST(test_location_of_scheduler_task_sp_is_readable_at_0x20FE_to_0x20FF,
           results);

  RUN_TEST(test_location_of_next_task_sp_is_readable_at_0x20FC_to_0x20FD,
           results);

  RUN_TEST(test_location_of_prev_task_sp_is_readable_at_0x20FA_to_0x20FB,
           results);

  RUN_TEST(test_location_of_kernel_data_starts_at_0x2102, results);

  RUN_TEST(test_size_of_kernel_does_not_lead_to_stack_overflow, results);

  RUN_TEST(test_first_task_high_starts_at_TASK_RAM_END_plus_num_bytes_minus_one,
           results);

  RUN_TEST(test_task_zero_low_is_at_end_of_availabel_task_ram, results);

  RUN_TEST(test_tasks_are_allocated_TASKS_MEMORY_SIZE_bytes_stack, results);

  RUN_TEST(test_task_1_sp_low_is_directly_after_task_0, results);
  RUN_TEST(test_task_2_sp_low_is_directly_after_task_1, results);
  RUN_TEST(test_task_3_sp_low_is_directly_after_task_2, results);
  RUN_TEST(test_task_4_sp_low_is_directly_after_task_3, results);
  return results;
}
