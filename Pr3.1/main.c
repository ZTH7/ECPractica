#include <stdio.h>
#include <msp430.h>

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0;
    P1DIR &= ~BIT1;
    P1OUT |= BIT1; // para tener a nivel alto el boton (led apagado)
    P1REN |= BIT1;

    while (1)
    {
        if (BIT1 & P1IN)
            P1OUT &= ~BIT0; // apaga el led si esta a nivel alto
        else
            P1OUT |= BIT0; // encender led si esta a nivel bajo
    }

    return 0;
}

