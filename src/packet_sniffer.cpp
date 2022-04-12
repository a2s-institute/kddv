/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include "packet_sniffer.h"
#include "robile_battery_slave.h"
#include "kelo_drive_slave.h"
#include "kelo_bms_slave.h"
#include <iostream>
#include <fstream>
#include "ui.h"

PacketSniffer::PacketSniffer(const std::string &ifname_or_filename, bool is_pcap_file, std::shared_ptr<ZMQPublisher> zmq_pub, std::string &error_msg)
    : EthercatDataSource(zmq_pub)
{
    Tins::SnifferConfiguration sniffer_config;
    // https://gitlab.com/wireshark/wireshark/-/wikis/Protocols/ethercat
    sniffer_config.set_filter("ether proto 0x88a4");
    // without this, the packet capture will lag behind the packets being received
    sniffer_config.set_immediate_mode(true);

    if (is_pcap_file)
    {
        try
        {
            sniffer = std::make_shared<Tins::FileSniffer>(ifname_or_filename, sniffer_config);
        }
        catch (std::exception &e)
        {
            error_msg = e.what();
        }
    }
    else
    {
        try
        {
            sniffer = std::make_shared<Tins::Sniffer>(ifname_or_filename, sniffer_config);
        }
        catch (std::exception &e)
        {
            error_msg = e.what() + std::string(" (try running with sudo)");
        }
    }
}

PacketSniffer::~PacketSniffer()
{
    if (sniffer_thread.joinable()) sniffer_thread.join();
}

void PacketSniffer::setConfigFile(const std::string &filename, std::string &error_msg)
{
    loadConfig(filename, error_msg);
}

std::vector<std::shared_ptr<EthercatSlave>>& PacketSniffer::getSlaves(std::string &error)
{
    slaves.clear();
    if (config.empty())
    {
        error = "No config file specified";
        return slaves;
    }
    for (int i = 0; i < config["Slaves"].size(); i++)
    {
        std::shared_ptr<EthercatSlave> slave;
        uint8_t slave_type = getSlaveType(config["Slaves"][i]["Name"].asString());
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
        slave->slave_info.name = config["Slaves"][i]["Name"].asString();
        slave->slave_info.slave_number = config["Slaves"][i]["Slave ID"].asInt();
        slave->slave_info.eep_man = config["Slaves"][i]["EEP Man"].asInt();
        slave->slave_info.eep_id = config["Slaves"][i]["EEP ID"].asInt();
        slave->slave_info.eep_rev = config["Slaves"][i]["EEP Rev"].asInt();
        slave->slave_info.address = config["Slaves"][i]["Address"].asInt();
        slave->slave_info.rx_start_offset = config["Slaves"][i]["RX start offset"].asInt();
        slave->slave_info.tx_start_offset = config["Slaves"][i]["TX start offset"].asInt();
        slaves.push_back(slave);
    }
    return slaves;
}

void PacketSniffer::start(std::string &error)
{
    sniffer_thread = std::thread(&PacketSniffer::startSnifferLoop, this);
}

void PacketSniffer::stop()
{
    sniffer->stop_sniff();
    if (sniffer_thread.joinable()) sniffer_thread.join();
}

bool PacketSniffer::packetCallback(Tins::Packet &packet)
{
    Tins::PDU * pdu = packet.pdu();
    Tins::Timestamp timestamp = packet.timestamp();
    Tins::EthernetII &eth  = pdu->rfind_pdu<Tins::EthernetII>();
    Tins::PDU::serialization_type buffer = eth.serialize();

    // make sure we only process incoming packets (i.e. those that have completed
    // the cycle through all slaves and have the correct working count)
    if (eth.src_addr().to_string() != "03:01:01:01:01:01")
    {
        return true;
    }

    ec_comt ethercat_header;
    std::memcpy(&ethercat_header, &buffer[0] + eth.header_size(), sizeof(ec_comt));

    // don't process anything that's not a logical read write datagram
    if (ethercat_header.command != EC_CMD_LRW)
    {
        return true;
    }

    int datagram_size = ((int)(ethercat_header.dlength) & 0x0fff);

    int wkcnt = 0;
    std::memcpy(&wkcnt, &buffer[0] + eth.header_size() + sizeof(ec_comt) + datagram_size, 2);

    // each slave gets +3 for read/write: https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_io_intro/1446515467.html&id= 
    int expected_wkcnt = slaves.size() * 3;
    // TODO: add a callback to the UI to display errors
    if (expected_wkcnt != wkcnt)
    {
        std::cout << "Working counter is " << wkcnt << " but expected " << expected_wkcnt << std::endl;
    }

    double ts = timestamp.seconds() + (timestamp.microseconds() / 1000000.0);

    int start_offset = eth.header_size() + sizeof(ec_comt);
    int current_offset;


    for (int i = 0; i < slaves.size(); i++)
    {
        current_offset = start_offset + slaves[i]->slave_info.rx_start_offset;
        uint8 *rx_data = (uint8*)(&buffer[0] + current_offset);
        current_offset = start_offset + slaves[i]->slave_info.tx_start_offset;
        uint8 *tx_data = (uint8*)(&buffer[0] + current_offset);
        slaves[i]->copyData(rx_data, tx_data);
    }

    (ui->*callback_fn)(slaves);
    if (zmq_publish_enabled)
    {
        zmq_pub->publishMsg(convertToJson(slaves));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5)); // 200 Hz

    return true;

}
void PacketSniffer::startSnifferLoop()
{
    sniffer->sniff_loop(std::bind(&PacketSniffer::packetCallback, this, std::placeholders::_1));
}

void PacketSniffer::loadConfig(const std::string &filename, std::string &error_msg)
{
    std::ifstream infile(filename);
    if (infile)
    {
        infile >> config;
    }
    else
    {
        error_msg = "Could not open file "  + filename;
    }
}
