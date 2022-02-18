/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include "robile_battery_slave.h"
#include <cstring>

const std::vector<std::string> RobileBatterySlave::tx_variables =
{
    "timestamp",
    "status",
    "error",
    "warning",
    "output_current",
    "output_voltage",
    "output_power",
    "aux_port_current",
    "generic_data1",
    "generic_data2",
    "bmsm_pwr_device_id",
    "bmsm_status",
    "bmsm_voltage",
    "bmsm_current",
    "bmsm_temperature",
    "bmsm_soc",
    "bmsm_sn",
    "bmsm_bat_data1",
    "bmsm_bat_data2"
};

// TODO: these units need to be clarified
const std::vector<std::string> RobileBatterySlave::tx_units =
{
    "[ms]",
    "",
    "",
    "",
    "[A]",
    "[V]",
    "[W]",
    "[A]",
    "",
    "",
    "",
    "",
    "[V]",
    "[A]",
    "[K]",
    "",
    "",
    "",
    ""
};

const std::vector<std::string> RobileBatterySlave::rx_variables =
{
    "command1",
    "command2",
    "shutdown",
    "pwr_device_id"
};

const std::vector<std::string> RobileBatterySlave::rx_units =
{
    "",
    "",
    "",
    ""
};

const std::vector<std::string>& RobileBatterySlave::getRxVariables()
{
    return rx_variables;
}

const std::vector<std::string>& RobileBatterySlave::getTxVariables()
{
    return tx_variables;
}

const std::vector<std::string>& RobileBatterySlave::getRxUnits()
{
    return rx_units;
}

const std::vector<std::string>& RobileBatterySlave::getTxUnits()
{
    return tx_units;
}

RobileBatterySlave::RobileBatterySlave()
{
}

RobileBatterySlave::~RobileBatterySlave()
{
}

void RobileBatterySlave::copyData(uint8_t *outputs, uint8_t *inputs)
{
    std::memcpy(&rx, outputs, sizeof(RobileMasterBatteryProcessDataOutput));
    std::memcpy(&tx, inputs, sizeof(RobileMasterBatteryProcessDataInput));
}

void RobileBatterySlave::convertToJson(Json::Value &data) const
{
    data["commands"]["command1"] = rx.Command1;
    data["commands"]["command2"] = rx.Command2;
    data["commands"]["shutdown"] = rx.Shutdown;
    data["commands"]["pwr_device_id"] = rx.PwrDeviceId;

    data["sensors"]["timestamp"] = Json::Value::UInt64(tx.TimeStamp);
    data["sensors"]["status"] = tx.Status;
    data["sensors"]["error"] = tx.Error;
    data["sensors"]["warning"] = tx.Warning;
    data["sensors"]["output_current"] = tx.OutputCurrent;
    data["sensors"]["output_voltage"] = tx.OutputVoltage;
    data["sensors"]["output_power"] = tx.OutputPower;
    data["sensors"]["aux_port_current"] = tx.AuxPortCurrent;
    data["sensors"]["generic_data1"] = tx.GenericData1;
    data["sensors"]["generic_data2"] = tx.GenericData2;
    data["sensors"]["bmsm_pwr_device_id"] = tx.bmsm_PwrDeviceId;
    data["sensors"]["bmsm_status"] = tx.bmsm_Status;
    data["sensors"]["bmsm_voltage"] = tx.bmsm_Voltage;
    data["sensors"]["bmsm_current"] = tx.bmsm_Current;
    data["sensors"]["bmsm_temperature"] = tx.bmsm_Temperature;
    data["sensors"]["bmsm_soc"] = tx.bmsm_SOC;
    data["sensors"]["bmsm_sn"] = tx.bmsm_SN;
    data["sensors"]["bmsm_bat_data1"] = tx.bmsm_BatData1;
    data["sensors"]["bmsm_bat_data2"] = tx.bmsm_BatData2;
}

std::vector<std::string> RobileBatterySlave::getRxValues()
{
    std::vector<std::string> data_values;
    data_values.reserve(RobileBatterySlave::rx_variables.size());
    data_values.push_back(std::to_string(rx.Command1));
    data_values.push_back(std::to_string(rx.Command2));
    data_values.push_back(std::to_string(rx.Shutdown));
    data_values.push_back(std::to_string(rx.PwrDeviceId));
    return data_values;
}


std::vector<std::string> RobileBatterySlave::getTxValues()
{
    std::vector<std::string> data_values;
    data_values.reserve(RobileBatterySlave::tx_variables.size());
    data_values.push_back(std::to_string(tx.TimeStamp));
    data_values.push_back(std::to_string(tx.Status));
    data_values.push_back(std::to_string(tx.Error));
    data_values.push_back(std::to_string(tx.Warning));
    data_values.push_back(std::to_string(tx.OutputCurrent));
    data_values.push_back(std::to_string(tx.OutputVoltage));
    data_values.push_back(std::to_string(tx.OutputPower));
    data_values.push_back(std::to_string(tx.AuxPortCurrent));
    data_values.push_back(std::to_string(tx.GenericData1));
    data_values.push_back(std::to_string(tx.GenericData2));
    data_values.push_back(std::to_string(tx.bmsm_PwrDeviceId));
    data_values.push_back(std::to_string(tx.bmsm_Status));
    data_values.push_back(std::to_string(tx.bmsm_Voltage));
    data_values.push_back(std::to_string(tx.bmsm_Current));
    data_values.push_back(std::to_string(tx.bmsm_Temperature));
    data_values.push_back(std::to_string(tx.bmsm_SOC));
    data_values.push_back(std::to_string(tx.bmsm_SN));
    data_values.push_back(std::to_string(tx.bmsm_BatData1));
    data_values.push_back(std::to_string(tx.bmsm_BatData2));
    return data_values;
}

void RobileBatterySlave::parseBits(uint16_t data, const std::string &var_name, std::vector<std::string> &vars, std::vector<std::string> &vals)
{
    vars.clear();
    vals.clear();
    vars.reserve(7);
    vals.reserve(7);
    // see include/RobileMasterBattery.h for bit definitions
    if (var_name == "error")
    {
        vars.push_back("Low Battery");
        vals.push_back(std::to_string((data & 0x0001)));
        vars.push_back("Empty Frame");
        vals.push_back(std::to_string((data & 0x0002) >> 1));
        vars.push_back("Wrong Frame");
        vals.push_back(std::to_string((data & 0x0004) >> 2));
        vars.push_back("Wrong Charger");
        vals.push_back(std::to_string((data & 0x0008) >> 3));
        vars.push_back("Overload CHG");
        vals.push_back(std::to_string((data & 0x0010) >> 4));
        vars.push_back("Over current");
        vals.push_back(std::to_string((data & 0x0020) >> 5));
        vars.push_back("Watchdog");
        vals.push_back(std::to_string((data & 0x0040) >> 6));
    }
    else if (var_name == "warning")
    {
        vars.push_back("Over current");
        vals.push_back(std::to_string((data & 0x0001)));
        vars.push_back("Shutdown");
        vals.push_back(std::to_string((data & 0x0002) >> 1));
    }
}

bool RobileBatterySlave::areBitsParsable(const std::string &var_name)
{
    return (var_name == "error" or var_name == "warning");
}
