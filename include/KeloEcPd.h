
struct __attribute__((packed)) EcPd_tx{
  uint32_t		status;			// Status bits as defined in STAT1_
  uint64_t      imu_ts;
  float         accel_x;
  float         accel_y;
  float         accel_z;
  float         gyro_x;
  float         gyro_y;
  float         gyro_z;
  float         imu_temperature;
  float         pressure;
  float         chargeport_voltage;
  float         enable_voltage;
  float         neopixel_voltage;
  float         bus_voltage;
  uint32_t      id1;
  uint16_t      status1;
  float         voltage1;
  float         current1;
  float         soc1;
  float         temperature1;
  float         cycles1;
  uint32_t      id2;
  uint16_t      status2;
  float         voltage2;
  float         current2;
  float         soc2;
  float         temperature2;
  float         cycles2;
};

struct __attribute__((packed)) EcPd_rx{
  uint32_t      command;
  uint16_t      bms1_command;
  uint16_t      bms2_command;
  uint16_t		neopixel_range1;
  uint32_t		neopixel_color1;
  uint16_t		neopixel_range2;
  uint32_t		neopixel_color2;
};


