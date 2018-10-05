#include <msp430.h> 


/**
 * main.c
 */

int duty_cycle = 500;
int timer_state = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

        // LED Setup
        P1DIR |= BIT0;      // P1.0 LED is an output

        //Button Setup
        P2DIR &= ~BIT1;     // P1.2 an input
        P2IE |= BIT1;       // P1.2 interrupt pin
        P2IES |= BIT1;      // Sets P1.2 interrupt to detect falling edge
        P2REN |= BIT1;      // Enables P1.2 Pull up/pull down
        P2OUT |= BIT1;      // Makes resistor pull up
        P2IFG &= ~BIT1;     // Clear interrupt flag

        //Timer Setup
        TA0CCR0 = 1000;                       // Period to set the frequency to 1kHz
        TA0CTL = TASSEL_2 + MC_1 + TACLR;     //SMCLK, UP-mode, Clears timer
        TA1CCTL0 = CCIE;                      // enables interrupt for CCR1;
        TA1CCR0 = 50000;                      //controls debouncing it overflows every 10ms


        __bis_SR_register(GIE);      //general interrupts enabled

        for(;;)     //the loop for pwm
        {
            if(TA0R <= duty_cycle)
            {
                P1OUT ^= BIT0;
            }
            else if(TA0R > duty_cycle)
            {
                P1OUT &= ~BIT0;
            }
        }

}
#pragma vector=PORT2_VECTOR
__interrupt void PORT_2(void)
{

    switch(timer_state)
    {
    case 0: //Initial state when LED is turning on
        TA1CTL = TASSEL_2 + MC_1 + TACLR;       // SMCLK, Up-mode, Clears timer
        P2IFG &= ~BIT1;                         // Clears the interrupt flag
        P2IE &= ~BIT1;                          // Disables the interrupt for P1.3
        break;
    case 1: //
        TA1CTL = TASSEL_2 + MC_1 + TACLR;       // SMCLK, Up-mode, Clears timer
        P2IFG &= ~BIT1;                         // Clears the interrupt flag
        P2IE &= ~BIT1;                          // Disables the interrupt for P1.3
        break;
    }

}
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void)
{
    switch(timer_state)
           {
           case 0:                      // initial state of timer starts at 50% duty cycle and increments by 10%
               if(duty_cycle < 1000)
                   {
                   duty_cycle += 100;   // increments the duty cycle by 10% to increase brightness
                   }
               else
                   {
                   duty_cycle = 0;      // when the duty cycle reaches 1000 it resets back to 0 and turns off the LED
                   }
               P2IES &= ~BIT1;          // Sets the button to detect rising edge
               P2IE |= BIT1;            // Enables the interrupt for P1.3 Button
               TA1CTL = MC_0;           // Stops the clock
               TA1CTL = TACLR;          // Clears the clock
               timer_state = 1;         // Brings the timer to the next state
               break;
           case 1:
               P2IES |= BIT1;           // Sets the button to detect falling edge
               P2IE |= BIT1;            // Enables the interrupt for P1.3 Button
               P2IFG &= ~BIT1;          // Clears the interrupt flag
               TA1CTL = MC_0;           // Stops the clock
               TA1CTL = TACLR;          // Clears the clock
               timer_state = 0;         // Brings the timer state back to 0
               break;
           }

}

