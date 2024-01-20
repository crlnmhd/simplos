#include <stdio.h>

#include "hal/context_switch_macros.h"
#include "hal/hal.h"
#include "simplos.h"
#include "tasks.h"
#include "timers.h"

volatile Kernel internal_kernel_location
    __attribute((section(".kernel_data_location"))) = {0};
volatile Kernel *volatile global_kernel = &internal_kernel_location;

volatile uint16_t task_sp __attribute__((section(".task_sp_location"))) = 0;
volatile uint16_t next_task_sp
    __attribute((section(".next_task_sp_location"))) = 0;
volatile uint16_t scheduler_task_sp
    __attribute((section(".scheduler_task_sp_location"))) = 0;

int main(void) {
#ifdef MOCK_HAL
#error "Can not run simplos on mock hal"
#else

  disable_interrupts();
  SP = INITIAL_STACK_START;

  uart_init();
  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  verify_that_kernel_is_uninitilized();

  init_scheduler_interupts();

  DISABLE_MT();

  init_memory();

  init_kernel(global_kernel);
  init_schedule();
#if defined(SW_TIME_MEASSREMENTS)
  init_ticks();
#endif  // defined SW_TIME_MEASSREMENTS

  uint8_t index = create_simplos_task("idle_fn", 0);
  global_kernel->schedule.queue.task_index_queue[0] = index;
  global_kernel->schedule.queue.queue_position = 0;

  const uint8_t margin_to_main = 10;

  configure_heap_location(margin_to_main);
  task_sp = global_kernel->schedule.queue.tasks[index].task_sp_adr;
  SET_SP();

  enable_interrupts();
  ENABLE_MT();
  run_idle_fn();
  FATAL_ERROR("UNREACHABLE END OF MAIN\n");
#endif  // MOCK_HAL
}
