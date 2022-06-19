#if !defined(MEMORY_LAYOUT_H_)
#define MEMORY_LAYOUT_H_

/*
 * 0x0-1FF            # Registers
 * 0x200 - 0x3FF      # Static ram
 * 0x400 - 0x44F      # OS stack
 * 0x450 - 0xE4F      # (5) task stack
 * 0xD50 - 0x2000     # heap
 */

#define TASK_MEMORY_BYTES 0x200
#define ALLOCABLE_MEMORY_BYTES 0x700

// End of main and some extra just to be sure.
#define HEAP_PAGE_SIZE 0x100

#define HEAP_CHUNKS (ALLOCABLE_MEMORY_BYTES / HEAP_PAGE_SIZE)
// OS task memory for use within constext switches and alike. Aprox 50 bytes).
// RAMSTART = 0x200
// Size of the heap map.
#define HEAP_START (0x2000 - HEAP_CHUNKS)

#define OS_STACK_SIZE 0x50
#define MEM_START 0x200
#define MAX_STATIC_RAM_USAGE 0x200
#define TASK_RAM_LOW (MEM_START + MAX_STATIC_RAM_USAGE + OS_STACK_SIZE)
#define HEAP_RAM_END (TASK_RAM_LOW + (TASKS_MAX * TASK_MEMORY_BYTES))

#define FUNCTION_NAME_MAX_LENGTH 8

#endif  // !defined MEMORY_LAYOUT_H_
