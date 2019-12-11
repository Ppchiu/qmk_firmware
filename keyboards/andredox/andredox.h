#pragma once

#include "quantum.h"
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00

#ifdef USE_I2C
#include <stddef.h>
#ifdef __AVR__
  #include <avr/io.h>
  #include <avr/interrupt.h>
#endif
#endif

void board_led_off(void);
void board_led_on(void);

#define LAYOUT( \
  k00, k01, k02, k03, k04, k05,                 k06, k07, k08, k09, k10, k11, \
  k12, k13, k14, k15, k16, k17,                 k18, k19, k20, k21, k22, k23, \
  k24, k25, k26, k27, k28, k29,                 k30, k31, k32, k33, k34, k35, \
  k36, k37, k38, k39, k40, k41,                 k42, k43, k44, k45, k46, k47, \
            k48, k49,                                     k50, k51,           \
                 k52, k53, k54,                 k55, k56, k57     ,           \
                      k58, k59,                 k60, k61                      \
) \
{ \
    { k00  , k01  , k02  , k03  , k04  , k05  , k06  , k07  , k08  , k09  , k10  , k11   }, \
    { k12  , k13  , k14  , k15  , k16  , k17  , k18  , k19  , k20  , k21  , k22  , k23   }, \
    { k24  , k25  , k26  , k27  , k28  , k29  , k30  , k31  , k32  , k33  , k34  , k35   }, \
    { k36  , k37  , k38  , k39  , k40  , k41  , k42  , k43  , k44  , k45  , k46  , k47   }, \
    { KC_NO, KC_NO, k48  , k49  , KC_NO, KC_NO, KC_NO, KC_NO, k50  , k51  , KC_NO, KC_NO }, \
    { KC_NO, KC_NO, KC_NO, k52  , k53  , k54  , k55  , k56  , k57  , k60  , k61  , KC_NO }, \
}
