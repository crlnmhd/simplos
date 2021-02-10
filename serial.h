/*
  Serial communication.
*/

#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdio.h>

void uart_init();

int uart_putchar(char c, FILE *stream);

char uart_getchar(FILE *stream);

#endif // SERIAL_H_