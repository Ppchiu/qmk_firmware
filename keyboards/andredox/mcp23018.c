#include "mcp23018.h"

bool i2c_initialized = 0;
i2c_status_t mcp23018_status = 0x20;

// Initialize I/O Expander
uint8_t init_mcp23018(void) {
    mcp23018_status = 0x20;

    // Init once only
    if (i2c_initialized == 0) {
        i2c_init();  // on pins D(1,0)
        i2c_initialized = true;
        _delay_ms(1000);
    }

    // set pin direction
    // - unused  : input  : 1
    // - input   : input  : 1
    // - driving : output : 0
    mcp23018_status = i2c_start(I2C_ADDR_WRITE, I2C_TIMEOUT);    if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(IODIRA, I2C_TIMEOUT);            if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b01111111, I2C_TIMEOUT);        if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b00000000, I2C_TIMEOUT);        if (mcp23018_status) goto out;

    i2c_stop(I2C_TIMEOUT);

    // set pull-up
    // - unused  : on  : 1
    // - input   : on  : 1
    // - driving : off : 0
    mcp23018_status = i2c_start(I2C_ADDR_WRITE, I2C_TIMEOUT);    if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(GPPUA, I2C_TIMEOUT);             if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b01111111, I2C_TIMEOUT);        if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b00000000, I2C_TIMEOUT);        if (mcp23018_status) goto out;

    i2c_stop(I2C_TIMEOUT);

    // set logical value (doesn't matter on inputs)
    // - unused  : hi-Z : 1
    // - input   : hi-Z : 1
    // - driving : hi-Z : 1
    mcp23018_status = i2c_start(I2C_ADDR_WRITE, I2C_TIMEOUT);    if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(OLATA, I2C_TIMEOUT);             if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b11111111, I2C_TIMEOUT);        if (mcp23018_status) goto out;
    mcp23018_status = i2c_write(0b11111111, I2C_TIMEOUT);        if (mcp23018_status) goto out;

out:
    i2c_stop(I2C_TIMEOUT);

    // Enable Arduino LED if expander connected
    if (!mcp23018_status) {
        board_led_on();
    }
    return mcp23018_status;
}

// Check expander to see if it's still there
void check_mcp23018(void) {
    mcp23018_status = i2c_start(I2C_ADDR_WRITE, I2C_TIMEOUT);
    i2c_stop(I2C_TIMEOUT);

    // Enable Arduino LED if expander still connected, otherwise disable it
    if (mcp23018_status) {
        print("mcp23018 connection lost\n");
        board_led_off();
    } else {
        print("mcp23018 connection OK\n");
        board_led_on();
    }
}

// Set expander row pin low, set other rows high
void select_mcp23018_row(uint8_t row) {
    // If no error...
    if (!mcp23018_status) {
        // set active row low  : 0
        // set other rows hi-Z : 1
        mcp23018_status = i2c_start(I2C_ADDR_WRITE, I2C_TIMEOUT);        if (mcp23018_status) goto out;
        mcp23018_status = i2c_write(GPIOB, I2C_TIMEOUT);                 if (mcp23018_status) goto out;
        mcp23018_status = i2c_write(0xFF & ~(1<<row), I2C_TIMEOUT);      if (mcp23018_status) goto out;
    out:
        i2c_stop(I2C_TIMEOUT);
    }
}

// Read column output, return inverse
uint8_t read_mcp23018_cols(void) {

    // if there was an error, return 0
    if (mcp23018_status) {
        return 0;
    } else {
        uint8_t data = 0;
        // read all columns, invert read value
        mcp23018_status = i2c_start(I2C_ADDR_WRITE, I2C_TIMEOUT);    if (mcp23018_status) goto out;
        mcp23018_status = i2c_write(GPIOA, I2C_TIMEOUT);             if (mcp23018_status) goto out;
        mcp23018_status = i2c_start(I2C_ADDR_READ, I2C_TIMEOUT);     if (mcp23018_status) goto out;
        mcp23018_status = i2c_read_nack(I2C_TIMEOUT);                if (mcp23018_status < 0) goto out;
        data = ~((uint8_t)mcp23018_status);
        mcp23018_status = I2C_STATUS_SUCCESS;

    out:
        i2c_stop(I2C_TIMEOUT);
        return data;
    }
}
