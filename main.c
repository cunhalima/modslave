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

// Cortina: Abrir/Fechar (2 bits)
// Luz: Ligar/desligar (1 bit)
// Dimmer: Nada,Aumenta 5%,Diminui 5% (2 bits)
// Sensor de temperatura (8 bits)

#include "defs.h"

#ifndef TEST

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
#include "lib.h"
#endif

//#include <pic/pic16f627a.h>

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

#ifdef TEST


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

#include <stdio.h>

int TRISA=0;
int PORTA=0;

Uint8 simulacao[] = { 0x11, 0x02, 0x00, 0xC4, 0x00, 0x16, 0xBA, 0xA9 };
int pos = 0;

void usart_open(int a, int b) {
}

Uint8 usart_wait_and_read(void) {
    if (pos >= sizeof(simulacao)) {
        for (;;);
    }
    return simulacao[pos++];
}

void usart_wait_and_write(char c) {
    printf("%c", c);
}

#endif

#define SLAVE_ADDR              0x01

#define READ_INPUT_STATUS       0x02



Uint16 CRC16;
#define SEED 0xFFFF  //initialization for CRC16
//#define GP   0xA001  //generating polynomial
#define GP   0x8005  //generating polynomial
//for standard CRC16
//(remainder of division)
//to start a new CRC, set CRC16 = SEED
//then for each byte call Calc_CRC(byte, &CRC16);
//CRC16 will contain the result
//(if you calculate all of the incoming data
//INCLUDING the CRC, the result should be 0x0000
//and if you are sending the CRC be sure to
//send the bytes in the correct order)
void Calc_CRC(Uint8 b, Uint16* CRC)
{
   int carry;
   int i;
   CRC[0] ^= b & 0xFF;
   for (i=0; i<8; i++)
   {
      carry = CRC[0] & 0x0001;
      CRC[0]>>=1;
      if (carry) CRC[0] ^= GP;
   }
}

Uint8 rec[1024];
int recpos = 0;

Uint8 read8(void) {
    Uint8 x = usart_wait_and_read();
    rec[recpos++] = x;
    return x;
}

Uint16 read16(void) {
    Uint16 x;
    x = read8();
    x = (x << 8) | (read8() & 0xFF);
    return x;
}

Uint16 read16_le(void) {
    Uint16 x;
    x = read8();
    x = x | (read8() << 8);
    return x;
}


// Compute the MODBUS RTU CRC
Uint16 ModRTU_CRC(Uint8 *buf, int len)
{
  Uint16 crc = 0xFFFF;
    int pos, i;
 
  for (pos = 0; pos < len; pos++) {
    crc ^= (Uint16)buf[pos];          // XOR byte into least sig. byte of crc
 
    for (i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;  
}

Uint16 read_crc(void) {
    Uint16 crc_enviado;
    Uint16 crc_calculado;
    crc_calculado = ModRTU_CRC(rec, recpos);
    crc_enviado = read16_le();
    if (crc_calculado == crc_enviado) {
        printf("eba\n");
    } else {
        printf("putz\n");
    }
    printf("CRC = %08X, %08X\n", crc_enviado, crc_calculado);
    return crc_enviado;
}
void startREC(void) {
    recpos = 0;
}

void endREC(void) {
}

void main()
{
    unsigned char c = 'G';
    usart_open(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_BRGH_LOW & USART_EIGHT_BIT, GPSIM_20Mhz_2400baud_BRGHlow);
    TRISA = 0;
    Uint8 slave;
    Uint8 fn;
    Uint16 addr;
    Uint16 count;
    for (;;) {
        // inicio do pacote
        startREC();
        slave = read8();
        fn = read8();
        switch(fn) {
            case READ_INPUT_STATUS:
                addr = read16();
                count = read16();
                printf("LER DE %04X\n", addr);
                printf("LER %d bits\n", count);
                break;
            default:
                printf("nao implementamos ainda\n");
        }
        endREC();
        printf("LER CRC %04X\n", read_crc());

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
