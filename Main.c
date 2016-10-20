/*
 * Main.c
 *
 *  Created on: 23-03-2013
 *      Author: slaw
 */
#define UART_BAUD_RATE      4800
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <uart.c>

#include "i2cmaster.h"
#include "pcf_helper.h"
char buffer[10];
/*
 * alarm
 */
void alarm(unsigned char a_h, unsigned char a_m, unsigned char a_s,date_time_t *dt) {

	if (dt->hours == a_h && dt->minutes == a_m && dt->seconds == a_s ) {
		PORTD&=~_BV(PIND7);//LED RED
		PORTD &= ~_BV(PIND6);
		_delay_ms(350);
		PORTD |= _BV(PIND6);
		_delay_ms(650);
		PORTD|=_BV(PIND7);//LED RED OFF
	}

}
int main(void) {
	date_time_t dt;

	i2c_init();
	DDRD = _BV(PIND6)|_BV(PIND7);
	PORTD = _BV(PIND6)|_BV(PIND7);

	DDRB = _BV(PINB2);
	PORTB = _BV(PINB2);

//	PORTD&=~_BV(PIND7);//LED RED
//	PORTB&=~_BV(PINB2);//LED GRN

    dt.month   = 10;    // December
    dt.day     = 5;    // 31
    dt.year    = 16;    // 2006
    dt.hours   = 20;    // 23 hours (11pm in 24-hour time)
    dt.minutes = 44;    // 59 minutes
    dt.seconds = 0;    // 50 seconds
    dt.weekday = 3;     // 0 = Sunday, 1 = Monday, etc.




    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
    TCCR0 |= (1<<CS00) | (1<<CS02);

	//ustawienie preskalera na 64

	TIMSK |= 1<<TOIE0;

	//w³¹czenie przerwania od przepe³nienia licznika

//	TCNT0 = 131;

	//ustawienie wartoœci pocz¹tkowej

    sei();
    uart_puts("\n\rstart");


   PCF8563_set_time(&dt);

	while (1) {

		PCF8563_get_time(&dt);

		itoa(dt.hours,buffer,10);
		uart_puts(buffer);
		uart_puts(":");

		itoa(dt.minutes,buffer,10);
		uart_puts(buffer);
		uart_puts(":");

		itoa(dt.seconds,buffer,10);
		uart_puts(buffer);

		alarm(0, 0, 0,&dt);
		alarm(4, 50, 0,&dt);

		uart_puts("\n\r");
		_delay_ms(750);
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
	};

	return (0);
}

ISR(TIMER0_OVF_vect){
	uart_puts("ovf");
}
