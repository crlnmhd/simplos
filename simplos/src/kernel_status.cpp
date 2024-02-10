#include "io_helpers.hpp"
#include "simplos_types.hpp"  // TODO: extract Kernel ?

void Kernel::set_task_name_P(const uint8_t task_index,
                             const ProgmemString &name) {
  this->schedule.queue.tasks[task_index].name = name;
}
