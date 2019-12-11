/*
Copyright 2012-2018 Jun Wako, Jack Humbert, Yiancar

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include "wait.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "timer.h"
#include "quantum.h"
#include "mcp23018.h"
#include "andredox.h"

/* Set 0 if debouncing isn't needed */
#ifndef DEBOUNCING_DELAY
#   define DEBOUNCING_DELAY 5
#endif

#if (DEBOUNCING_DELAY > 0)
    static uint16_t debouncing_time;
    static bool debouncing = false;
#endif

#define print_matrix_header()  print("\nr/c 0123456789ABCDEF\n")
#define print_matrix_row(row)  print_bin_reverse16(matrix_get_row(row))
#define matrix_bitpop(i)       bitpop16(matrix[i])
#define ROW_SHIFTER ((uint16_t)1)

#ifdef MATRIX_MASKED
    extern const matrix_row_t matrix_mask[];
#endif

// Arduino row/col pins
static const pin_t row_pins[MATRIX_ROWS_TOTAL] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS_PER_SIDE] = MATRIX_COL_PINS;

// matrix state (1:on, 0:off)
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

// expander connectivity test loop counter
static uint16_t mcp23018_reset_loop = 0;

// Diode direction: COL2ROW
static void init_cols(void);
static bool read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row);
static void unselect_rows(void);
static void select_row(uint8_t row);
static void unselect_row(uint8_t row);

__attribute__ ((weak))
void matrix_init_quantum(void) {
    matrix_init_kb();
}

__attribute__ ((weak))
void matrix_scan_quantum(void) {
    matrix_scan_kb();
}

__attribute__ ((weak))
void matrix_init_kb(void) {
    matrix_init_user();
}

__attribute__ ((weak))
void matrix_scan_kb(void) {
    matrix_scan_user();
}

__attribute__ ((weak))
void matrix_init_user(void) {
}

__attribute__ ((weak))
void matrix_scan_user(void) {
}

inline
uint8_t matrix_rows(void) {
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void) {
    return MATRIX_COLS;
}

// Initialize switch matrix
void matrix_init(void) {

    // Initialize expander
    mcp23018_status = init_mcp23018();

    // Initialize Arduino
    unselect_rows();
    init_cols();

    // Set all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }

    matrix_init_quantum();
}

// Scan switch matrix for pressed keys
uint8_t matrix_scan(void)
{
    // Check that expander is connected every 2-3 seconds
    if (mcp23018_reset_loop == 8192) {
        mcp23018_reset_loop = 0;

        check_mcp23018();

        // Reset expander if there was an error
        if (mcp23018_status) {

            print("trying to reset mcp23018\n");
            mcp23018_status = init_mcp23018();
            if (mcp23018_status) {
                print("mcp23018 isn't responding\n");
            } else {
                print("mcp23018 responded!\n");
            }
        }
    } else {
        mcp23018_reset_loop++;
    }

    // Set row, read cols
    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {
#       if (DEBOUNCING_DELAY > 0)
            bool matrix_changed = read_cols_on_row(matrix_debouncing, current_row);

            if (matrix_changed) {
                debouncing = true;
                debouncing_time = timer_read();
            }

#       else
            read_cols_on_row(matrix, current_row);
#       endif

    }

#   if (DEBOUNCING_DELAY > 0)
        if (debouncing && (timer_elapsed(debouncing_time) > DEBOUNCING_DELAY)) {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                matrix[i] = matrix_debouncing[i];
            }
            debouncing = false;
        }
#   endif

    matrix_scan_quantum();
    return 1;
}

bool matrix_is_modified(void)
{
#if (DEBOUNCING_DELAY > 0)
    if (debouncing) return false;
#endif
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    // Matrix mask lets you disable switches in the returned matrix data. For example, if you have a
    // switch blocker installed and the switch is always pressed.
#ifdef MATRIX_MASKED
    return matrix[row] & matrix_mask[row];
#else
    return matrix[row];
#endif
}

void matrix_print(void)
{
    print_matrix_header();

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        print_matrix_row(row);
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += matrix_bitpop(i);
    }
    return count;
}

/* Column pin configuration
 *
 * Pro Micro: 6    5    4    3    2    1    0
 *            PB6  PB2  PB3  PB1  PF7  PF6  PF5

 * Expander:  13   12   11   10   9    8    7
 *            GPA7 GPA6 GPA4 GPA3 GPA2 GPA1 GPA0
 */

// Init cols: set all column pins to input / high
static void init_cols(void)
{
    for(uint8_t x = 0; x < MATRIX_COLS_PER_SIDE; x++) {
        setPinInputHigh(col_pins[x]);
        // Not needed for expander cols (done by init_mcp23018)
    }
}

// Read column pins for specified row
static bool read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row)
{
    // Store last value of row prior to reading
    matrix_row_t last_row_value = current_matrix[current_row];

    // Clear data in matrix row
    current_matrix[current_row] = 0;

    // Select row and wait for row selecton to stabilize
    select_row(current_row);
    wait_us(30);

    // For each col...
    for(uint8_t col_index = 0; col_index < MATRIX_COLS_PER_SIDE; col_index++) {

        // Select the col pin to read (active low)
        uint8_t arduino_pin_state = readPin(col_pins[col_index]);

        // Populate the matrix row with the state of the col pin
        current_matrix[current_row] |=  arduino_pin_state ? 0 : (ROW_SHIFTER << (col_index + 5));
    }

    // If expander is connected...
    if (!mcp23018_status) {
        // Read all expander col pins
        uint8_t expander_cols = read_mcp23018_cols();

        // Populate matrix row with the state of the expander col pins
        //current_matrix[current_row] |= ((expander_cols&127)<<7);
        current_matrix[current_row] |= ((expander_cols&127));
    }

    // Unselect row
    unselect_row(current_row);

    return (last_row_value != current_matrix[current_row]);
}

/* Row pin configuration
 *
 * Pro Micro: 0    1    2    3    4
 *            PD4  PD7  PE6  PB4  PB5
 *
 * Expander:  0    1    2    3    4
 *            GPB0 GPB1 GPB2 GPB3 GPB4
 */

// Set row to output, set pin to low
static void select_row(uint8_t row)
{
    setPinOutput(row_pins[row]);
    writePinLow(row_pins[row]);
    if (!mcp23018_status) {
        select_mcp23018_row(row);
    }
}

// Set pin to input / high
static void unselect_row(uint8_t row)
{
    setPinInputHigh(row_pins[row]);
    // Selecting row on expander unselects other rows
}

// Set all row pins to input
static void unselect_rows(void)
{
    for(uint8_t x = 0; x < MATRIX_ROWS; x++) {
        setPinInput(row_pins[x]);
    }
}
