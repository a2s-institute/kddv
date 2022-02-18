/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include "ethercat_master.h"
#include "zmq_publisher.h"
#include "tui.h"
#include <memory>
#include <iostream>

void print_usage(const std::string &exec_name)
{
    std::cout << std::endl << "---------------------------------------" << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << exec_name
              << std::endl
              << "\t[--src INPUT_SOURCE]"
              << std::endl
              << "\t[--iface NETWORK_INTERFACE]"
              << std::endl
              << "\t[--config CONFIG_FILE]"
              << std::endl
              << "\t[--pcap PCAP_FILE]"
              << std::endl
              << "\t[--enable_zmq]"
              << std::endl
              << "\t[--zmq_port ZMQ_PORT]"
              << std::endl
              << "\t[--start]"
              << std::endl;
    std::cout << std::endl;
    std::cout << "INPUT_SOURCE: valid sources are\n\tecat\n\tsniffer\n\tpcap" << std::endl;
    std::vector<std::string> interfaces = getNetworkInterfaces();
    std::cout << "NETWORK_INTERFACE: valid interfaces are:" << std::endl;;
    for (int i = 0; i < interfaces.size(); i++)
    {
        std::cout << "\t" << interfaces[i] << std::endl;
    }
    std::cout << "---------------------------------------" << std::endl;
}

int main(int argc, char **argv)
{
    std::string input_source;
    std::string network_interface;
    std::string config_file;
    std::string pcap_file;
    bool start = false;
    std::string zmq_port = "9872";
    bool publish_zmq = false;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "--src") == 0)
            {
                if (argc <= i+1)
                {
                    std::cerr << "Specified argument " << argv[i] << " but did not provide a value " << std::endl;
                    print_usage(std::string(argv[0]));
                    return 1;
                }
                input_source = std::string(argv[i+1]);
                if (input_source != "ecat" and
                    input_source != "sniffer" and
                    input_source != "pcap")
                {
                    std::cerr << "Invalid input source " << input_source << std::endl;
                    std::cerr << "Valid sources are: 'ecat', 'sniffer' and 'pcap' " << std::endl;
                    print_usage(std::string(argv[0]));
                    return 1;
                }
                i += 1;
            }
            else if (strcmp(argv[i], "--iface") == 0)
            {
                if (argc <= i+1)
                {
                    std::cerr << "Specified argument " << argv[i] << " but did not provide a value " << std::endl;
                    print_usage(std::string(argv[0]));
                    return 1;
                }
                network_interface = std::string(argv[i+1]);
                i += 1;
            }
            else if (strcmp(argv[i], "--config") == 0)
            {
                if (argc <= i+1)
                {
                    std::cerr << "Specified argument " << argv[i] << " but did not provide a value " << std::endl;
                    print_usage(std::string(argv[0]));
                    return 1;
                }
                config_file = std::string(argv[i+1]);
                i += 1;
            }
            else if (strcmp(argv[i], "--pcap") == 0)
            {
                if (argc <= i+1)
                {
                    std::cerr << "Specified argument " << argv[i] << " but did not provide a value " << std::endl;
                    print_usage(std::string(argv[0]));
                    return 1;
                }
                pcap_file = std::string(argv[i+1]);
                i += 1;
            }
            else if (strcmp(argv[i], "--start") == 0)
            {
                start = true;
            }
            else if (strcmp(argv[i], "--zmq_port") == 0)
            {
                if (argc <= i+1)
                {
                    std::cerr << "Specified argument " << argv[i] << " but did not provide a value " << std::endl;
                    print_usage(std::string(argv[0]));
                    return 1;
                }
                zmq_port = std::string(argv[i+1]);
                i += 1;
            }
            else if (strcmp(argv[i], "--publish_zmq") == 0)
            {
                publish_zmq = true;
            }
            else
            {
                print_usage(std::string(argv[0]));
                return 1;
            }
        }
    }
    std::shared_ptr<ZMQPublisher> zmq_pub = std::make_shared<ZMQPublisher>(zmq_port);

    if (input_source.empty())
    {
        std::cerr << "No input source specified" << std::endl;
        print_usage(std::string(argv[0]));
        return 1;
    }
    if (input_source == "ecat")
    {
        if (network_interface.empty())
        {
            std::cerr << "No network interface specified" << std::endl;
            print_usage(std::string(argv[0]));
            return 1;
        }
    }
    else if (input_source == "sniffer")
    {
        if (network_interface.empty())
        {
            std::cerr << "No network interface specified" << std::endl;
            print_usage(std::string(argv[0]));
            return 1;
        }
        if (config_file.empty())
        {
            std::cerr << "No config file specified" << std::endl;
            print_usage(std::string(argv[0]));
            return 1;
        }
    }
    else if (input_source == "pcap")
    {
        if (config_file.empty())
        {
            std::cerr << "No config file specified" << std::endl;
            print_usage(std::string(argv[0]));
            return 1;
        }
        if (pcap_file.empty())
        {
            std::cerr << "No PCAP file specified" << std::endl;
            print_usage(std::string(argv[0]));
            return 1;
        }
    }

    TUI tui(zmq_pub);
    if (!input_source.empty())
    {
        tui.selectSource(input_source);
    }
    if (!network_interface.empty())
    {
        tui.selectNetworkInterface(network_interface);
    }
    if (!config_file.empty())
    {
        tui.setConfigFile(config_file);
    }
    if (!pcap_file.empty())
    {
        tui.setPCAPFile(pcap_file);
    }
    if (publish_zmq)
    {
        tui.enableZMQ(true);
    }
    tui.start();
}
