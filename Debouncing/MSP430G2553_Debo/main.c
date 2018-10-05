#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1DIR |= BIT0;      // P1.0 LED is an output
	P1OUT &= ~BIT0;     // sets P1.0 LED to off

	P1DIR &= ~BIT3;     // P1.3 an input
	P1IE |= BIT3;       // P1.3 interrupt pin
	P1IES &= ~BIT3;     // Sets P1.3 interrupt to detect rising edge
	P1REN |= BIT3;      // Enables P1.3 Pull up resistor
	P1OUT |= BIT3;      // Makes resistor pull up mode
	P1IFG &= ~BIT3;     // Clear interrupt flag

	TA0CCTL0 = 0x0010;         // Enables the capture/compare register for interrupt timer
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
    P1IE |= BIT3;       // sets the interrupt to P1.3
    P1IFG &= ~BIT3;     // clears the interrupt flag

}
#pragma vector=PORT1_VECTOR     //interrupt at Port 1
__interrupt void Port_1(void)
{
    P1OUT ^= BIT0;              // toggles LED
    P1IE &= ~BIT3;              // resets interrupt
    TA0CTL = TASSEL_2 + MC_1;   // SMCLK, sets to up mode
}

