// PWM Square wave tone app
//
// Use PWM to play a tone over the speaker using a square wave

// #include <cstdio>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_pwm.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"

// PWM configuration
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// Holds duty cycle values to trigger PWM toggle
nrf_pwm_values_common_t sequence_data[1] = {0};

// Sequence structure for configuring DMA
nrf_pwm_sequence_t pwm_sequence = {
    .values.p_common = sequence_data,
    .length = 1,
    .repeats = 0,
    .end_delay = 0,
};

static void pwm_init(void) {
  // Initialize the PWM
  // SPEAKER_OUT is the output pin, mark the others as NRFX_PWM_PIN_NOT_USED
  // Set the clock to 500 kHz, count mode to Up, and load mode to Common
  // The Countertop value doesn't matter for now. We'll set it in play_tone()

  nrfx_pwm_config_t config = {
      .output_pins = {SPEAKER_OUT, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED,
                      NRFX_PWM_PIN_NOT_USED},
      .irq_priority = 0,
      .base_clock = NRF_PWM_CLK_500kHz,
      .count_mode = NRF_PWM_MODE_UP,
      .load_mode = NRF_PWM_LOAD_COMMON,
      .step_mode = NRF_PWM_STEP_AUTO,
  };
  nrfx_pwm_init(&PWM_INST, &config, NULL);
}

static void play_tone(uint16_t frequency) {
  // Stop the PWM (and wait until its finished)
  // TODO
  printf("waiting until stopped");
  while (!nrfx_pwm_stop(&PWM_INST, true))
    ;
  printf("stopped");

  // Set a countertop value based on desired tone frequency
  // You can access it as NRF_PWM0->COUNTERTOP
  // TODO
  NRF_PWM0->COUNTERTOP = (500 * 10E4) / frequency;

  // Modify the sequence data to be a 25% duty cycle
  // TODO
  sequence_data[0] = (NRF_PWM0->COUNTERTOP) / 4;

  // Start playback of the samples and loop indefinitely
  // TODO
  nrfx_pwm_flag_t flags = NRFX_PWM_FLAG_LOOP;
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, flags);
}

int main(void) {
  printf("Board started!\n");

  // initialize PWM
  pwm_init();

  // Play the A4 tone for one second
  // TODO
  play_tone(440);
  nrf_delay_ms(1000);

  // Play the C#5 tone for one second
  // TODO
  play_tone(554);
  nrf_delay_ms(1000);

  // Play the E5 tone for one second
  // TODO

  // Play the A5 tone for one second
  // TODO

  // Stop all noises
  // TODO
}