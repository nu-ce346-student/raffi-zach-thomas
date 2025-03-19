#include "ir_sensor.h"

bool read_ir_sensor() {
  bool sensor_value = nrf_gpio_pin_read(EDGE_P8) > 0 ? true : false;

  printf("The ir sensor is %i\n", sensor_value);

  return sensor_value;
}