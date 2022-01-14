#if defined(DEFINES_H_)
#define DEFINES_H

/*
 * Define common settings for simplos at build time.
 * */

// Verbose output from prints
#undef VERBOSE_OUTPUT

// Use attached device on  22-26 to output current task as status during context
// switch. Poll this over time on a separate device to gain knowledge of system
// performance and how the ratio of time spent in context switches. Very small
// overhead.
#undef HW_TIME_MEASSUREMENTS

// Count the time spent in interups as oposed to outside. Incurres some
// overhead. Print to uart using PRINT_CS_TIMING_DATA()

// NOTE: use build flag instead
// #define SW_TIME_MEASSREMENTS

// Use priority based scheduling.
#undef PRIORITY_SCHEDULING

// Use static scheduling.
#define USE_STATIC

#endif  // DEFINES_H_
