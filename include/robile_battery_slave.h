/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#ifndef ROBILE_BATTERY_SLAVE_H_
#define ROBILE_BATTERY_SLAVE_H_

#include "ethercat_slave.h"
#include "RobileMasterBattery.h"

class RobileBatterySlave : public EthercatSlave
{
    public:
        RobileBatterySlave();
        virtual ~RobileBatterySlave();
        void copyData(uint8_t *outputs, uint8_t *inputs);
        void convertToJson(Json::Value &data) const;
        std::vector<std::string> getRxValues();
        std::vector<std::string> getTxValues();
        const std::vector<std::string>& getRxVariables();
        const std::vector<std::string>& getTxVariables();
        const std::vector<std::string>& getRxUnits();
        const std::vector<std::string>& getTxUnits();
        static const std::vector<std::string> tx_variables;
        static const std::vector<std::string> tx_units;
        static const std::vector<std::string> rx_variables;
        static const std::vector<std::string> rx_units;
        void parseBits(uint16_t data, const std::string &var_name, std::vector<std::string> &vars, std::vector<std::string> &vals);
        bool areBitsParsable(const std::string &var_name);

    private:
        RobileMasterBatteryProcessDataOutput rx;
        RobileMasterBatteryProcessDataInput tx;
};

#endif
