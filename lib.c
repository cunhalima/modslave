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

#include "defs.h"

//#include <pic/pic16f627a.h>
#include "lib.h"

#if 0

void usart_open(unsigned char config, unsigned int spbrg) { /* {{{ */
  TXSTA = 0;           // Reset USART registers to POR state
  RCSTA = 0;
 
  if(config & ~USART_ASYNCH_MODE)
   SYNC = 1;

  if(config & ~USART_EIGHT_BIT) {
    TX9 = 1;
    RX9 = 1;
  }

  if(config & ~USART_SYNC_SLAVE)
    CSRC = 1;

  /*
 *   if(config & ~USART_SINGLE_RX)
 *       CREN = 1;
 *         else 
 *             SREN = 1;
 *               */

  CREN = 1;
  SREN = 1;

  if(config & ~USART_BRGH_LOW)
    BRGH = 1;
  else 
    BRGH = 0;

  /* TX interrupts */
  TXIF = 0;

  if(config & ~USART_RX_INT_OFF)
    RCIE = 1;
  else 
    RCIE = 0;

  /* RX interrupts */
  RCIF = 0;

  if(config & ~USART_TX_INT_OFF)
    TXIE = 1;
  else 
    TXIE = 0;

  SPBRG = (char)spbrg;

  TXEN = 1;
  SPEN = 1;

  TRISB1 = 1;  // set B1 to transmit
  TRISB2 = 0;  // set B2 to receive
}
/* }}} */

void usart_putc(unsigned char dat) { /* {{{ */
  //if(TX9) {
  //  TX9D = 0;
  //  if(USART_Status.TX_NINE)
  //    TX9D = 1;
  //}
  TXREG = dat;      // Write the data byte to the USART
}
/* }}} */
unsigned char usart_getc(void) { /* {{{ */
  return RCREG;
}
/* }}} */

unsigned char usart_busy(void) { /* {{{ */
  return !TRMT;
}
/* }}} */
unsigned char usart_drdy(void) { /* {{{ */
    return RCIF;
}
/* }}} */

#endif

unsigned char usart_wait_and_read(void) { /* {{{ */
    while(!usart_drdy());
    return usart_getc();
} 
/* }}} */
void usart_wait_and_write(unsigned char dat) { /* {{{ */
    while(usart_busy());
    usart_putc(dat);
}
/* }}} */
