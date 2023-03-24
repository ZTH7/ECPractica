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

void ShiftBuffer(int buffer[], int m)
{
    if (*buffer <= 90)
        *buffer += m;
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

volatile unsigned int str[6];
int n = 65, cnt = 1;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    Initialize_LCD();

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
    UCA1IE |= BIT1;

    //_BIS_SR(LPM4_bits + GIE);
    __enable_interrupt();

    return 0;
}

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    if (UCA1IFG & BIT0)
    {
        int i;
        for (i = cnt - 1; i > 0; i--)
        {
            str[i] = str[i - 1];
        }
        str[0] = UCA1RXBUF;
        if (cnt < 6)
            cnt++;

//        str[cnt] = UCA1RXBUF;
//        cnt++;
//        if(cnt == 6) cnt = 0;

        ShowBuffer(str);
        UCA1IFG &= ~BIT0;
    }

    else if (UCA1IFG & BIT1)
    {
        if (n < 91)
            UCA1TXBUF = n;

        ShiftBuffer(&n, 1);
        UCA1IFG &= ~BIT1;
    }
}
