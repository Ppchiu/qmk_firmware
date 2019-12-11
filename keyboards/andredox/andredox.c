#include "andredox.h"

// Keyboard init
void matrix_init_kb(void) {
	matrix_init_user();
};

// Enable red LED on Arduino
void board_led_on(void) {
    DDRB |= (1<<PB0);
    PORTB &= ~(1<<PB0);
}

// Disable red LED on Arduino
void board_led_off(void) {
    DDRB &= ~(1<<PB0);
    PORTB |= (1<<PB0);
}
