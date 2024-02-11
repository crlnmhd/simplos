#if !defined(MEMORY_LAYOUT_H_)
#define MEMORY_LAYOUT_H_

#include <stdint.h>

/*
 * 0x0-1FF               # Registers
 * 0x200 - 0x204         # Canary
 * 0x205 - 0x34F         # OS stack
 * 0x350 - 0xFFF         # (5) task stack
 * 0x1000 - ? (<0x2000)  # heap
 */
/*
#define tasks_max 5U
#define registers_start 0x1FFU
#define canary_start 0x204U
#define canary_end 0x200U
#define canary_value 0x66U  // as good a random value as any.
*/

constexpr uint8_t tasks_max{5};
constexpr uintptr_t registers_start{0x1FFU};
constexpr uintptr_t canary_start{0x204U};
constexpr uintptr_t canary_end{0x200U};
constexpr uintptr_t canary_value{0x66U};  // as good a random value as any.

constexpr uintptr_t os_stack_start{0x3FFU};
constexpr uintptr_t os_stack_end{0x205U};

// note: there is an unused area between the 'os' range and the first task
// that is currently *not* verified with canaries.
constexpr uintptr_t task_ram_end{0x350U};
constexpr uintptr_t task_ram_start{0xFFFU};
constexpr uintptr_t task_memory_size{(task_ram_start - task_ram_end) /
                                     tasks_max};

constexpr uintptr_t heap_end{(task_ram_start + 1)};
// start of heap (high) is dynamically defined based on the size of the ram
// needed by init (the main function).

constexpr uintptr_t function_name_max_length{8};
constexpr uint8_t task_queue_length{6};

struct MemorySpan {
  uint16_t low;
  uint16_t high;
};

#endif  // !defined MEMORY_LAYOUT_H_
