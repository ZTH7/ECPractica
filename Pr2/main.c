#include <stdio.h>
#include <msp430.h> 

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    printf("Hello World!\n");

    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0;

    while (1)
    {
        P1OUT ^= BIT0;

        __delay_cycles(300000);
    }

    return 0;
}
