#include <msp430.h> 


/**
 * main.c
 */

int pwm = 500;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

        //Button Setup
        P2DIR &= ~BIT1;     // P2.1 an input
        P2IE  |= BIT1;       // P2.1 interrupt pin
        P2IES |= BIT1;      // Sets P2.1 interrupt to detect falling edge
        P2REN |= BIT1;      // Enables P2.1 Pull up/pull down
        P2OUT |= BIT1;      // Makes resistor pull up
        P2IFG &= ~BIT1;     // Clear interrupt flag

        //Output Setup
        P1DIR |= BIT0;      // Sets the P1.0 LED as an output
        P1SEL |= BIT0;      // Enables the output of a timer to be used
                            // directly to trigger the LED

        //Timer Setup
        TA0CTL = TASSEL_2 + MC_1 + TACLR;      // SMCLK, UP Mode, clear clock
        TA0CCR0 = 1000;                      // Sets the clock freq to 1Khz
        TA0CCTL1 = OUTMOD_7 ;                  // Reset/set mode
        TA0CCR1 = 500;                         // duty cycle = (ACLK/TA0CCR1)*100%

        TA1CCTL0 = CCIE;                       // Enables the interrupt for this clock
        TA1CCR0 = 10000;                       // Sets the overflow time for debouncing

        __bis_SR_register(LPM0_bits + GIE);      //general interrupts enabled


}
#pragma vector=PORT2_VECTOR
__interrupt void PORT_2(void)
{
    TA1CTL = TASSEL_2 + MC_1 + TACLR;        // SMCLK, up mode

    P2IFG &= ~BIT1;                  // reset flag
    P2IES &= ~BIT1;                  // disable interrupts for the button P2.1
}
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void)
{
    if(pwm < 500)
    {
        pwm += 100;
    }
    else
    {
        pwm = 0;
    }
    P2IE |= BIT1;       // enables interrupts
    TA0CCR1 = pwm;      // sets ccr1 register to current value of pwm
    TA1CTL = MC_0;      // stops timer
}

