#include "hal/context_switch_macros.hpp"
#include "hal/hal.hpp"
#include "io_helpers.hpp"
#include "simplos.hpp"
#include "timers.hpp"
#include "user_tasks/idle_function.hpp"

volatile Kernel internal_kernel_location
    __attribute((section(".kernel_data_location"))) = {};
volatile Kernel *volatile global_kernel = &internal_kernel_location;

volatile uint16_t task_sp __attribute__((section(".task_sp_location"))) = 0;
volatile uint16_t prev_task_sp
    __attribute__((section(".prev_task_sp_location"))) = 0;
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

  verify_that_kernel_is_uninitilized(*(const_cast<Kernel *>(global_kernel)));

  init_scheduler_interupts();

  disable_mt();

  init_memory();

  init_kernel(*(const_cast<Kernel *>(global_kernel)));
  init_schedule(*(const_cast<Kernel *>(global_kernel)));
#if defined(SW_TIME_MEASSREMENTS)
  init_ticks();
#endif  // defined SW_TIME_MEASSREMENTS

  uint8_t index = create_simplos_task(progmem_string("idle task"), 0,
                                      *(const_cast<Kernel *>(global_kernel)));
  global_kernel->schedule.queue.task_index_queue[0] = index;
  global_kernel->schedule.queue.queue_position = 0;

  constexpr uint8_t margin_to_main = 10;

  configure_heap_location(margin_to_main,
                          *(const_cast<Kernel *>(global_kernel)));
  task_sp = global_kernel->schedule.queue.tasks[index].task_sp_adr;
  SET_SP();

  run_idle_fn();
  FATAL_ERROR("UNREACHABLE END OF MAIN\n");
#endif  // MOCK_HAL
}
