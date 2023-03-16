#include <msp430.h> 


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0;

    TA0CTL = TASSEL_1 | TACLR | MC_1;
    TA0CCR0 = 40000;
    TA0CCTL0 = CCIE; // Habilita interrupcion del temporizador (bit CCIE)

    _BIS_SR(LPM4_bits + GIE); // habilita interrupciones

    return 0;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
    P1OUT ^= BIT0; // Toggle P1.0
}
