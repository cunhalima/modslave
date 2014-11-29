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

#ifdef __16f628a

//_XT_OSC;        // oscilador com cristal
//_CP_OFF;        // proteção de código DESATIVADA
//_WDT_OFF;       // watchdog timer DESATIVADO
//_BOREN_ON;      // brown-out reset ATIVADO
//_LVP_OFF        // programação em baixa tensão DESATIVADA
//_MCLRE_ON;      // reset por pino externo ATIVADO

//unsigned int __at(_CONFIG) __CONFIG = _BOREN_ON & _CP_OFF & _PWRTE_ON & _WDT_OFF & _LVP_OFF & _MCLRE_ON & _XT_OSC;
static __code unsigned short __at(_CONFIG) _conf = _INTRC_OSC_NOCLKOUT & _WDT_OFF;
#else
static __code unsigned short __at(0x2007) _conf = _INTRC_OSC_NOCLKOUT & _WDT_OFF;
#endif

//unsigned int __at(_CONFIG) __CONFIG = _BOREN_ON & _CP_OFF & _PWRTE_ON & _WDT_OFF & _LVP_OFF & _MCLRE_ON & _XT_OSC;
//static __code unsigned short __at(0x2007) _conf = _INTRC_OSC_NOCLKOUT;

/*
void main(void) {
    TRISA = 0x01;                                   // Porta A toda de entrada
    TRISB = 0xF0;                                   // Porta B toda de saida
    CMCON = 0x07;                                   // desabilita os comparadores
    T2CON = 0x02;                                   // prescaler 1:16 (deixa mais lento o timer 16 vezes)
    PR2 = 0xff;                                     // periodo do timer
    TMR2IE = 0;                                     // habilita interrupcoes pro timer 2
    TMR2ON = 1;                                     // habilita o timer 2
    NOT_RBPU = 0;
    for (;;) {                                      // loop mais externo que passa por todas as jogadas (quantas o jogador quiser)

    }
}  
*/

//

//#include <pic/pic16f627a.h>
#include "lib.h"

#define GPSIM_20Mhz_2400baud_BRGHlow 0x81
#define GPSIM_20Mhz_9600baud_BRGHlow 0x20

#define PIC_4Mhz_2400baud_BRGHlow 0x19
#define PIC_4Mhz_9600baud_BRGHhigh 0x19


unsigned char data_num[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

unsigned char data_alpha[] = {
    0x77, 0x7c, 0x58, 0x5e, 0x79, 0x71
};

//unsigned char hex[] = "0123456789ABCDEF"; <- this doesn't work yet
unsigned char hex[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

void main()
{
    unsigned char c = 'G';
    usart_open(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_BRGH_LOW & USART_EIGHT_BIT, GPSIM_20Mhz_2400baud_BRGHlow);
    TRISA = 0;
    for (;;) {
        c = usart_wait_and_read();
        if(c >= 'A' && c <= 'F') {
            PORTA = data_alpha[c - 'A'];
        } else {
        if(c >= '0' && c <= '9')
            PORTA = data_num[c - '0'];
        }
        usart_wait_and_write(c);
        usart_wait_and_write(':');
        usart_wait_and_write(' ');
        usart_wait_and_write('0');
        usart_wait_and_write('x');
        usart_wait_and_write(hex[(c & 0xf0) >> 4]);
        usart_wait_and_write(hex[c & 0x0f]);
        usart_wait_and_write('\n');
    }
}
