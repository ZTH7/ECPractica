#include <msp430.h> 
#include <stdio.h>

const unsigned char LCD_Num[10] = { 0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF,
                                    0xE0, 0xFF, 0xE7 };
//**********************************************************
// Initializes the LCD_C module
// *** Source: Function obtained from MSP430FR6989�s Sample Code
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

//***************function that displays any 16-bit unsigned
void display_num_lcd(unsigned int n)
{
//initialize i to count though input paremter from main function, digit is used for while loop so as long as n is not 0 the if statements will be executed.
    int i;
    int digit;
    digit = n;
    while (digit != 0)
    {
        digit = digit * 10;
        i++;
    }
    if (i > 1000)
    {
        LCDM8 = LCD_Num[n % 10]; // inputs the first(least significant digit) from the array onto the LCD output.
        n = n / 10;
        i++;
    }
    if (i > 100)
    {
        LCDM15 = LCD_Num[n % 10]; // inputs the second(least significant digit) from the array onto the LCD output.
        n = n / 10;
        i++;
    }
    if (i > 10)
    {
        LCDM19 = LCD_Num[n % 10]; // inputs the third(least significant digit) from the array onto the LCD output.
        n = n / 10;
        i++;
    }
    if (i > 1)
    {
        LCDM4 = LCD_Num[n % 10]; // inputs the fourth(least significant digit) from the array onto the LCD output.
        n = n / 10;
        i++;
    }
    if (i > 0)
    {
        LCDM6 = LCD_Num[n % 10]; // inputs the last (most significant digit) from the array onto the LCD output.
        n = n / 10;
        i++;
    }
}

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

unsigned int cnt = 0, state = 0;

int main(void)
{

    cnt = 0;
    state = 0;

    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    Initialize_LCD();
    config_ACLK_to_32KHz_crystal();

    // Boton 1
    P1OUT |= BIT1;
    P1REN |= BIT1;
    P1IE |= BIT1;
    P1IES |= BIT1;
    P1IFG &= ~BIT1;

    P1OUT |= BIT2;
    P1REN |= BIT2;
    P1IE |= BIT2;
    P1IES |= BIT2;
    P1IFG &= ~BIT2;

    TA0CTL = TASSEL_1 | TACLR | MC_1;
    TA0CCR0 = 4000; //Tiempo de ciclo
    TA0CCTL0 = CCIE;

    _BIS_SR(LPM4_bits + GIE);

    return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & BIT1)
    {
        TA0CTL ^= BIT4;
        P1IFG &= ~BIT1;
    }
    else if (P1IFG & BIT2)
    {
        cnt = 0;
        P1IFG &= ~BIT2;
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    cnt = cnt + 1;
    display_num_lcd(cnt);
}
