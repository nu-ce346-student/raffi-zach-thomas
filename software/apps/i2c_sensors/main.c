// I2C sensors app
//
// Read from I2C accelerometer/magnetometer on the Microbit

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "nrf_delay.h"
#include "nrf_twi_mngr.h"

#include "microbit_v2.h"
#include "lsm303agr.h"

#include "app_timer.h"

// Global variables
NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);
APP_TIMER_DEF(temp_timer);
APP_TIMER_DEF(other_timer);
APP_TIMER_DEF(mag_timer);

int main(void) {
  printf("Board started!\n");

  // Initialize I2C peripheral and driver
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  // WARNING!!
  // These are NOT the correct pins for external I2C communication.
  // If you are using QWIIC or other external I2C devices, the are
  // connected to EDGE_P19 (a.k.a. I2C_QWIIC_SCL) and EDGE_P20 (a.k.a. I2C_QWIIC_SDA)
  i2c_config.scl = I2C_INTERNAL_SCL;
  i2c_config.sda = I2C_INTERNAL_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;
  nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);

  // Initialize the LSM303AGR accelerometer/magnetometer sensor
  lsm303agr_init(&twi_mngr_instance);

  //TODO: implement me!

  app_timer_init();
  app_timer_create(&temp_timer, APP_TIMER_MODE_REPEATED, read_temp_wrapper);
  app_timer_create(&other_timer, APP_TIMER_MODE_REPEATED, read_acc_wrapper);
  app_timer_create(&mag_timer, APP_TIMER_MODE_REPEATED, read_mag_wrapper);
  app_timer_start(temp_timer, 32768, NULL);
  app_timer_start(other_timer, 32768, NULL);
  app_timer_start(mag_timer, 32768, NULL);

  // Loop forever
  while (1) {
    // Don't put any code in here. Instead put periodic code in a callback using a timer.
    nrf_delay_ms(1000);
  }
}

