#include <stdbool.h>
#include "action.h"
#include "i2cmaster.h"
#include "expander.h"
#include "ergodone.h"
#include "debug.h"
#include <print.h>

#define ERGODOX_EZ_I2C_TIMEOUT 100

// I2C aliases and register addresses (see "mcp23018.md")
#define I2C_ADDR        0b0100000
#define I2C_ADDR_WRITE  ( (I2C_ADDR<<1) | I2C_WRITE )
#define I2C_ADDR_READ   ( (I2C_ADDR<<1) | I2C_READ  )
#define IODIRA          0x00            // i/o direction register
#define IODIRB          0x01
#define GPPUA           0x0C            // GPIO pull-up resistor register
#define GPPUB           0x0D
#define GPIOA           0x12            // general purpose i/o port register (write modifies OLAT)
#define GPIOB           0x13
#define OLATA           0x14            // output latch register
#define OLATB           0x15

static uint8_t expander_status = 0;
static uint8_t expander_input = 0;
bool i2c_initialized = 0;

int16_t mcp23018_status = 0x20;

void expander_config(void);
uint8_t expander_write(uint8_t reg, uint8_t data);
uint8_t expander_read(uint8_t reg, uint8_t *data);
uint8_t init_mcp23018(void);

void expander_init(void)
{
  uprintf("expander_init");
  i2c_init();
  expander_scan();
}

uint8_t init_mcp23018(void) {
    mcp23018_status = 0x20;

    // I2C subsystem

    // uint8_t sreg_prev;
    // sreg_prev=SREG;
    // cli();

    if (i2c_initialized == 0) {
        i2c_init();  // on pins D(1,0)
        i2c_initialized = true;
        _delay_ms(1000);
    }
    // i2c_init(); // on pins D(1,0)
    // _delay_ms(1000);

    // set pin direction
    // - unused  : input  : 1
    // - input   : input  : 1
    // - driving : output : 0
    mcp23018_status = i2c_start(I2C_ADDR_WRITE);    if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(IODIRA);            if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b00000000);        if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b00111111);        if (mcp23018_status) goto out;
    i2c_stop();

    // set pull-up
    // - unused  : on  : 1
    // - input   : on  : 1
    // - driving : off : 0
    mcp23018_status = i2c_start(I2C_ADDR_WRITE);    if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(GPPUA);             if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b00000000);        if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b00111111);        if (mcp23018_status) goto out;

out:
    i2c_stop();

    // SREG=sreg_prev;

    uprintf("Paul: expander status: %d ... ", mcp23018_status);
    return mcp23018_status;
}

void expander_scan(void)
{
  //uprintf("expander status: %d ... ", expander_status);
  uint8_t ret = i2c_start(EXPANDER_ADDR | I2C_WRITE);
  if (ret == 0) {
    i2c_stop();
    if (expander_status == 0) {
      uprintf("attached\n");
      expander_status = 1;
      expander_config();
      clear_keyboard();
    }
  }
  else {
    if (expander_status == 1) {
      uprintf("detached\n");
      expander_status = 0;
      clear_keyboard();
    }
  }
}

void expander_read_cols(void)
{
  expander_read(EXPANDER_REG_GPIOA, &expander_input);
}

uint8_t expander_get_col(uint8_t col)
{
  if (col > 4) {
    col++;
  }
  return expander_input & (1<<col) ? 1 : 0;
}

matrix_row_t expander_read_row(void)
{
    return 0;
  expander_read_cols();

  /* make cols */
  matrix_row_t cols = 0;
  for (uint8_t col = 0; col < MATRIX_COLS; col++) {
    if (expander_get_col(col)) {
      cols |= (1UL << (MATRIX_COLS - 1 - col));
    }
  }

  return cols;
}

void expander_unselect_rows(void)
{
  expander_write(EXPANDER_REG_IODIRB, 0xFF);
}

void expander_select_row(uint8_t row)
{
  expander_write(EXPANDER_REG_IODIRB, ~(1<<(row+1)));
}

void expander_config(void)
{
  //Invert the values of the registers of all the A input pins
  expander_write(EXPANDER_REG_IPOLA, 0xFF);
  //Enable pull up on all A input pins
  expander_write(EXPANDER_REG_GPPUA, 0xFF);
  //Set B pins to input
  expander_write(EXPANDER_REG_IODIRB, 0xFF);
}

uint8_t expander_write(uint8_t reg, uint8_t data)
{
  if (expander_status == 0) {
    return 0;
  }
  uint8_t ret;
  ret = i2c_start(EXPANDER_ADDR | I2C_WRITE);
  if (ret) goto stop;
  ret = i2c_write(reg);
  if (ret) goto stop;
  ret = i2c_write(data);
 stop:
  i2c_stop();
  return ret;
}

uint8_t expander_read(uint8_t reg, uint8_t *data)
{
  if (expander_status == 0) {
    return 0;
  }
  uint8_t ret;
  ret = i2c_start(EXPANDER_ADDR | I2C_WRITE);
  if (ret) goto stop;
  ret = i2c_write(reg);
  if (ret) goto stop;
  ret = i2c_rep_start(EXPANDER_ADDR | I2C_READ);
  if (ret) goto stop;
  *data = i2c_readNak();
 stop:
  i2c_stop();
  return ret;
}
