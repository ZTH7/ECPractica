#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0;

    // Configuración TIMER_A:
    // TimerA1, ACLK/1, modo up, reinicia TACLR
    TA1CTL = TASSEL_1 | TACLR | MC_1;
    TA1CCR0 = 40000;
    TA1CCTL0 = CCIE; // Habilita interrupcion del temporizador (bit CCIE)

    _BIS_SR(LPM4_bits + GIE); // habilita interrupciones

    return 0;
}

// Rutina de interrupción de TIMER1_A0
#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
    P1OUT ^= BIT0; // Toggle P1.0
}
