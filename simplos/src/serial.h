/*
  Serial communication.
*/

#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdio.h>

void uart_init(void);

int16_t uart_putchar(char c, FILE *);

int16_t uart_getchar(FILE *);

#endif  // SERIAL_H_
