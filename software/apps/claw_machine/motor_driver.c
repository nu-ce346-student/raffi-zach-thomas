// PWM Square wave tone app

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_pwm.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"

#define Z_DOWN_TIME 48
#define Z_UP_TIME 58
#define BOTTOM_WAIT_TIME 60
#define RETURN_X_HOME 30
#define RETURN_Y_HOME 65

// PWM instance
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// Holds duty cycle values
nrf_pwm_values_individual_t channel_data = {0,0,500 * 10000,500 * 10000};

// PWM sequence configuration
nrf_pwm_sequence_t pwm_sequence = {
    .values.p_individual = &channel_data,
    .length = NRF_PWM_CHANNEL_COUNT,
    .repeats = 0,
    .end_delay = 0,
};

// PWM Initialization
void pwm_init(void) {
    nrfx_pwm_config_t config = {
        .output_pins = {EDGE_P0, EDGE_P1, EDGE_P2, EDGE_P3}, 
        .irq_priority = 0,
        .base_clock = NRF_PWM_CLK_500kHz,
        .count_mode = NRF_PWM_MODE_UP,
        .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode = NRF_PWM_STEP_AUTO,
    };

    nrfx_pwm_init(&PWM_INST, &config, NULL);

    NRF_PWM0->COUNTERTOP = (500 * 10000); // 500kHz clock
}

void pwm_motor_kill() {
  nrfx_pwm_uninit(&PWM_INST);
}

float get_duty_cycle(int displacement) {
  if (displacement > 500) {
    displacement = 500;
  }

  if (displacement < 0) {
    displacement *= -1;
  }

  return (1 - displacement/500.0);
}

// Play a tone on the external PWM pin
void set_x_motor_speed(int displacement) {
    while (!nrfx_pwm_stop(&PWM_INST, true));    

    channel_data.channel_0 = NRF_PWM0->COUNTERTOP * get_duty_cycle(displacement);

    nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void set_x_motor_direction(bool direction) {
  // true means right, false is left

  if (direction) {
    nrf_gpio_pin_set(EDGE_P10);
  } else {
    nrf_gpio_pin_clear(EDGE_P10);
  }
}

void set_y_motor_speed(int displacement) {
  while (!nrfx_pwm_stop(&PWM_INST, true));    

  channel_data.channel_1 = NRF_PWM0->COUNTERTOP * get_duty_cycle(displacement);

  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);

}

void set_y_motor_direction(bool direction) {
  // true means right, false is left

  if (direction) {
    nrf_gpio_pin_set(EDGE_P11);
  } else {
    nrf_gpio_pin_clear(EDGE_P11);
  }
}

void set_magnet_active() {
  while (!nrfx_pwm_stop(&PWM_INST, true));

  channel_data.channel_3 = 0;
  
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void set_magnet_inactive() {
  while (!nrfx_pwm_stop(&PWM_INST, true));

  channel_data.channel_3 = NRF_PWM0->COUNTERTOP;
  
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void return_to_home() {

  // nrf_gpio_pin_clear(EDGE_P10);

  // channel_data.channel_0 = 0;
  // channel_data.channel_1 = PWM_INST.p_registers->COUNTERTOP;
  
  // nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, RETURN_X_HOME, NRFX_PWM_FLAG_STOP);
  // while (!nrfx_pwm_is_stopped(&PWM_INST));

  nrf_gpio_pin_set(EDGE_P10);
  
  channel_data.channel_0 = 0;
  channel_data.channel_1 = PWM_INST.p_registers->COUNTERTOP;
  
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, RETURN_X_HOME, NRFX_PWM_FLAG_STOP);
  while (!nrfx_pwm_is_stopped(&PWM_INST));
  
  nrf_gpio_pin_clear(EDGE_P11);

  channel_data.channel_0 = PWM_INST.p_registers->COUNTERTOP;
  channel_data.channel_1 = 0;

  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, RETURN_Y_HOME, NRFX_PWM_FLAG_STOP);
  while (!nrfx_pwm_is_stopped(&PWM_INST));

  nrf_gpio_pin_set(EDGE_P11);
  channel_data.channel_1 = 0;
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 4, NRFX_PWM_FLAG_STOP);
  while (!nrfx_pwm_is_stopped(&PWM_INST));

  channel_data.channel_0 = PWM_INST.p_registers->COUNTERTOP;
  channel_data.channel_1 = PWM_INST.p_registers->COUNTERTOP;

  set_magnet_inactive();

  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void toggle_z_sequence() {
  nrfx_pwm_stop(&PWM_INST, true);
  while (!nrfx_pwm_is_stopped(&PWM_INST));

  // go down

  nrf_gpio_pin_set(EDGE_P12);
  channel_data.channel_2 = 0;

  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, Z_DOWN_TIME, NRFX_PWM_FLAG_STOP);
  while (!nrfx_pwm_is_stopped(&PWM_INST));

  // wait at bottom

  channel_data.channel_2 = PWM_INST.p_registers->COUNTERTOP;

  set_magnet_active();

  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, BOTTOM_WAIT_TIME, NRFX_PWM_FLAG_STOP);
  while (!nrfx_pwm_is_stopped(&PWM_INST));

  // go up

  nrf_gpio_pin_clear(EDGE_P12);
  channel_data.channel_2 = 0;

  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, Z_UP_TIME, NRFX_PWM_FLAG_STOP);
  while (!nrfx_pwm_is_stopped(&PWM_INST));


  nrf_gpio_pin_set(EDGE_P12);
  channel_data.channel_2 = 0;

  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 5, NRFX_PWM_FLAG_STOP);
  while (!nrfx_pwm_is_stopped(&PWM_INST));

  nrf_gpio_pin_set(EDGE_P12);

  // hold item

  channel_data.channel_2 = PWM_INST.p_registers->COUNTERTOP;

  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
  
  // go home

  return_to_home();


  // release item

}


