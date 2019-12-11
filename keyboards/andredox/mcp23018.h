#include <stdint.h>
#include <stdbool.h>
#include "i2c_master.h"
#include "print.h"
#include "andredox.h"
#include <util/delay.h>

// I2C aliases and register addresses (see "mcp23018.md")
#define I2C_ADDR        0x20
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

extern i2c_status_t mcp23018_status;
#define I2C_TIMEOUT 100

uint8_t init_mcp23018(void);
void check_mcp23018(void);
void select_mcp23018_row(uint8_t row);
uint8_t read_mcp23018_cols(void);
