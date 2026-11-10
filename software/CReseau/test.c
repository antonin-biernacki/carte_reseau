// avr-gcc -mmcu=atmega32u4 -Wall -Werror -I. -DF_CPU=16000000 -Os -o test.elf test.c
// avr-objcopy -j .text -j .data -O ihex test.elf test.hex


// Include
 
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 
// Define
 
#define LED1 0
#define LED2 1
#define LED3 6
#define LED4 5
#define LED5 4
#define CS 5




void led_init() {
    DDRD |= (1<<LED1);
	DDRD |= (1<< LED2);
	DDRD |= (1<< LED3);
	DDRD |= (1<< LED4);
	DDRD |= (1<< LED5);
	DDRB |= (1<< CS);

}


void leds_blink() {
	PORTD ^= (1<<LED1);
	PORTD ^= (1<<LED2);
	PORTD ^= (1<<LED3);
	PORTD ^= (1<<LED4);
	PORTD ^= (1<<LED5);
    PORTB ^= (1<<CS);
    
	_delay_ms(500);
}



int main() {
    led_init();
    while(1) {
        leds_blink();
    }
	return 0;
}
