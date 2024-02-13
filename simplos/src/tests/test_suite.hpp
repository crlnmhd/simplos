#ifndef TEST_SUITE_H_
#define TEST_SUITE_H_

#include "test.hpp"

TestStatistics unit_test_context_switch(void);

TestStatistics unit_test_scheduler(void);

TestStatistics unit_test_spawning(void);

TestStatistics unit_test_hal_log(void);

TestStatistics unit_test_yields(void);

TestStatistics unit_test_memory(void);

TestStatistics unit_test_os(void);

TestStatistics unit_test_tasks(void);

TestStatistics unit_test_kernel_status(void);

TestStatistics unit_test_data_structures(void);

#endif  // TEST_SUITE_H_
