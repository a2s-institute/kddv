/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cstring>
#include "ui.h"
#include "ethercat_master.h"
#include "robile_battery_slave.h"
#include "kelo_drive_slave.h"
#include "kelo_bms_slave.h"
#include <ctime>

EthercatMaster::EthercatMaster(const std::string &ifname, std::shared_ptr<ZMQPublisher> zmq_pub) : EthercatDataSource(zmq_pub), ifname(ifname), ethercat_running(false)
{
}

EthercatMaster::~EthercatMaster()
{
    if (ethercat_running)
    {
        ethercat_running = false;
        if (ethercat_thread.joinable()) ethercat_thread.join();
        if (data_copy_thread.joinable()) data_copy_thread.join();
    }
}


std::vector<std::shared_ptr<EthercatSlave>>& EthercatMaster::getSlaves(std::string &error)
{
    slaves.clear();
    if (ec_init(ifname.c_str()))
    {
        if (ec_config(FALSE, &IOmap) > 0)
        {
            ec_configdc();
            ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 3);
            if (ec_slave[0].state != EC_STATE_SAFE_OP)
            {
                error = "Not in SAFE_OP state";
                return slaves;
            }
            ec_readstate();
            for (int cnt = 1; cnt <= ec_slavecount; cnt++)
            {
                uint8_t slave_type = getSlaveType(std::string(ec_slave[cnt].name));
                if (slave_type == KELO_DRIVE_SLAVE or slave_type == ROBILE_BATTERY_SLAVE)
                {
                    std::shared_ptr<EthercatSlave> slave;
                    if (slave_type == ROBILE_BATTERY_SLAVE)
                    {
                        slave = std::make_shared<RobileBatterySlave>();
                    }
                    else if (slave_type == KELO_DRIVE_SLAVE)
                    {
                        slave = std::make_shared<KeloDriveSlave>();
                    }
                    else if (slave_type == KELO_BMS_SLAVE)
                    {
                        slave = std::make_shared<KeloBMSSlave>();
                    }
                    slave->slave_info.slave_type = slave_type;
                    slave->slave_info.name = std::string(ec_slave[cnt].name);
                    slave->slave_info.slave_number = cnt;
                    slave->slave_info.eep_man = ec_slave[cnt].eep_man;
                    slave->slave_info.eep_rev = ec_slave[cnt].eep_rev;
                    slave->slave_info.eep_id = ec_slave[cnt].eep_id;
                    slave->slave_info.address = ec_slave[cnt].configadr;
                    // slave->slave_info.rx_start_offset = // do we really need this in ethercat mode?
                    // slave->slave_info.tx_start_offset = // do we really need this in ethercat mode?
                    slaves.push_back(slave);
                }
            }
        }
        else
        {
            error = "No slaves found";
            return slaves;
        }
        ec_close();
    }
    else
    {
        error = "No socket connection. (try running with sudo)";
        return slaves;
    }

    return slaves;
}

void EthercatMaster::start(std::string &error)
{
    if (ec_init(ifname.c_str()))
    {
        if (ec_config_init(FALSE) > 0)
        {
            ec_config_map(&IOmap);
            ec_configdc();
            ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);
            // each slave gets +3 for read/write: https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_io_intro/1446515467.html&id= 
            int expected_wkcnt = slaves.size() * 3;
            int wkcnt = 0;
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_writestate(0);
            unsigned int num_retries = 200;
            do
            {
                ec_send_processdata();
                wkcnt = ec_receive_processdata(EC_TIMEOUTRET);
                ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
            }
            while (num_retries-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
            if (ec_slave[0].state == EC_STATE_OPERATIONAL)
            {
                ethercat_running = true;
                ethercat_thread = std::thread(&EthercatMaster::ethercatLoop, this);
                data_copy_thread = std::thread(&EthercatMaster::dataCopyLoop, this);
            }
            if (expected_wkcnt != wkcnt)
            {
                // TODO: add a callback to the UI to display errors
                std::cout << "Working counter is " << wkcnt << " but expected " << expected_wkcnt << std::endl;
            }
        }
        else
        {
            error = "No slaves found";
        }
    }
    else
    {
        error = "No socket connection. (try running with sudo)";
    }
}

void EthercatMaster::stop()
{
    if (ethercat_running)
    {
        ethercat_running = false;
        if (ethercat_thread.joinable()) ethercat_thread.join();
        if (data_copy_thread.joinable()) data_copy_thread.join();
    }
}

void EthercatMaster::ethercatLoop()
{
    int wkcnt = 0;
    // each slave gets +3 for read/write: https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_io_intro/1446515467.html&id= 
    int expected_wkcnt = slaves.size() * 3;
    while (1)
    {
        if (!ethercat_running)
        {
            break;
        }
        ec_send_processdata();
        wkcnt = ec_receive_processdata(EC_TIMEOUTRET);
        if (expected_wkcnt != wkcnt)
        {
            // TODO: add a callback to the UI to display errors
            std::cout << "Working counter is " << wkcnt << " but expected " << expected_wkcnt << std::endl;
        }
        copyData();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    ec_close();
}

void EthercatMaster::copyData()
{
    // only copy if we can get ownership of the lock
    // https://stackoverflow.com/questions/33980860/use-stdlock-guard-with-try-lock/
    if (auto lock = std::unique_lock<std::mutex>(data_mutex, std::try_to_lock))
    {
        for (int i = 0; i < slaves.size(); i++)
        {
            slaves[i]->copyData(ec_slave[slaves[i]->slave_info.slave_number].outputs,
                                ec_slave[slaves[i]->slave_info.slave_number].inputs);
        }
    }
}
void EthercatMaster::dataCopyLoop()
{
    while (1)
    {
        if (!ethercat_running)
        {
            break;
        }

        // lock mutex within this scope
        {
            std::lock_guard<std::mutex> guard(data_mutex);
            // TODO: consider only creating a copy within this lock,
            // and publish the copied data outside the lock
            (ui->*callback_fn)(slaves);
            if (zmq_publish_enabled)
            {
                zmq_pub->publishMsg(convertToJson(slaves));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    ec_close();
}

