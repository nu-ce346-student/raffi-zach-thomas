// LSM303AGR driver for Microbit_v2
//
// Initializes sensor and communicates over I2C
// Capable of reading temperature, acceleration, and magnetic field strength

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "lsm303agr.h"
#include "nrf_delay.h"


// Pointer to an initialized I2C instance to use for transactions
static const nrf_twi_mngr_t* i2c_manager = NULL;

// Helper function to perform a 1-byte I2C read of a given register
//
// i2c_addr - address of the device to read from
// reg_addr - address of the register within the device to read
//
// returns 8-bit read value
static uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) {
  uint8_t rx_buf = 0;

  nrf_twi_mngr_transfer_t write_struct = NRF_TWI_MNGR_WRITE(
    i2c_addr,
    &reg_addr, 
    1,
    NRF_TWI_MNGR_NO_STOP
  );

  nrf_twi_mngr_transfer_t read_struct = NRF_TWI_MNGR_READ(
    i2c_addr,
    &rx_buf,
    1,
    0
  );

  nrf_twi_mngr_transfer_t const read_transfer[] = {
    write_struct, 
    read_struct
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

// Helper function to perform a 1-byte I2C write of a given register
//
// i2c_addr - address of the device to write to
// reg_addr - address of the register within the device to write
static void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {

  uint8_t n_data[2] = {reg_addr, data};
  
  printf("final data: %x\n", n_data);

  nrf_twi_mngr_transfer_t write_struct = NRF_TWI_MNGR_WRITE(
    i2c_addr, 
    &n_data,
    2,
    0
  );

  nrf_twi_mngr_transfer_t const write_transfer[] = {
    write_struct, 
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

// Initialize and configure the LSM303AGR accelerometer/magnetometer
//
// i2c - pointer to already initialized and enabled twim instance
void lsm303agr_init(const nrf_twi_mngr_t* i2c) {
  i2c_manager = i2c;

  // ---Initialize Accelerometer---

  // Reboot acclerometer
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, CTRL_REG5_A, 0x80);
  nrf_delay_ms(100); // needs delay to wait for reboot

  // Enable Block Data Update
  // Only updates sensor data when both halves of the data has been read
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, CTRL_REG4_A, 0x80);

  // Configure accelerometer at 100Hz, normal mode (10-bit)
  // Enable x, y and z axes
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, CTRL_REG1_A, 0x57);

  // Read WHO AM I register
  // Always returns the same value if working

  uint8_t a_whoami = i2c_reg_read(LSM303AGR_ACC_ADDRESS, WHO_AM_I_A);

  printf("Accelerometer whoami: 0x%x\n", a_whoami);
  
  // ---Initialize Magnetometer---

  // Reboot magnetometer
  i2c_reg_write(LSM303AGR_MAG_ADDRESS, CFG_REG_A_M, 0x40);
  nrf_delay_ms(100); // needs delay to wait for reboot

  // Enable Block Data Update
  // Only updates sensor data when both halves of the data has been read
  i2c_reg_write(LSM303AGR_MAG_ADDRESS, CFG_REG_C_M, 0x10);

  // Configure magnetometer at 100Hz, continuous mode
  i2c_reg_write(LSM303AGR_MAG_ADDRESS, CFG_REG_A_M, 0x0C);

  // Read WHO AM I register

  uint8_t m_whoami = i2c_reg_read(LSM303AGR_MAG_ADDRESS, WHO_AM_I_M);

  printf("Magnetometer whoami: 0x%x\n", m_whoami);

  // ---Initialize Temperature---

  // Enable temperature sensor
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, TEMP_CFG_REG_A, 0xC0);
}

// Read the internal temperature sensor
//
// Return measurement as floating point value in degrees C
float lsm303agr_read_temperature(void) {
  int8_t least_sig = i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_TEMP_L_A);
  int8_t most_sig = i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_TEMP_H_A);

  int16_t raw_temp = (most_sig << 8) | least_sig;

  float temp = (float) raw_temp;

  temp *= (1.0/256.0);

  temp += 25.0;

  return temp;
}

void read_temp_wrapper(void* _unused) {
  float ans = lsm303agr_read_temperature();

  printf("temp: %f\n", ans);
}

lsm303agr_measurement_t lsm303agr_read_accelerometer(void) {
  lsm303agr_measurement_t measurement = {0};

  int8_t x_ls = i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_X_L_A);
  int8_t x_ms = i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_X_H_A);

  int8_t y_ls = i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_Y_L_A);
  int8_t y_ms = i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_Y_H_A);

  int8_t z_ls = i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_Z_L_A);
  int8_t z_ms = i2c_reg_read(LSM303AGR_ACC_ADDRESS, OUT_Z_H_A);

  int16_t x_accel = (x_ms << 8) | x_ls;
  x_accel = (x_accel >> 6);
  float x_accel_f = (float) x_accel;
  x_accel_f /= 1000.0;
  x_accel_f *= 3.9;
  
  int16_t y_accel = (y_ms << 8) | y_ls;
  y_accel = (y_accel >> 6);
  float y_accel_f = (float) y_accel;
  y_accel_f /= 1000.0;
  y_accel_f *= 3.9;

  int16_t z_accel = (z_ms << 8) | z_ls;
  z_accel = (z_accel >> 6);
  float z_accel_f = (float) z_accel;
  z_accel_f /= 1000.0;
  z_accel_f *= 3.9;

  measurement.x_axis = x_accel_f;
  measurement.y_axis = y_accel_f;
  measurement.z_axis = z_accel_f;

  return measurement;
}

void read_acc_wrapper(void* _unused) {
  lsm303agr_measurement_t ans = lsm303agr_read_accelerometer();

  printf("%f, %f, %f\n", ans.x_axis, ans.y_axis, ans.z_axis);
}

lsm303agr_measurement_t lsm303agr_read_magnetometer(void) {
  lsm303agr_measurement_t measurement = {0};

  int8_t x_ls = i2c_reg_read(LSM303AGR_MAG_ADDRESS, OUTX_L_REG_M);
  int8_t x_ms = i2c_reg_read(LSM303AGR_MAG_ADDRESS, OUTX_H_REG_M);

  int16_t x_mag = (x_ms << 8) | x_ls;
  float x_mag_f = (float) x_mag;
  x_mag_f *= 1.5;
  x_mag_f /= 10.0;

  int8_t y_ls = i2c_reg_read(LSM303AGR_MAG_ADDRESS, OUTY_L_REG_M);
  int8_t y_ms = i2c_reg_read(LSM303AGR_MAG_ADDRESS, OUTY_H_REG_M);

  int16_t y_mag = (y_ms << 8) | y_ls;
  float y_mag_f = (float) y_mag;
  y_mag_f *= 1.5;
  y_mag_f /= 10.0;

  int8_t z_ls = i2c_reg_read(LSM303AGR_MAG_ADDRESS, OUTZ_L_REG_M);
  int8_t z_ms = i2c_reg_read(LSM303AGR_MAG_ADDRESS, OUTZ_H_REG_M);

  int16_t z_mag = (z_ms << 8) | z_ls;
  float z_mag_f = (float) z_mag;
  z_mag_f *= 1.5;
  z_mag_f /= 10.0;

  measurement.x_axis = x_mag_f;
  measurement.y_axis = y_mag_f;
  measurement.z_axis = z_mag_f;

  return measurement;
}


lsm303agr_measurement_t helper_accel(lsm303agr_measurement_t msr) {

  lsm303agr_measurement_t measurement = {0};

  float theta = atan((msr.x_axis)/sqrt(pow(msr.y_axis, 2)+pow(msr.z_axis, 2))) * 57.2958;
  float phi = atan((msr.y_axis)/sqrt((pow(msr.x_axis,2))+pow(msr.z_axis,2))) * 57.2958;
  float gamma = atan(sqrt(pow(msr.x_axis, 2)+pow(msr.y_axis, 2))/(msr.z_axis)) * 57.2958;

  measurement.x_axis = theta;
  measurement.y_axis = phi;
  measurement.z_axis = gamma;

  return measurement;
}

void read_mag_wrapper(void* _unused){
  lsm303agr_measurement_t ans = lsm303agr_read_magnetometer();

  lsm303agr_measurement_t ans2 = lsm303agr_read_accelerometer();

  lsm303agr_measurement_t ans1 = helper_accel(ans2);


  // printf("%f, %f, %f\n", ans.x_axis, ans.y_axis, ans.z_axis);
  printf("%f, %f, %f\n", ans1.x_axis, ans1.y_axis, ans1.z_axis);
}