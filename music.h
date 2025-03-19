#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"

void play_tone(uint16_t frequency);

void music_init(void);

void play_win(void);

void play_lose(void);

void music_pwm_kill(void);