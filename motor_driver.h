#pragma once
#include <stdint.h>



void pwm_init(void);

void pwm_motor_kill(void);

float get_duty_cycle(int displacement); 

void set_x_motor_speed(int displacement);

void set_x_motor_direction(bool direction);

void set_y_motor_speed(int displacement);

void set_y_motor_direction(bool direction);

void toggle_z_sequence();
