/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#ifndef ETHERCAT_SLAVE_H_
#define ETHERCAT_SLAVE_H_

#include <string>
#include <vector>
#include <json/json.h>

#define KELO_DRIVE_SLAVE 1
#define ROBILE_BATTERY_SLAVE 2
#define KELO_BMS_SLAVE 3

struct SlaveInfo
{
    std::string name; // e.g. KELOD105
    uint8_t slave_type; // e.g. KELO_DRIVE_SLAVE
    int slave_number; // index in the EtherCAT ring (1, 2, 3 etc.)
    int eep_man; // manufacturer
    int eep_rev; // revision
    int eep_id; // ID
    int address; //  configadr from ec_slave
    int rx_start_offset; // where in the datagram does the RX data start
    int tx_start_offset; // where in the datagram does the TX data start
};

class EthercatSlave
{
    public:
        EthercatSlave() {};
        virtual ~EthercatSlave() {};
        virtual void copyData(uint8_t *outputs, uint8_t *inputs) = 0;
        virtual void convertToJson(Json::Value &data) const = 0;
        virtual std::vector<std::string> getRxValues() = 0;
        virtual std::vector<std::string> getTxValues() = 0;
        virtual const std::vector<std::string>& getRxUnits() = 0;
        virtual const std::vector<std::string>& getTxUnits() = 0;
        virtual const std::vector<std::string>& getRxVariables() = 0;
        virtual const std::vector<std::string>& getTxVariables() = 0;
        virtual void parseBits(uint16_t data, const std::string &var_name, std::vector<std::string> &vars, std::vector<std::string> &vals) = 0;
        virtual bool areBitsParsable(const std::string &var_name) = 0;
        SlaveInfo slave_info;
};
#endif
