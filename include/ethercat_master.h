/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#ifndef ETHERCAT_MASTER_H_
#define ETHERCAT_MASTER_H_

#include <thread>
#include <atomic>
#include <mutex>
#include "zmq_publisher.h"
#include <json/json.h>
#include "ethercat_data_source.h"


class EthercatMaster : public EthercatDataSource
{
    public:
        EthercatMaster(const std::string &ifname, std::shared_ptr<ZMQPublisher> zmq_pub);
        virtual ~EthercatMaster();
        std::vector<std::shared_ptr<EthercatSlave>>& getSlaves(std::string &error);
        void start(std::string &error);
        void stop();

    private:
        std::string ifname;
        char IOmap[4096];

        std::thread ethercat_thread;
        std::thread data_copy_thread;
        std::atomic_bool ethercat_running;

        std::mutex data_mutex;

        void ethercatLoop();
        void dataCopyLoop();
        void copyData();


};

#endif
