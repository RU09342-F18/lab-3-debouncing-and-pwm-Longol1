#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR |= BIT0;      // P1.0 LED is an output
    P1OUT &= ~BIT0;     // sets P1.0 LED to off

    P2DIR &= ~BIT1;     // P2.1 an input
    P2IE |= BIT1;       // P2.1 interrupt pin
    P2IES &= ~BIT1;     // Sets P2.1 interrupt to detect rising edge
    P2REN |= BIT1;      // Enables P2.1 Pull up resistor
    P2OUT |= BIT1;      // Makes resistor pull up mode
    P2IFG &= ~BIT1;     // Clear interrupt flag

    TA0CCTL0 = CCIE;         // Enables the capture/compare register for interrupt timer
    TA0CCR0 = 50000;           // The value for Timer to count up to
    TA0CTL = TASSEL_2 + MC_0;  //SMCLK, stop timer

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE);      // Low Power Mode w/ general interrupts enabled


    return 0;
}
#pragma vector=TIMER0_A0_VECTOR     // interrupt for Timer0_A0
__interrupt void Timer_A(void)
{
    TA0CTL = MC_0;      // stops timer
    P2IE |= BIT1;       // sets the interrupt to P1.3
    P2IFG &= ~BIT1;     // clears the interrupt flag

}
#pragma vector=PORT2_VECTOR     //interrupt at Port 1
__interrupt void Port_2(void)
{
    P1OUT ^= BIT0;              // toggles LED
    P2IE &= ~BIT1;              // resets interrupt
    TA0CTL = TASSEL_2 + MC_1;   // SMCLK, sets to up mode
}
