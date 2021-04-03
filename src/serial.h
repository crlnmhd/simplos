/*
  Serial communication.
*/

#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdio.h>

void uart_init(void);

int uart_putchar(char c, FILE *);

int uart_getchar(FILE *);

#endif // SERIAL_H_