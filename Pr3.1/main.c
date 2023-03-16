#include <stdio.h>
#include <msp430.h>

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0; // set P1.0 como salida
    P1DIR &= ~BIT1; // set P1.1 como entrada
    P1OUT |= BIT1; // P1.1 hi (pullup)
    P1REN |= BIT1; // P1.1 pull up

    while (1)
    {
        if (BIT1 & P1IN)
            P1OUT &= ~BIT0; // apaga el led
        else
            P1OUT ^= BIT0; // encender led
    }

    return 0;
}

