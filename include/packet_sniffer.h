/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#ifndef PACKET_SNIFFER_H_
#define PACKET_SNIFFER_H_

#include <tins/tins.h>
#include <thread>
#include <atomic>
#include "zmq_publisher.h"
#include <json/json.h>
#include "ethercat_data_source.h"

class PacketSniffer : public EthercatDataSource
{
    public:
        PacketSniffer(const std::string &ifname_or_filename, bool is_pcap_file, std::shared_ptr<ZMQPublisher> zmq_pub, std::string &error_msg);
        virtual ~PacketSniffer();
        std::vector<std::shared_ptr<EthercatSlave>>& getSlaves(std::string &error);
        void start(std::string &error);
        void stop();
        void setConfigFile(const std::string &filename, std::string &error_msg);

    private:
        std::shared_ptr<Tins::BaseSniffer> sniffer;
        std::thread sniffer_thread;

        Json::Value config;
        void loadConfig(const std::string &filename, std::string &error_msg);

        bool packetCallback(Tins::Packet &packet);
        void startSnifferLoop();

};
#endif
