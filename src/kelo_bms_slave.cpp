/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include "kelo_bms_slave.h"
#include <cstring>

const std::vector<std::string> KeloBMSSlave::tx_variables =
{
    "status",
    "imu_ts",
    "accel_x",
    "accel_y",
    "accel_z",
    "gyro_x",
    "gyro_y",
    "gyro_z",
    "imu_temperature",
    "pressure",
    "chargeport_voltage",
    "enable_voltage",
    "neopixel_voltage",
    "bus_voltage",
    "id1",
    "status1",
    "voltage1",
    "current1",
    "soc1",
    "temperature1",
    "cycles1",
    "id2",
    "status2",
    "voltage2",
    "current2",
    "soc2",
    "temperature2",
    "cycles2"
};

const std::vector<std::string> KeloBMSSlave::tx_units =
{
    "",
    "[ns]",
    "[m/s^2]",
    "[m/s^2]",
    "[m/s^2]",
    "[rad/s]",
    "[rad/s]",
    "[rad/s]",
    "[K]",
    "[Pa]",
    "[V]",
    "[V]",
    "[V]",
    "[V]",
    "",
    "",
    "[V]",
    "[A]",
    "",
    "[K]",
    "",
    "",
    "",
    "[V]",
    "[A]",
    "",
    "[K]",
    ""
};

const std::vector<std::string> KeloBMSSlave::rx_variables =
{
    "command",
    "bms1_command",
    "bms2_command",
    "neopixel_range1",
    "neopixel_color1",
    "neopixel_range2",
    "neopixel_color2",
};

// TODO: the units for the setpoints and limits can be derived from
// the commanded mode
const std::vector<std::string> KeloBMSSlave::rx_units =
{
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

const std::vector<std::string>& KeloBMSSlave::getRxVariables()
{
    return rx_variables;
}

const std::vector<std::string>& KeloBMSSlave::getTxVariables()
{
    return tx_variables;
}

const std::vector<std::string>& KeloBMSSlave::getRxUnits()
{
    return rx_units;
}

const std::vector<std::string>& KeloBMSSlave::getTxUnits()
{
    return tx_units;
}

KeloBMSSlave::KeloBMSSlave()
{
}

KeloBMSSlave::~KeloBMSSlave()
{
}

void KeloBMSSlave::copyData(uint8_t *outputs, uint8_t *inputs)
{
    std::memcpy(&rx, outputs, sizeof(EcPd_rx));
    std::memcpy(&tx, inputs, sizeof(EcPd_tx));
}

void KeloBMSSlave::convertToJson(Json::Value &data) const
{
    data["commands"]["command"] = rx.command;
    data["commands"]["bms1_command"] = rx.bms1_command;
    data["commands"]["bms2_command"] = rx.bms2_command;
    data["commands"]["neopixel_range1"] = rx.neopixel_range1;
    data["commands"]["neopixel_color1"] = rx.neopixel_color1;
    data["commands"]["neopixel_range2"] = rx.neopixel_range2;
    data["commands"]["neopixel_color2"] = rx.neopixel_color2;

    data["sensors"]["status"] = tx.status;
    data["sensors"]["imu_ts"] = Json::Value::UInt64(tx.imu_ts);
    data["sensors"]["accel_x"] = tx.accel_x;
    data["sensors"]["accel_y"] = tx.accel_y;
    data["sensors"]["accel_z"] = tx.accel_z;
    data["sensors"]["gyro_x"] = tx.gyro_x;
    data["sensors"]["gyro_y"] = tx.gyro_y;
    data["sensors"]["gyro_z"] = tx.gyro_z;
    data["sensors"]["imu_temperature"] = tx.imu_temperature;
    data["sensors"]["pressure"] = tx.pressure;
    data["sensors"]["chargeport_voltage"] = tx.chargeport_voltage;
    data["sensors"]["enable_voltage"] = tx.enable_voltage;
    data["sensors"]["neopixel_voltage"] = tx.neopixel_voltage;
    data["sensors"]["bus_voltage"] = tx.bus_voltage;
    data["sensors"]["id1"] = tx.id1;
    data["sensors"]["status1"] = tx.status1;
    data["sensors"]["voltage1"] = tx.voltage1;
    data["sensors"]["current1"] = tx.current1;
    data["sensors"]["soc1"] = tx.soc1;
    data["sensors"]["temperature1"] = tx.temperature1;
    data["sensors"]["cycles1"] = tx.cycles1;
    data["sensors"]["id2"] = tx.id2;
    data["sensors"]["status2"] = tx.status2;
    data["sensors"]["voltage2"] = tx.voltage2;
    data["sensors"]["current2"] = tx.current2;
    data["sensors"]["soc2"] = tx.soc2;
    data["sensors"]["temperature2"] = tx.temperature2;
    data["sensors"]["cycles2"] = tx.cycles2;
}

std::vector<std::string> KeloBMSSlave::getRxValues()
{
    std::vector<std::string> data_values;
    data_values.reserve(KeloBMSSlave::rx_variables.size());
    data_values.push_back(std::to_string(rx.command));
    data_values.push_back(std::to_string(rx.bms1_command));
    data_values.push_back(std::to_string(rx.bms2_command));
    data_values.push_back(std::to_string(rx.neopixel_range1));
    data_values.push_back(std::to_string(rx.neopixel_color1));
    data_values.push_back(std::to_string(rx.neopixel_range2));
    data_values.push_back(std::to_string(rx.neopixel_color2));
    return data_values;
}


std::vector<std::string> KeloBMSSlave::getTxValues()
{
    std::vector<std::string> data_values;
    data_values.reserve(KeloBMSSlave::tx_variables.size());
    data_values.push_back(std::to_string(tx.status));
    data_values.push_back(std::to_string(tx.imu_ts));
    data_values.push_back(std::to_string(tx.accel_x));
    data_values.push_back(std::to_string(tx.accel_y));
    data_values.push_back(std::to_string(tx.accel_z));
    data_values.push_back(std::to_string(tx.gyro_x));
    data_values.push_back(std::to_string(tx.gyro_y));
    data_values.push_back(std::to_string(tx.gyro_z));
    data_values.push_back(std::to_string(tx.imu_temperature));
    data_values.push_back(std::to_string(tx.pressure));
    data_values.push_back(std::to_string(tx.chargeport_voltage));
    data_values.push_back(std::to_string(tx.enable_voltage));
    data_values.push_back(std::to_string(tx.neopixel_voltage));
    data_values.push_back(std::to_string(tx.bus_voltage));
    data_values.push_back(std::to_string(tx.id1));
    data_values.push_back(std::to_string(tx.status1));
    data_values.push_back(std::to_string(tx.voltage1));
    data_values.push_back(std::to_string(tx.current1));
    data_values.push_back(std::to_string(tx.soc1));
    data_values.push_back(std::to_string(tx.temperature1));
    data_values.push_back(std::to_string(tx.cycles1));
    data_values.push_back(std::to_string(tx.id2));
    data_values.push_back(std::to_string(tx.status2));
    data_values.push_back(std::to_string(tx.voltage2));
    data_values.push_back(std::to_string(tx.current2));
    data_values.push_back(std::to_string(tx.soc2));
    data_values.push_back(std::to_string(tx.temperature2));
    data_values.push_back(std::to_string(tx.cycles2));
    return data_values;
}
bool KeloBMSSlave::areBitsParsable(const std::string &var_name)
{
    return false;
}

void KeloBMSSlave::parseBits(uint16_t data, const std::string &var_name, std::vector<std::string> &vars, std::vector<std::string> &vals)
{
}
