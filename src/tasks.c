#include "tasks.h"
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
    printf("x: %d\n", shared_x);
    printf("??\n");
  }
}