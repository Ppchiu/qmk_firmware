 #include QMK_KEYBOARD_H

extern keymap_config_t keymap_config;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _QWERTY 0
#define _SYMB 1

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  SYMB
};

#define SYM_L   MO(_SYMB)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_QWERTY] = LAYOUT(
//  k00, k01, k02, k03, k04, k05,                 k06, k07, k08, k09, k10, k11, 
//  k12, k13, k14, k15, k16, k17,                 k18, k19, k20, k21, k22, k23, 
//  k24, k25, k26, k27, k28, k29,                 k30, k31, k32, k33, k34, k35, 
//  k36, k37, k38, k39, k40, k41,                 k42, k43, k44, k45, k46, k47, 
//            k48, k49,                                     k50, k51,           
//                 k52, k53, k54,                 k55, k56, k57     ,           
//                      k58, k59,                 k60, k61,                     
     KC_GRV  ,KC_1    ,KC_2    ,KC_3    ,KC_4    ,KC_5    ,        KC_6    ,KC_7    ,KC_8    ,KC_9    ,KC_0    ,KC_MINS ,
     KC_TAB  ,KC_Q    ,KC_W    ,KC_E    ,KC_R    ,KC_T    ,        KC_Y    ,KC_U    ,KC_I    ,KC_O    ,KC_P    ,KC_BSLS ,
     KC_ESC  ,KC_A    ,KC_S    ,KC_D    ,KC_F    ,KC_G    ,        KC_H    ,KC_J    ,KC_K    ,KC_L    ,KC_SCLN ,KC_QUOT ,
     KC_LSFT ,KC_Z    ,KC_X    ,KC_C    ,KC_V    ,KC_B    ,        KC_N    ,KC_M    ,KC_COMM ,KC_DOT  ,KC_UP   ,KC_SLSH ,
                       KC_PSCR ,KC_LALT ,                          KC_EQL  ,KC_LEFT ,          
                       KC_1,    KC_LALT ,     KC_DEL ,                 KC_1   ,     KC_EQL  ,KC_LEFT,    
                                KC_LALT ,     KC_DEL ,                 KC_2   ,     KC_EQL      
  )

  //[_SYMB] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                                           ┌────────┬────────┬────────┬────────┬────────┬────────┐
  // XXXXXXX ,KC_F1   ,KC_F2   ,KC_F3   ,KC_F4   ,KC_F5   ,                                            KC_F6   ,KC_F7   ,KC_F8   ,KC_F9   ,KC_F10  ,XXXXXXX ,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐                         ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
  // XXXXXXX ,XXXXXXX ,KC_MS_U ,XXXXXXX ,KC_WH_U ,XXXXXXX ,KC_F11  ,                          KC_F12  ,XXXXXXX ,KC_KP_7 ,KC_KP_8 ,KC_KP_9 ,KC_PSLS ,XXXXXXX ,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┤                         ├────────┼────────┼────────┼────────┼────────┼────────┼────────┤
  // XXXXXXX ,KC_MS_L ,KC_MS_D ,KC_MS_R ,KC_WH_D ,XXXXXXX ,KC_VOLU ,                          KC_VOLD ,XXXXXXX ,KC_KP_4 ,KC_KP_5 ,KC_KP_6 ,KC_PAST ,XXXXXXX ,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┐       ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┤
  // XXXXXXX ,XXXXXXX ,XXXXXXX ,XXXXXXX ,XXXXXXX ,XXXXXXX ,KC_PAUS, KC_CAPS,         KC_SLCK ,KC_NLCK ,XXXXXXX ,KC_KP_1 ,KC_KP_2 ,KC_KP_3 ,KC_PMNS ,_______ ,
  //├────────┼────────┼────────┼────────┼────┬───┴────┬───┼────────┼────────┤       ├────────┼────────┼───┬────┴───┬────┼────────┼────────┼────────┼────────┤
  // _______ ,_______ ,_______ ,_______ ,     _______ ,    KC_BTN1 ,KC_BTN2 ,        KC_CUT  ,KC_PSTE ,    _______ ,     KC_KP_0 ,KC_PDOT ,KC_PPLS ,XXXXXXX
  //└────────┴────────┴────────┴────────┘    └────────┘   └────────┴────────┘       └────────┴────────┘   └────────┘    └────────┴────────┴────────┴────────┘
  //)

};
