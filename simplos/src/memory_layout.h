#if !defined(MEMORY_LAYOUT_H_)
#define MEMORY_LAYOUT_H_

/*
 * 0x0-1FF               # Registers
 * 0x200 - 0x204         # Canary
 * 0x205 - 0x34F         # OS stack
 * 0x350 - 0xFFF         # (5) task stack
 * 0x1000 - ? (<0x2000)  # heap
 */

#define REGISTERS_START 0x1FFU

#define CANARY_START 0x204U
#define CANARY_END 0x200U
#define CANARY_VALUE 0x66U  // As good a random value as any.

#define OS_STACK_START 0x3F4U
#define OS_STACK_END 0x205U

#define TASK_RAM_END 0x350U
#define TASK_RAM_START 0xFFFU

#define HEAP_END (TASK_RAM_START + 1U)
// Start of heap (high) is dynamically defined based on the size of the RAM
// needed by init (the main function).

#define FUNCTION_NAME_MAX_LENGTH 8U
#define TASK_QUEUE_LENGTH 6U

#include <stdint.h>
struct StackRange {
  uint16_t low;
  uint16_t high;
};

#endif  // !defined MEMORY_LAYOUT_H_
