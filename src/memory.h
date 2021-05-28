#ifndef MEMORY_H_
#define MEMORY_H_

#include "simplos_types.h"
#include "stdint.h"

uint16_t task_default_sp(uint8_t);
uint16_t stack_end(void);
uint16_t os_stack_start(void);
uint16_t os_stack_end(void);

void assert_stack_integrity(taskptr_t task_block);

#endif  // MEMORY_H_
