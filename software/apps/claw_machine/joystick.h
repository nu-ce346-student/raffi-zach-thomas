#pragma once
#include "nrf_twi_mngr.h"

static const uint8_t JOYSTICK_ADDR = 0x20;

static const uint16_t y_offset = 512;
static const uint16_t x_offset = 506;

typedef struct direction_t {
  int x;
  int y;
  bool x_direction; // true is right, left is false
  bool y_direction;
  bool is_button_active; // true means button is pressed
} direction_t;


void joystick_init(const nrf_twi_mngr_t* i2c);

direction_t joystick_get_value(const nrf_twi_mngr_t* i2c);
