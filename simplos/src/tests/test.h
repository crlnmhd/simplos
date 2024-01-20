#if !defined(TESTS_H_)
#define TESTS_H_
#include <avr/pgmspace.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "../io_helpers.h"
#include "../os.h"

struct TestStatistics {
  uint8_t failed;
  uint8_t passed;
  uint8_t skipped;
};

void combine_statistics(struct TestStatistics *statistics,
                        struct TestStatistics *to_add);

// todo: linked list that can hold commands?
struct HardwareState {
  bool interrupt_enabled;
  bool halted;
};

extern struct HardwareState state;

#define SKIP_TEST(test_fn, function_name, test_statistics)       \
  test_statistics.skipped += 1;                                  \
  /* This is the least of the security concerns, so let it be.*/ \
  printf_P(PSTR(function_name));

void run_test_function(bool (*fn_ptr)(void), PGM_P function_name,
                       struct TestStatistics statistics);

#define TEST_ASSERT(cond, msg)                \
  if (!(bool)(cond)) {                        \
    uint8_t sreg = SREG;                      \
    disable_interrupts();                     \
    print_from_prg_mem(" FAILED!\n");         \
    print_from_prg_mem("ASSERTION FAILED! "); \
    print_from_prg_mem(msg);                  \
    print_from_prg_mem("\n");                 \
    SREG = sreg;                              \
    return false;                             \
  }

#define TEST_ASSERT_EQ(recieved, expected, fmt, msg) \
  if ((expected) != (recieved)) {                    \
    uint8_t sreg = SREG;                             \
    disable_interrupts();                            \
    print_from_prg_mem(" FAILED\n");                 \
    print_from_prg_mem(                              \
        "ASSERT_EQUAL ERROR! "                       \
        "Expected: " fmt ", Got: " fmt "\n",         \
        expected, recieved);                         \
    print_from_prg_mem("%s\n", msg);                 \
    SREG = sreg;                                     \
    return false;                                    \
  }

// Wrapper to put 'function_name' string in program memory.
#define RUN_TEST(fn, function_name, test_statistics) \
  run_test_function(fn, PSTR(function_name), test_statistics);

#endif  // defined(TESTS_H_)
