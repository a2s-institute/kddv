/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#ifndef ETHERCAT_DATA_SOURCE_H_
#define ETHERCAT_DATA_SOURCE_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "zmq_publisher.h"
#include <json/json.h>

extern "C" {
#include "ethercat.h"
}

#include "ethercat_slave.h"

std::vector<std::string> getNetworkInterfaces();
uint8_t getSlaveType(const std::string &name);

class UI;
typedef void(UI::*DataCallbackFunction)(const std::vector<std::shared_ptr<EthercatSlave>> &);

class EthercatDataSource
{
    public:
        EthercatDataSource(std::shared_ptr<ZMQPublisher> zmq_pub);
        virtual ~EthercatDataSource();
        virtual std::vector<std::shared_ptr<EthercatSlave>>& getSlaves(std::string &error) = 0;
        void setDataCallback(DataCallbackFunction callback_fn, UI *ui_obj);
        void setZMQPublish(bool value);
        virtual void start(std::string &error) = 0;
        virtual void stop() = 0;
    protected:
        std::vector<std::shared_ptr<EthercatSlave>> slaves;
        std::string convertToJson(const std::vector<std::shared_ptr<EthercatSlave>> &slaves);

        UI *ui;
        DataCallbackFunction callback_fn;

        bool zmq_publish_enabled;
        std::shared_ptr<ZMQPublisher> zmq_pub;
        Json::StreamWriterBuilder json_stream_builder;

};
#endif
