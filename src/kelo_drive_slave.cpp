/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include "kelo_drive_slave.h"
#include <cstring>

const std::vector<std::string> KeloDriveSlave::tx_variables =
{
    "status1",
    "status2",
    "sensor_ts",
    "setpoint_ts",
    "encoder_1",
    "velocity_1",
    "current_1_d",
    "current_1_q",
    "current_1_u",
    "current_1_v",
    "current_1_w",
    "voltage_1",
    "voltage_1_u",
    "voltage_1_v",
    "voltage_1_w",
    "temperature_1",
    "encoder_2",
    "velocity_2",
    "current_2_d",
    "current_2_q",
    "current_2_u",
    "current_2_v",
    "current_2_w",
    "voltage_2",
    "voltage_2_u",
    "voltage_2_v",
    "voltage_2_w",
    "temperature_2",
    "encoder_pivot",
    "velocity_pivot",
    "voltage_bus",
    "imu_ts",
    "accel_x",
    "accel_y",
    "accel_z",
    "gyro_x",
    "gyro_y",
    "gyro_z",
    "temperature_imu",
    "pressure",
    "current_in"
};

const std::vector<std::string> KeloDriveSlave::tx_units =
{
    "",
    "",
    "[ns]",
    "[ns]",
    "[rad]",
    "[rad/s]",
    "[A]",
    "[A]",
    "[A]",
    "[A]",
    "[A]",
    "[V]",
    "[V]",
    "[V]",
    "[V]",
    "[K]",
    "[rad]",
    "[rad/s]",
    "[A]",
    "[A]",
    "[A]",
    "[A]",
    "[A]",
    "[V]",
    "[V]",
    "[V]",
    "[V]",
    "[K]",
    "[rad]",
    "[rad/s]",
    "[V]",
    "[ns]",
    "[m/s^2]",
    "[m/s^2]",
    "[m/s^2]",
    "[rad/s]",
    "[rad/s]",
    "[rad/s]",
    "[K]",
    "[Pa]",
    "[A]",
};

const std::vector<std::string> KeloDriveSlave::rx_variables =
{
    "command1",
    "command2",
    "setpoint1",
    "setpoint2",
    "limit1_p",
    "limit1_n",
    "limit2_p",
    "limit2_n",
    "timestamp"
};

// TODO: the units for the setpoints and limits can be derived from
// the commanded mode
const std::vector<std::string> KeloDriveSlave::rx_units =
{
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "[ns]"
};

const std::vector<std::string>& KeloDriveSlave::getRxVariables()
{
    return rx_variables;
}

const std::vector<std::string>& KeloDriveSlave::getTxVariables()
{
    return tx_variables;
}

const std::vector<std::string>& KeloDriveSlave::getRxUnits()
{
    return rx_units;
}

const std::vector<std::string>& KeloDriveSlave::getTxUnits()
{
    return tx_units;
}

KeloDriveSlave::KeloDriveSlave()
{
}

KeloDriveSlave::~KeloDriveSlave()
{
}

void KeloDriveSlave::copyData(uint8_t *outputs, uint8_t *inputs)
{
    std::memcpy(&rx, outputs, sizeof(rxpdo1_t));
    std::memcpy(&tx, inputs, sizeof(txpdo1_t));
}

void KeloDriveSlave::convertToJson(Json::Value &data) const
{
    data["commands"]["command1"] = rx.command1;
    data["commands"]["command2"] = rx.command2;
    data["commands"]["setpoint1"] = rx.setpoint1;
    data["commands"]["setpoint2"] = rx.setpoint2;
    data["commands"]["limit1_p"] = rx.limit1_p;
    data["commands"]["limit1_n"] = rx.limit1_n;
    data["commands"]["limit2_p"] = rx.limit2_p;
    data["commands"]["limit2_n"] = rx.limit2_n;
    data["commands"]["timestamp"] = Json::Value::UInt64(rx.timestamp);

    data["sensors"]["status1"] = tx.status1;
    data["sensors"]["status2"] = tx.status2;
    data["sensors"]["sensor_ts"] = Json::Value::UInt64(tx.sensor_ts);
    data["sensors"]["setpoint_ts"] = Json::Value::UInt64(tx.setpoint_ts);
    data["sensors"]["encoder_1"] = tx.encoder_1;
    data["sensors"]["velocity_1"] = tx.velocity_1;
    data["sensors"]["current_1_d"] = tx.current_1_d;
    data["sensors"]["current_1_q"] = tx.current_1_q;
    data["sensors"]["current_1_u"] = tx.current_1_u;
    data["sensors"]["current_1_v"] = tx.current_1_v;
    data["sensors"]["current_1_w"] = tx.current_1_w;
    data["sensors"]["voltage_1"] = tx.voltage_1;
    data["sensors"]["voltage_1_u"] = tx.voltage_1_u;
    data["sensors"]["voltage_1_v"] = tx.voltage_1_v;
    data["sensors"]["voltage_1_w"] = tx.voltage_1_w;
    data["sensors"]["temperature_1"] = tx.temperature_1;
    data["sensors"]["encoder_2"] = tx.encoder_2;
    data["sensors"]["velocity_2"] = tx.velocity_2;
    data["sensors"]["current_2_d"] = tx.current_2_d;
    data["sensors"]["current_2_q"] = tx.current_2_q;
    data["sensors"]["current_2_u"] = tx.current_2_u;
    data["sensors"]["current_2_v"] = tx.current_2_v;
    data["sensors"]["current_2_w"] = tx.current_2_w;
    data["sensors"]["voltage_2"] = tx.voltage_2;
    data["sensors"]["voltage_2_u"] = tx.voltage_2_u;
    data["sensors"]["voltage_2_v"] = tx.voltage_2_v;
    data["sensors"]["voltage_2_w"] = tx.voltage_2_w;
    data["sensors"]["temperature_2"] = tx.temperature_2;
    data["sensors"]["encoder_pivot"] = tx.encoder_pivot;
    data["sensors"]["velocity_pivot"] = tx.velocity_pivot;
    data["sensors"]["voltage_bus"] = tx.voltage_bus;
    data["sensors"]["imu_ts"] = Json::Value::UInt64(tx.imu_ts);
    data["sensors"]["accel_x"] = tx.accel_x;
    data["sensors"]["accel_y"] = tx.accel_y;
    data["sensors"]["accel_z"] = tx.accel_z;
    data["sensors"]["gyro_x"] = tx.gyro_x;
    data["sensors"]["gyro_y"] = tx.gyro_y;
    data["sensors"]["gyro_z"] = tx.gyro_z;
    data["sensors"]["temperature_imu"] = tx.temperature_imu;
    data["sensors"]["pressure"] = tx.pressure;
    data["sensors"]["current_in"] = tx.current_in;
}

std::vector<std::string> KeloDriveSlave::getRxValues()
{
    std::vector<std::string> data_values;
    data_values.reserve(KeloDriveSlave::rx_variables.size());
    data_values.push_back(std::to_string(rx.command1));
    data_values.push_back(std::to_string(rx.command2));
    data_values.push_back(std::to_string(rx.setpoint1));
    data_values.push_back(std::to_string(rx.setpoint2));
    data_values.push_back(std::to_string(rx.limit1_p));
    data_values.push_back(std::to_string(rx.limit1_n));
    data_values.push_back(std::to_string(rx.limit2_p));
    data_values.push_back(std::to_string(rx.limit2_n));
    data_values.push_back(std::to_string(rx.timestamp));
    return data_values;
}


std::vector<std::string> KeloDriveSlave::getTxValues()
{
    std::vector<std::string> data_values;
    data_values.reserve(KeloDriveSlave::tx_variables.size());
    data_values.push_back(std::to_string(tx.status1));
    data_values.push_back(std::to_string(tx.status2));
    data_values.push_back(std::to_string(tx.sensor_ts));
    data_values.push_back(std::to_string(tx.setpoint_ts));
    data_values.push_back(std::to_string(tx.encoder_1));
    data_values.push_back(std::to_string(tx.velocity_1));
    data_values.push_back(std::to_string(tx.current_1_d));
    data_values.push_back(std::to_string(tx.current_1_q));
    data_values.push_back(std::to_string(tx.current_1_u));
    data_values.push_back(std::to_string(tx.current_1_v));
    data_values.push_back(std::to_string(tx.current_1_w));
    data_values.push_back(std::to_string(tx.voltage_1));
    data_values.push_back(std::to_string(tx.voltage_1_u));
    data_values.push_back(std::to_string(tx.voltage_1_v));
    data_values.push_back(std::to_string(tx.voltage_1_w));
    data_values.push_back(std::to_string(tx.temperature_1));
    data_values.push_back(std::to_string(tx.encoder_2));
    data_values.push_back(std::to_string(tx.velocity_2));
    data_values.push_back(std::to_string(tx.current_2_d));
    data_values.push_back(std::to_string(tx.current_2_q));
    data_values.push_back(std::to_string(tx.current_2_u));
    data_values.push_back(std::to_string(tx.current_2_v));
    data_values.push_back(std::to_string(tx.current_2_w));
    data_values.push_back(std::to_string(tx.voltage_2));
    data_values.push_back(std::to_string(tx.voltage_2_u));
    data_values.push_back(std::to_string(tx.voltage_2_v));
    data_values.push_back(std::to_string(tx.voltage_2_w));
    data_values.push_back(std::to_string(tx.temperature_2));
    data_values.push_back(std::to_string(tx.encoder_pivot));
    data_values.push_back(std::to_string(tx.velocity_pivot));
    data_values.push_back(std::to_string(tx.voltage_bus));
    data_values.push_back(std::to_string(tx.imu_ts));
    data_values.push_back(std::to_string(tx.accel_x));
    data_values.push_back(std::to_string(tx.accel_y));
    data_values.push_back(std::to_string(tx.accel_z));
    data_values.push_back(std::to_string(tx.gyro_x));
    data_values.push_back(std::to_string(tx.gyro_y));
    data_values.push_back(std::to_string(tx.gyro_z));
    data_values.push_back(std::to_string(tx.temperature_imu));
    data_values.push_back(std::to_string(tx.pressure));
    data_values.push_back(std::to_string(tx.current_in));
    return data_values;
}
bool KeloDriveSlave::areBitsParsable(const std::string &var_name)
{
    return (var_name == "command1" or var_name == "status1");
}

void KeloDriveSlave::parseBits(uint16_t data, const std::string &var_name, std::vector<std::string> &vars, std::vector<std::string> &vals)
{
    vars.clear();
    vals.clear();
    vars.reserve(15);
    vals.reserve(15);
    // see include/KeloDriveAPI.h for bit definitions
    if (var_name == "command1")
    {
        vars.push_back("ENABLE1");
        vals.push_back(std::to_string((data & COM1_ENABLE1) >> 0));
        vars.push_back("ENABLE2");
        vals.push_back(std::to_string((data & COM1_ENABLE2) >> 1));
        vars.push_back("MODE_TORQUE");
        vars.push_back("MODE_DTORQUE");
        vars.push_back("MODE_VELOCITY");
        vars.push_back("MODE_DVELOCITY");

        // mask out all but bits 3 and 4, then check which mode
        int mode = int(data & (0x3 << 2));
        if (mode == COM1_MODE_TORQUE)
        {
            vals.push_back("1");
            vals.push_back("0");
            vals.push_back("0");
            vals.push_back("0");
        }
        else if (mode == COM1_MODE_DTORQUE)
        {
            vals.push_back("0");
            vals.push_back("1");
            vals.push_back("0");
            vals.push_back("0");
        }
        else if (mode == COM1_MODE_VELOCITY)
        {
            vals.push_back("0");
            vals.push_back("0");
            vals.push_back("1");
            vals.push_back("0");
        }
        else if (mode == COM1_MODE_DVELOCITY)
        {
            vals.push_back("0");
            vals.push_back("0");
            vals.push_back("0");
            vals.push_back("1");
        }
        else
        {
            vals.push_back("0");
            vals.push_back("0");
            vals.push_back("0");
            vals.push_back("0");
        }
        vars.push_back("EMERGENCY1");
        vals.push_back(std::to_string((data & COM1_EMERGENCY1) >> 4));
        vars.push_back("EMERGENCY2");
        vals.push_back(std::to_string((data & COM1_EMERGENCY2) >> 5));
        vars.push_back("ENABLESERVO");
        vals.push_back(std::to_string((data & COM1_ENABLESERVO) >> 10));
        vars.push_back("SERVOCLOSE");
        vals.push_back(std::to_string((data & COM1_SERVOCLOSE) >> 11));
        vars.push_back("USE_TS");
        vals.push_back(std::to_string((data & COM1_USE_TS) >> 15));
    }
    else if (var_name == "status1")
    {
        vars.push_back("ENABLED1");
        vars.push_back("ENABLED2");
        vars.push_back("ENC_1_OK");
        vars.push_back("ENC_2_OK");
        vars.push_back("ENC_PIVOT_OK");
        vars.push_back("UNDERVOLTAGE");
        vars.push_back("OVERVOLTAGE");
        vars.push_back("OVERCURRENT_1");
        vars.push_back("OVERCURRENT_2");
        vars.push_back("OVERTEMP_1");
        vars.push_back("OVERTEMP_2");
        vars.push_back("ENABLED_GRIP");
        vars.push_back("INPOS_GRIP");
        vars.push_back("OVERLOAD_GRIP");
        vars.push_back("DETECT");
        vals.push_back(std::to_string((data & STAT1_ENABLED1) >> 0));
        vals.push_back(std::to_string((data & STAT1_ENABLED2) >> 1));
        vals.push_back(std::to_string((data & STAT1_ENC_1_OK) >> 2));
        vals.push_back(std::to_string((data & STAT1_ENC_2_OK) >> 3));
        vals.push_back(std::to_string((data & STAT1_ENC_PIVOT_OK) >> 4));
        vals.push_back(std::to_string((data & STAT1_UNDERVOLTAGE) >> 5));
        vals.push_back(std::to_string((data & STAT1_OVERVOLTAGE) >> 6));
        vals.push_back(std::to_string((data & STAT1_OVERCURRENT_1) >> 7));
        vals.push_back(std::to_string((data & STAT1_OVERCURRENT_2) >> 8));
        vals.push_back(std::to_string((data & STAT1_OVERTEMP_1) >> 9));
        vals.push_back(std::to_string((data & STAT1_OVERTEMP_2) >> 10));
        vals.push_back(std::to_string((data & STAT1_ENABLED_GRIP) >> 11));
        vals.push_back(std::to_string((data & STAT1_INPOS_GRIP) >> 12));
        vals.push_back(std::to_string((data & STAT1_OVERLOAD_GRIP) >> 13));
        vals.push_back(std::to_string((data & STAT1_DETECT) >> 14));
    }
}
