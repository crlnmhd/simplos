#include "tasks.h"
#include "simplos.h"
#include <stdio.h>

// extern
volatile uint8_t shared_x = 5;

void test_fn1(void)
{
  shared_x = 10;
}

void test_fn2(void)
{
  printf("fn2\n");
  for (;;)
  {
    printf("2:: x = %d\n", shared_x);
  }
}

void test_fn3(void)
{
  for (;;)
  {
    printf("3 touch and go!\n");
    ++shared_x;
    yield();
  }
}

void idle_fn(volatile Scheduler* schedule)
{
  printf("In idle loop!\n");
  ENABLE_MT();
  // main_sp = *STACK_POINTER;
  //SPAWN_TASK(test_fn1, 1, schedule);

  // // main_sp = *STACK_POINTER;
  SPAWN_TASK(test_fn2, 1, schedule);

  SPAWN_TASK(test_fn3, 1, schedule);


  for (;;)
  {
    // printf("Yielding from idle loop!\n");
    yield();
  }

  // Finally let the scheduler run!
  for (;;)
  {
    for (uint16_t i = 0; i < 0xFFFF; ++i) { ; }
    printf("i");
    fflush(stdout);
  }
}