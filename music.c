// PWM Square wave tone app
//
// Use PWM to play a tone over the speaker using a square wave

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"

// PWM configuration
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

nrfx_pwm_config_t config; 
nrfx_pwm_config_t *p_config = &config;


// Holds duty cycle values to trigger PWM toggle
nrf_pwm_values_common_t sequence_data[1] = {0};

// Sequence structure for configuring DMA
nrf_pwm_sequence_t pwm_sequence_1 = {
  .values.p_common = sequence_data,
  .length = 1,
  .repeats = 0,
  .end_delay = 0,
};


static void pwm_init_music(void) {


  p_config->base_clock = NRF_PWM_CLK_500kHz;
  p_config->count_mode = NRF_PWM_MODE_UP;
  p_config->step_mode = NRF_PWM_STEP_AUTO;
  p_config->top_value = 0;
  p_config->load_mode = NRF_PWM_LOAD_COMMON;

  
  p_config->output_pins[0] = SPEAKER_OUT;
  p_config->output_pins[1] = NRFX_PWM_PIN_NOT_USED;
  p_config->output_pins[2] = NRFX_PWM_PIN_NOT_USED;
  p_config->output_pins[3] = NRFX_PWM_PIN_NOT_USED;



  nrfx_pwm_init(&PWM_INST, p_config, NULL);

}

void play_tone(uint16_t frequency) {
  // Stop the PWM (and wait until its finished)
  nrfx_pwm_stop(&PWM_INST, true);


  // Set a countertop value based on desired tone frequency
  // You can access it as NRF_PWM0->COUNTERTOP
  NRF_PWM0->COUNTERTOP = 500000 / frequency;

  // Modify the sequence data to be a 25% duty cycle
  uint16_t volume = NRF_PWM0->COUNTERTOP / 2;
  sequence_data[0] = volume;

  // Start playback of the samples and loop indefinitely
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence_1, 1, NRFX_PWM_FLAG_LOOP);

}

void music_init(void) {
  printf("Board started!\n");

  pwm_init_music();
}

void music_pwm_kill(void) {
  nrfx_pwm_uninit(&PWM_INST);
}

void play_win(void) {
  play_tone(440);
  nrf_delay_ms(500);

  play_tone(554);
  nrf_delay_ms(500);

  play_tone(659);
  nrf_delay_ms(500);

  play_tone(880);
  nrf_delay_ms(500);

  nrfx_pwm_stop(&PWM_INST, true);
}

void play_lose(void) {

  play_tone(392);
  nrf_delay_ms(500);

  play_tone(369);
  nrf_delay_ms(500);

  play_tone(349);
  nrf_delay_ms(500);

  play_tone(329);
  nrf_delay_ms(500);

  nrfx_pwm_stop(&PWM_INST, true);
}