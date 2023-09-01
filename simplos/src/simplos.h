#if !defined(SIMPLOS_H_)
#define SIMPLOS_H_

#include "io_helpers.h"
#include "memory.h"
#include "scheduler.h"
#include "serial.h"
#include "simplos_types.h"
#include "tasks.h"
#include "timers.h"

#define INLINED static inline __attribute__((always_inline))

// clang-format off
#if defined(__clang__)
_Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"")
#endif
//clang-format on

extern volatile Kernel internal_kernel; 
extern Kernel volatile *volatile kernel; 


#define DO_PRAGMA_(x) _Pragma(#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)
//clang-format off
#if defined(__clang__)
#define CLANG_IGNORE_BEGIN(warning)                             \
  _Pragma("clang diagnostic push")                              \
  _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
  DO_PRAGMA(clang diagnostic ignored warning)
#define CLANG_IGNORE_END() _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define GCC_IGNORE_BEGIN(warning)                             \
  _Pragma("GCC diagnostic push")                              \
  _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"") \
  DO_PRAGMA(GCC diagnostic ignored warning)
#define GCC_IGNORE_END() _Pragma("GCC diagnostic pop")
#endif  // __GNUC__

#if defined(__clang__)
#define BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING() \
  CLANG_IGNORE_BEGIN("-Wincompatible-pointer-types-discards-qualifiers")
#define END_DISCARD_VOLATILE_QUALIFIER_WARNING() CLANG_IGNORE_END()
#elif defined(__GNUC__)
#define BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING() \
  GCC_IGNORE_BEGIN("-Wdiscarded-qualifiers")
#define END_DISCARD_VOLATILE_QUALIFIER_WARNING() GCC_IGNORE_END()
#else // other tools like cppcheck
#define BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING()
#define END_DISCARD_VOLATILE_QUALIFIER_WARNING()
#endif  // __GNUC__
// clang-format on

#define INDEX_OF_CURRENT_TASK \
  kernel->schedule.queue.task_index_queue[kernel->schedule.queue.queue_position]

/*
 * Add a task to the task queue. This is needed to let the the task execute.
 * */
uint8_t add_to_task_list(uint8_t priority, Task_Queue *queue);

/*
 * Set up kernel configuration.
 * */
void init_kernel(void);

/*
 * Set up the scheduler.
 * */
void init_schedule(void);

/*
 * Initialise heap mapping and canaries.
 */
void init_memory(void);

uint16_t num_context_switch_overhead_bytes(void);

Simplos_Task *get_task(pid_type pid);
enum Task_Status task_status(pid_type pid);

void kill_current_task(void);

typedef uint8_t Index;
void set_task_name(Index task_index, const char *name);
Index create_simplos_task(const char *name, const uint8_t priority);

#endif  // SIMPLOS_H_
