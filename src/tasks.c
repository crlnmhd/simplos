#include "tasks.h"
#include "simplos.h"
#include <stdio.h>

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