#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include "joystick.h"

// Pointer to an initialized I2C instance to use for transactions
static const nrf_twi_mngr_t* i2c_manager = NULL;


static void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {
    //TODO: implement me
    //Note: there should only be a single two-byte transfer to be performed
    uint8_t buf[2] = {reg_addr, data};
  
    nrf_twi_mngr_transfer_t const write_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, buf, 2, 0)
    };
    ret_code_t result = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
    if (result != NRF_SUCCESS) {
      // Likely error codes:
      //  NRF_ERROR_INTERNAL            (0x0003) - something is wrong with the driver itself
      //  NRF_ERROR_INVALID_ADDR        (0x0010) - buffer passed was in Flash instead of RAM
      //  NRF_ERROR_BUSY                (0x0011) - driver was busy with another transfer still
      //  NRF_ERROR_DRV_TWI_ERR_OVERRUN (0x8200) - data was overwritten during the transaction
      //  NRF_ERROR_DRV_TWI_ERR_ANACK   (0x8201) - i2c device did not acknowledge its address
      //  NRF_ERROR_DRV_TWI_ERR_DNACK   (0x8202) - i2c device did not acknowledge a data byte
      printf("I2C transaction failed! Error: %lX\n", result);
    }
}

static uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) {
    uint8_t rx_buf = 0;
    nrf_twi_mngr_transfer_t const read_transfer[] = {
      NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
      NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0)
    };

    ret_code_t result = nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);
    if (result != NRF_SUCCESS) {
      // Likely error codes:
      //  NRF_ERROR_INTERNAL            (0x0003) - something is wrong with the driver itself
      //  NRF_ERROR_INVALID_ADDR        (0x0010) - buffer passed was in Flash instead of RAM
      //  NRF_ERROR_BUSY                (0x0011) - driver was busy with another transfer still
      //  NRF_ERROR_DRV_TWI_ERR_OVERRUN (0x8200) - data was overwritten during the transaction
      //  NRF_ERROR_DRV_TWI_ERR_ANACK   (0x8201) - i2c device did not acknowledge its address
      //  NRF_ERROR_DRV_TWI_ERR_DNACK   (0x8202) - i2c device did not acknowledge a data byte
      printf("I2C transaction failed! Error: %lX\n", result);
    }
  
    return rx_buf;
}

static inline void eliminate_drift(int* original) {
  int drift = fabs(*original);

  if (drift > 10) {
    return;
  } else {
    *original = 0;
  }
}



void joystick_init(const nrf_twi_mngr_t* i2c) {}

direction_t joystick_get_value(const nrf_twi_mngr_t* i2c) {
  i2c_manager = i2c;

    uint8_t button_val_1 = i2c_reg_read(JOYSTICK_ADDR, 0x07);
    uint8_t button_val = i2c_reg_read(JOYSTICK_ADDR, 0x08);

    uint16_t upper_x = i2c_reg_read(JOYSTICK_ADDR, 0x03);
    uint16_t lower_x = i2c_reg_read(JOYSTICK_ADDR, 0x04);
    uint16_t x = (upper_x << 8) | lower_x;
    int x_val = -((x >> 6) - x_offset);
    eliminate_drift(&x_val);

    uint16_t upper_y = i2c_reg_read(JOYSTICK_ADDR, 0x05);
    uint16_t lower_y = i2c_reg_read(JOYSTICK_ADDR, 0x06);
    uint16_t y = (upper_y << 8) | lower_y;
    int y_val = (y >> 6) - y_offset;
    eliminate_drift(&y_val);

    struct direction_t dir = {x_val, y_val, (x_val > 0), (y_val > 0), !button_val_1};

    return dir;
}
  