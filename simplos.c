#include "serial.h"
#include "simplos.h"
#include "memory.h"

// #include <util/delay.h>
#include <avr/io.h>
// #include <util/setbaud.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#define INTERNAL_LED PORTB5

void uart_init();
void debug_print_message(uint16_t var, uint16_t var2);

// setup IO buffers
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

volatile uint8_t x = 0;

int main()
{
  uint8_t test[100] = { 0 };

  // Initialite serial communication.
  uart_init();

  stdout = &uart_output;
  stdin = &uart_input;

  // FIXME this hides a bug? where some characters from the first serial message are repeated in the begining.
  puts("");

  DDRB |= (1 << INTERNAL_LED);

  //set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2 = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  printf("Hi!\n");

  sei();
  for (;;)
  {
    printf("x: %d\n", x);
    _delay_ms(500);
  }
  debug_print_message(&test[0], &test[99]);
}

void debug_print_message(uint16_t var, uint16_t var2)
{
  printf("Hi!\n");
  task_sp = get_SP();
  printf("Stack pointer: %d\n", task_sp);

  // volatile uint16_t task_sp = SP_ADR;

  portSAVE_CONTEXT()
    printf("Stack pointer inside: %d\n", task_sp);

  portRESTORE_CONTEXT()

    printf("Stack pointer after: %d\n", task_sp);
  printf("Hello World!\n");
  int x;
  printf("Address %d\nStart: %d, %d\n", &x, var, var2);
}

// Timer interupt 
ISR(TIMER2_COMPA_vect)
{
  // For now just save and restore
  task_sp = get_SP();
  SAVE_CONTEXT();
  task_sp = get_SP();
  RESTORE_CONTEXT();
}