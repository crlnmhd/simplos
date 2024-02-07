#ifndef TEST_SUITE_H_
#define TEST_SUITE_H_

#include "test.hpp"

struct TestStatistics unit_test_context_switch(void);

struct TestStatistics unit_test_scheduler(void);

struct TestStatistics unit_test_spawning(void);

struct TestStatistics unit_test_hal_log(void);

struct TestStatistics unit_test_yields(void);

struct TestStatistics unit_test_memory(void);

struct TestStatistics unit_test_os(void);

#endif  // TEST_SUITE_H_
