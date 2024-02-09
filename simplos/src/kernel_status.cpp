#include "simplos_types.hpp"  // TODO: extract Kernel ?
#include "string.h"

void Kernel::set_task_name(const uint8_t task_index,
                           const char *name) {  // TODO: progmem
  strlcpy((char *)this->task_names[task_index], name,
          FUNCTION_NAME_MAX_LENGTH + 1);

  this->schedule.queue.tasks[task_index].name = this->task_names[task_index];
}
