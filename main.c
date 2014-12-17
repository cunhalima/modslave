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
// j

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
#include <pic16/usart.h>

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

//Uint8 simulacao[] = { 0x11, 0x02, 0x00, 0xC4, 0x00, 0x16, 0xBA, 0xA9 };
//Uint8 simulacao[] = { 0x11, 0x03, 0x00, 0x6B, 0x00, 0x05, 0x76, 0x87 };
Uint8 simulacao[] = { 0x11, 0x06, 0x00, 0x01, 0x00, 0x03, 0x9A, 0x9B };
int pos = 0;

void usart_open(int a, int b) {
}

Uint8 usart_wait_and_read(void) {
    if (pos >= sizeof(simulacao)) {
        for (;;);
    }
    return simulacao[pos++];
}

void usart_wait_and_write(Uint8 c) {
    printf("%02X\n", c);
}

#endif

#define SLAVE_ADDR              0x01
#define READ_INPUT_STATUS       0x02
#define READ_HOLDING_REGISTERS  0x03
#define PRESET_SINGLE_REGISTER  0x06

#define NUM_REGS    6

#define LAMP1   0
#define LAMP2   1
#define LUMIN   2
#define TEMP    3
#define CORT    4

/*
 * Lampada1 = 0
 * Lampada2 = 1
 * LumL1 = 2
 * Temp = 3
 * Cortina = 4
 */

Uint16 crc;
Uint16 regs[NUM_REGS];

Uint16 swap16(Uint16 x) {
    return (x >> 8) | (x << 8);
}

void calc_CRC(Uint8 x) {
    int i;
    crc ^= (Uint16)x;
    for (i = 8; i != 0; i--) {
        if (crc & 1) {
            crc >>= 1;
            crc ^= 0xA001;
        } else {
            crc >>= 1;
        }
    }
}

void write8(Uint8 x) {
    calc_CRC(x);
    //printf("devolvendo: %02X\n", x);
    usart_wait_and_write(x);
}

void write16(Uint16 x) {
    write8(x >> 8);
    write8(x & 0xFF);
}

Uint8 read8(void) {
    Uint8 x = usart_wait_and_read();
    calc_CRC(x);
    return x;
}

Uint16 read16(void) {
    Uint16 x;
    x = read8();
    x = (x << 8) | read8();
    return x;
}

void packet_start(void) {
    crc = 0xFFFF;
}

void packet_end(void) {
    Uint16 readcrc;
    Uint16 bkcrc = crc;
    bkcrc = swap16(bkcrc);
    readcrc = read16();
    if (readcrc != bkcrc) {
        //printf("CRC ERROR: read %04X expect %04X\n", readcrc, bkcrc);
    }
}

Uint8 lerTemperatura(void) {
    return 0;
}

void interruptorLampada1(Uint8 value) {
    //printf("lamp1 %d\n", value);
    (void)value;
}

void interruptorLampada2(Uint8 value) {
    //printf("lamp2 %d\n", value);
    (void)value;
}

void intensidadeLuz(Uint8 value) {
    //printf("intensidade %d\n", value);
    (void)value;
}

void moverCortina(Uint8 value) {
    //printf("cortina %d\n", value);
    (void)value;
}

void main()
{
    Uint8 slave;
    Uint8 fn;
    Uint16 addr;
    Uint16 count;
    Uint16 value;
    usart_open(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_BRGH_LOW & USART_EIGHT_BIT, GPSIM_20Mhz_2400baud_BRGHlow);
    //TRISA = 0;
    for (;;) {
        int i;
        // inicio do pacote
        packet_start();
        slave = read8();
        fn = read8();
        switch(fn) {
            case READ_HOLDING_REGISTERS: // leitura
                addr = read16();
                count = read16();
                packet_end();
                //printf("LER DE %04X\n", addr);
                //printf("LER %d registradores\n", count);
                // mandar resposta
                packet_start();
                write8(SLAVE_ADDR);
                write8(fn);
                write8(count * 2);
                for (i = 0; i < count; i++) {
                    Uint8 r = addr + i;
                    if (r == TEMP) {
                        regs[r] = lerTemperatura();
                    }
                    write16(regs[r]);
                }
                write16(swap16(crc));
                break;
            case PRESET_SINGLE_REGISTER: // escrita
                addr = read16();
                value = read16();
                packet_end();
                //printf("GRAVAR EM %04X\n", addr);
                //printf("GRAVAR valor %04X\n", value);
                regs[addr] = value;
                switch(addr) {
                    case LAMP1:
                        interruptorLampada1(value);
                        break;
                    case LAMP2:
                        interruptorLampada2(value);
                        break;
                    case LUMIN:
                        intensidadeLuz(value);
                        break;
                    case TEMP:
                        break;
                    case CORT:
                        moverCortina(value);
                        break;
                }
                // mandar resposta
                packet_start();
                write8(SLAVE_ADDR);
                write8(fn);
                write16(addr);
                write16(value);
                write16(swap16(crc));
                break;
            /*
            case READ_INPUT_STATUS:
                addr = read16();
                count = read16();
                printf("LER DE %04X\n", addr);
                printf("LER %d bits\n", count);
                break;
            */
            default:
                //printf("nao implementamos ainda\n");
        }
        //printf("LER CRC %04X\n", read_crc());
        //usart_wait_and_write(':');
        usart_wait_and_write(' ');
        usart_wait_and_write('0');
        usart_wait_and_write('x');
        usart_wait_and_write('\n');
    }
}
