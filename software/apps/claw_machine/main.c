// Computer Bidirectional app
//
// Use serial to echo data typed by the user

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include "nrf_gpio.h"

#include "microbit_v2.h"
// #include "lsm303agr.h"

#include "app_timer.h"

#include "joystick.h"
#include "motor_driver.h"
#include "ir_sensor.h"
#include "music.h"


NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

int main(void) {
  // music_init();

  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;

  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;
  nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  pwm_init();


  nrf_gpio_cfg_output(EDGE_P10);
  nrf_gpio_cfg_output(EDGE_P11);
  nrf_gpio_cfg_output(EDGE_P12);

  nrf_gpio_cfg_input(EDGE_P8, GPIO_PIN_CNF_PULL_Disabled); // IR sensor

  while (1) {
    direction_t dir = joystick_get_value(&twi_mngr_instance);
    
    set_x_motor_direction(dir.x_direction);
    set_x_motor_speed(dir.x);

    set_y_motor_direction(dir.y_direction);
    set_y_motor_speed(dir.y);

    if (dir.is_button_active) {
      toggle_z_sequence();
      // reset home
      
      nrf_delay_ms(1000); // allow ir time to read the value
      
      if (!read_ir_sensor()) {
        // flash lights or whatever
        pwm_motor_kill();
        music_init();
        nrf_delay_ms(50);
        printf("THERE IS A THING");
        play_win();
        music_pwm_kill();
        pwm_init();
      } else {
        pwm_motor_kill();
        music_init();
        nrf_delay_ms(50);
        play_lose();
        music_pwm_kill();
        pwm_init();
      }

    }

    nrf_delay_ms(20);
  }
}

