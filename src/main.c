#include <stdbool.h>
#include <stdnoreturn.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
 
// Calculate the value needed for 
// the CTC match value in OCR1A.
#define CTC_MATCH_OVERFLOW ((unsigned long)((F_CPU / 1000ul) / 8ul))

volatile unsigned long timer1_millis;
long milliseconds_since;
 
ISR(TIMER1_COMPA_vect)
{
    timer1_millis++;
}

unsigned long millis()
{
    unsigned long millis_return;

    // Ensure this cannot be disrupted
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        millis_return = timer1_millis;
    }
 
    return millis_return;
}


noreturn void main(void)
{
	unsigned long highfreq = 40;
	unsigned long lowfreq = 1;
	unsigned long highcnt = 0;
	unsigned long lowcnt = 0;

	// setup ports
	DDRD |= 0x01<<PD7; // high clock (out)
	DDRD |= 0x01<<PD6; // high clock (out)
	DDRD |= 0x01<<PD5; // high clock negated (out)
	DDRD |= 0x01<<PD4; // low clock (out)
	DDRD &= ~(0x01<<PD3); // high clock ctrl (in)
	DDRD &= ~(0x01<<PD2); // low clock ctrl (in)

	PORTD &= ~(0x01<<PD7); // off
	PORTD &= ~(0x01<<PD6); // off
	PORTD |=   0x01<<PD5;  // on
	PORTD &= ~(0x01<<PD4); // off
	PORTD |= 0x01<<PD3; // pull up
	PORTD |= 0x01<<PD2; // pull up

	// setup interrupts milli counter
	// CTC mode, Clock/8
	TCCR1B |= (1 << WGM12) | (1 << CS11);

	// Load the high byte, then the low byte
	// into the output compare
	OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
	OCR1AL = CTC_MATCH_OVERFLOW;

	// Enable the compare match interrupt
	TIMSK1 |= (1 << OCIE1A);

	// Now enable global interrupts
	sei();

	for (;;) {
		const unsigned long lowclk = lowfreq;
		const unsigned long highclk = highfreq;

		if ((millis() - highcnt) > (1000 / highclk)) {
			if (!(PORTD&(0x01<<PD7))) {
				PORTD |= 0x01<<PD7;
				PORTD |= 0x01<<PD6;
				PORTD &= ~(0x01<<PD5);
			} else {
				PORTD &= ~(0x01<<PD7);
				PORTD &= ~(0x01<<PD6);
				PORTD |= 0x01<<PD5;
			}
			highcnt = millis();
		}

		if ((millis() - lowcnt) > (1000 / lowclk)) {
			if (!(PORTD&(0x01<<PD4)))
				PORTD |= 0x01<<PD4;
			else
				PORTD &= ~(0x01<<PD4);
			lowcnt = millis();
		}

		if (!(PIND&(0x01<<PD3)))
			highfreq += 1;
		if (!(PIND&(0x01<<PD2)))
			lowfreq += 1;
	}

}

