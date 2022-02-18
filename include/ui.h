/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#ifndef UI_H_
#define UI_H_

#include "zmq_publisher.h"
#include "ethercat_slave.h"
#include "ethercat_data_source.h"
#include <memory>

class UI
{
    public:
        UI(std::shared_ptr<ZMQPublisher> zmq_pub) : zmq_pub(zmq_pub) {};
        virtual ~UI() {};
        virtual void selectSource(const std::string &src) = 0;
        virtual void selectNetworkInterface(const std::string &iface) = 0;
        virtual void setConfigFile(const std::string &path) = 0;
        virtual void setPCAPFile(const std::string &path) = 0;
        virtual void enableZMQ(bool enable) = 0;
        virtual void start() = 0;
        virtual void dataCallback(const std::vector<std::shared_ptr<EthercatSlave>> &slaves) = 0;
    protected:
        std::shared_ptr<ZMQPublisher> zmq_pub;
        std::shared_ptr<EthercatDataSource> ecat_data_source;
        std::string config_file_name;
        std::string pcap_file_name;
};
#endif
