/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include "ethercat_data_source.h"
#include <iostream>
#include <net/if.h>

EthercatDataSource::EthercatDataSource(std::shared_ptr<ZMQPublisher> zmq_pub) : zmq_pub(zmq_pub)
{
    zmq_publish_enabled = false;
    json_stream_builder["indentation"] = "";
}

EthercatDataSource::~EthercatDataSource()
{
}

void EthercatDataSource::setDataCallback(DataCallbackFunction callback_fn, UI *ui_obj)
{
    ui = ui_obj;
    this->callback_fn = callback_fn;
}

void EthercatDataSource::setZMQPublish(bool value)
{
    this->zmq_publish_enabled = value;
}

std::string EthercatDataSource::convertToJson(const std::vector<std::shared_ptr<EthercatSlave>> &slaves)
{

    auto millisec_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    double secs_since_epoch = millisec_since_epoch / 1000.0;
    Json::Value root;
    root["timestamp"] = secs_since_epoch;
    for (int i = 0; i < slaves.size(); i++)
    {
        Json::Value slaveData;
        slaves[i]->convertToJson(slaveData);
        root[slaves[i]->slave_info.name + " " + std::to_string(slaves[i]->slave_info.slave_number)] = slaveData;
    }
    std::string json_string = Json::writeString(json_stream_builder, root);
    return json_string;
}

uint8_t getSlaveType(const std::string &name)
{
    if (name == "KELO_ROBILE")
    {
        return ROBILE_BATTERY_SLAVE;
    }
    if (name == "SWMC" or name == "KELOD105")
    {
        return KELO_DRIVE_SLAVE;
    }
    return -1;
}

std::vector<std::string> getNetworkInterfaces()
{
    std::vector<std::string> interfaces;
    // https://stackoverflow.com/questions/19227781/linux-getting-all-network-interface-names
    struct if_nameindex *if_nidxs, *intf;

    if_nidxs = if_nameindex();
    if (if_nidxs != NULL)
    {
        for (intf = if_nidxs; intf->if_index != 0 || intf->if_name != NULL; intf++)
        {
            interfaces.push_back(std::string(intf->if_name));
        }

        if_freenameindex(if_nidxs);
    }

    return interfaces;
}
