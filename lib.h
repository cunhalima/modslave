/*
The MIT License (MIT)

Copyright (c) 2014 Alex R. Cunha Lima, Dinara Rigon, Gabrielle A. de Souza

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _INC_LIB_H_
#define _INC_LIB_H_

#if 0

#define USART_TX_INT_ON   0xff
#define USART_TX_INT_OFF  0x7f

#define USART_RX_INT_ON   0xff
#define USART_RX_INT_OFF  0xbf

#define USART_BRGH_HIGH   0xff
#define USART_BRGH_LOW    0xef

#define USART_CONT_RX     0xff
#define USART_SINGLE_RX   0xf7

#define USART_SYNC_MASTER 0xff
#define USART_SYNC_SLAVE  0xfb

#define USART_NINE_BIT    0xff
#define USART_EIGHT_BIT   0xfd

#define USART_SYNCH_MODE  0xff
#define USART_ASYNCH_MODE 0xfe

void usart_open(unsigned char, unsigned int);

void usart_putc(unsigned char);
unsigned char usart_getc(void);

unsigned char usart_busy(void);
unsigned char usart_drdy(void);

#endif
unsigned char usart_wait_and_read(void);
void usart_wait_and_write(unsigned char);

#endif
