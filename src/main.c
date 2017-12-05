#include <stdbool.h>
#include <stdnoreturn.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


noreturn void main(void)
{

	int freq = 1;
	DDRD |= 0x01<<PD7;
	DDRD &= ~(0x01<<PD6);
	PORTD &= ~(0x01<<PD7);
	PORTD |= 0x01<<PD6;

	for (;;) {
		PORTD |= 0x01<<PD7;
		_delay_ms(1000 / freq);
		PORTD &= ~(0x01<<PD7);
		_delay_ms(1000 / freq);
		if (!(PIND&(0x01<<PD6))) {
			freq += 1;
			_delay_ms(1000);
		}
	}

}

