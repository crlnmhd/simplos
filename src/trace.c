/*
#include <avr/io.h>
#include <simavr/avr/avr_mcu_section.h>
// generate gtkwave files for register content and interrupts.

const struct avr_mmcu_vcd_trace_t _mytrace[] _MMCU_ = {
    {
        AVR_MCU_VCD_SYMBOL("PORTB"),
        .what = (void *)&PORTB,
    },
    {
        AVR_MCU_VCD_SYMBOL("timer_int"),
        .mask = (1 << ICIE1),
        .what = (void *)&TIMSK1,
    },
};
*/
