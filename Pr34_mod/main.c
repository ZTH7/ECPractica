#include <stdio.h>
#include <msp430.h> 

const char alphabetBig[26][2] = { { 0xEF, 0x00 }, /* "A" LCD segments a+b+c+e+f+g+m */
                                  { 0xF1, 0x50 }, /* "B" */
                                  { 0x9C, 0x00 }, /* "C" */
                                  { 0xF0, 0x50 }, /* "D" */
                                  { 0x9F, 0x00 }, /* "E" */
                                  { 0x8F, 0x00 }, /* "F" */
                                  { 0xBD, 0x00 }, /* "G" */
                                  { 0x6F, 0x00 }, /* "H" */
                                  { 0x90, 0x50 }, /* "I" */
                                  { 0x78, 0x00 }, /* "J" */
                                  { 0x0E, 0x22 }, /* "K" */
                                  { 0x1C, 0x00 }, /* "L" */
                                  { 0x6C, 0xA0 }, /* "M" */
                                  { 0x6C, 0x82 }, /* "N" */
                                  { 0xFC, 0x00 }, /* "O" */
                                  { 0xCF, 0x00 }, /* "P" */
                                  { 0xFC, 0x02 }, /* "Q" */
                                  { 0xCF, 0x02 }, /* "R" */
                                  { 0xB7, 0x00 }, /* "S" */
                                  { 0x80, 0x50 }, /* "T" */
                                  { 0x7C, 0x00 }, /* "U" */
                                  { 0x0C, 0x28 }, /* "V" */
                                  { 0x6C, 0x0A }, /* "W" */
                                  { 0x00, 0xAA }, /* "X" */
                                  { 0x00, 0xB0 }, /* "Y" */
                                  { 0x90, 0x28 } /* "Z" */
};

//**********************************
// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal()
{
    // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
    // Reroute pins to LFXIN/LFXOUT functionality
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;
    // Wait until the oscillator fault flags remain cleared
    CSCTL0 = CSKEY; // Unlock CS registers
    do
    {
        CSCTL5 &= ~LFXTOFFG; // Local fault flag
        SFRIFG1 &= ~OFIFG; // Global fault flag
    }
    while ((CSCTL5 & LFXTOFFG) != 0);
    CSCTL0_H = 0; // Lock CS registers
    return;
}

void ShowBuffer(int buffer[])
{
    LCDMEM[9] = alphabetBig[(buffer[0]) - 65][0];
    LCDMEM[10] = alphabetBig[(buffer[0]) - 65][1];
    LCDMEM[5] = alphabetBig[(buffer[1]) - 65][0];
    LCDMEM[6] = alphabetBig[(buffer[1]) - 65][1];
    LCDMEM[3] = alphabetBig[(buffer[2]) - 65][0];
    LCDMEM[4] = alphabetBig[(buffer[2]) - 65][1];
    LCDMEM[18] = alphabetBig[(buffer[3]) - 65][0];
    LCDMEM[19] = alphabetBig[(buffer[3]) - 65][1];
    LCDMEM[14] = alphabetBig[(buffer[4]) - 65][0];
    LCDMEM[15] = alphabetBig[(buffer[4]) - 65][1];
    LCDMEM[7] = alphabetBig[(buffer[5]) - 65][0];
    LCDMEM[8] = alphabetBig[(buffer[5]) - 65][1];
}

int ShiftBuffer(int buffer, int m)
{
    if (buffer < 90)
        buffer += m;
    else
        buffer = 65;

    return buffer;
}

void Initialize_LCD()
{
    PJSEL0 = BIT4 | BIT5; // For LFXT
    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;
    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8; // Unlock CS registers
    CSCTL4 &= ~LFXTOFF; // Enable LFXT
    do
    {
        CSCTL5 &= ~LFXTOFFG; // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    }
    while (SFRIFG1 & OFIFG); // Test oscillator fault flag
    CSCTL0_H = 0; // Lock CS registers
    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;
    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;
    LCDCCPCTL = LCDCPCLKSYNC; // Clock synchronization enabled
    LCDCMEMCTL = LCDCLRM; // Clear LCD memory
    //Turn LCD on
    LCDCCTL0 |= LCDON;
    return;
}

int str[6];
int n = 65, cnt = 1;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P9DIR |= BIT7;
    P1DIR |= BIT0;

    P1OUT &= ~BIT0;

    Initialize_LCD();
    config_ACLK_to_32KHz_crystal();

    // para seleccionar la función del módulo primario (P3.4 y P3.5)
    P3SEL0 |= BIT4;
    P3SEL0 |= BIT5;

    P3SEL1 &= ~BIT4;
    P3SEL1 &= ~BIT5;

    CSCTL0_H = CSKEY >> 8; // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL; // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // Set all dividers
    CSCTL0_H = 0; // Lock CS registers

    // Configure USCI_A1 for UART mode
    UCA1CTLW0 = UCSWRST; // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK; // CLK = SMCLK

    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1

    UCA1BR0 = 52; // 8000000/16/9600
    UCA1BR1 = 0x00;
    UCA1MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA1CTLW0 &= ~UCSWRST; // Initialize eUSCI

    // Configure interruptions to receive and transmit data in register UCA1IE
    UCA1IE |= BIT0;
    UCA1IE &= ~BIT1;

    // Configuración TIMER_A:
    // TimerA1, ACLK/1, modo up, reinicia TACLR
    TA0CTL = TASSEL_1 | TACLR | MC_1;
    TA0CCR0 = 40000; // periodo del reloj
    TA0CCTL0 = CCIE; // Habilita interrupción (bit CCIE)

    //_BIS_SR(LPM4_bits + GIE);
    __enable_interrupt();

    UCA1IE &= ~BIT1;

    return 0;
}

volatile int sw = 0, sw2 = 0x00, sw3 = 0x00;
int contador = 0;

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    if (UCA1IFG & UCRXIFG)
    {
        if (UCA1RXBUF == 0x2E)
        {
            UCA1IE |= BIT1;
            UCA1IE &= ~BIT0;
            sw2 = 0x01;
            sw3 = 0x01;
        }

        int i;
        for (i = cnt - 1; i > 0; i--)
        {
            str[i] = str[i - 1];
        }
        str[0] = UCA1RXBUF; // Transfer data
        contador++;

        if (cnt < 6)
            cnt++;

        ShowBuffer(str);

        sw = 0x01;

        UCA1IFG &= ~BIT0; //reset flag
    }

    if (UCA1IFG & UCTXIFG)
    {
        if (sw2 == 0x01)
        {
            char num[22] = { ' ', 'c', 'a', 'r', 'a', 'c', 't', 'e', 'r', 'e',
                             's', ' ', 'r', 'e', 'c', 'i', 'b', 'i', 'd', 'o',
                             's', '.' };

            volatile int x = contador;
            for (; x > 0;)
            {
                UCA1TXBUF = x % 10 + 0x30;
                x /= 10;
            }

            volatile int i = 0;
            for (; i < 22;)
            {
                UCA1TXBUF = num[i];
                volatile unsigned int j;
                j = 1000;                          // SW Delay
                do
                    j--;
                while (j != 0);
                i++;
            }

            sw2 = 0x00;
        }

        //UCA1IFG &= ~BIT1; //reset flag
    }
}

// Rutina de interrupción de TIMER1_A0 (cada perido del reloj entra)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    if (sw == 0x01)
    {
        P9OUT ^= BIT7;
        sw = 0x02;
    }
    else if (sw == 0x02)
    {
        P9OUT ^= BIT7;
        sw = 0x00;
    }

    if(sw3 == 0x01){
        P1OUT |= BIT0;
    }
}
