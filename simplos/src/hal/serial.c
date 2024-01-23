/*
 * Note: Must be compiled with optimizations.
 *
 * G++ bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=48026 causes the
 * includes to be evalugated *before* the pragmas. Optimizations thus do not
 * affect the included files, as desired.
 *
 * A workaround is to handle this file separatly in the build system.
 * */

#pragma GCC push_options
#pragma GCC optimize("Os")

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcpp"
#include <util/setbaud.h>
#pragma GCC diagnostic pop

/*
  Initiate serial communication, taken from
  https://appelsiini.net/2011/simple-usart-with-avr-libc/
*/
void uart_init(void) {
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

#if USE_2X
  UCSR0A |= _BV(U2X0);
#else
  UCSR0A &= ~(_BV(U2X0));
#endif

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

int16_t uart_putchar(char c, FILE *stream) {
  if (c == '\n') {
    uart_putchar('\r', stream);
  }
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = (uint8_t)c;
  return 0;
}

int16_t uart_getchar(__attribute__((unused)) FILE *stream) {
  loop_until_bit_is_set(UCSR0A, RXC0);
  return UDR0;
}

#pragma GCC pop_options
