#include <msp430.h> 


/**
 * main.c
 */

int pwm = 500;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

        //Button Setup
        P1DIR &= ~BIT3;     // P1.3 an input
        P1IE |= BIT3;       // P1.3 interrupt pin
        P1IES |= BIT3;      // Sets P1.3 interrupt to detect falling edge
        P1REN |= BIT3;      // Enables P1.3 Pull up/pull down
        P1OUT |= BIT3;      // Makes resistor pull up
        P1IFG &= ~BIT3;     // Clear interrupt flag

        //Output Setup
        P1DIR |= BIT6;      // Sets the P1.6 LED as an output
        P1SEL |= BIT6;      // Enables the output of a timer to be used
                            // directly to trigger the LED

        //Timer Setup
        TA0CTL = TASSEL_2 + MC_1 + TACLR;      // SMCLK, UP Mode, clear
        TA0CCR0 = 1000-1;                      // Sets the clock freq to 1Khz
        TA0CCTL1 = OUTMOD_7 ;                  // Reset/set mode
        TA0CCR1 = 500;                         // duty cycle = (SMCLK/TA0CCR1)*100%

        TA1CCTL0 = CCIE;                       // Enables the interrupt for this clock
        TA1CCR0 = 50000;                       // Sets the overflow time for debouncing

        __bis_SR_register(LPM0_bits + GIE);      //general interrupts enabled


}
#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    TA1CTL = TASSEL_2 + MC_1;        // SMCLK, up mode

    P1IFG &= ~BIT3;                  // reset flag
    P1IES &= ~BIT3;                  // disable interrupts for the button P1.3
}
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void)
{
    if(pwm < 1000)
    {
        pwm += 100;
    }
    else
    {
        pwm = 0;
    }
    P1IE |= BIT3;       // enables interrupts
    TA0CCR1 = pwm;      // sets ccr1 register to current value of pwm
    TA1CTL = MC_0;      // stops timer
}

