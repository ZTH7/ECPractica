#include <msp430.h>


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0; // set P1.0 como salida
    P1DIR &= ~BIT1; // set P1.1 como entrada
    P1OUT |= BIT1; // set P1.1 as pullup
    P1REN |= BIT1; // set P1.1 as pullup
    P1IE |= BIT1; // Enable interrupt on P1.1
    P1IES |= BIT1; // set sensitivity to falling edge on P1.1
    P1IFG &= ~BIT1; //Clear interrupt flag P1.1

    _BIS_SR(LPM4_bits + GIE); // Habilita las interrupciones

    return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    P1OUT ^= BIT0; // P1.O = toggle
    P1IFG &= ~BIT1; // P1.1 IFG clear
}
