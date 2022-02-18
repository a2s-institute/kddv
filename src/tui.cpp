/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include "tui.h"
#include "ethercat_master.h"
#include "packet_sniffer.h"
#include <iostream>

TUI::TUI(std::shared_ptr<ZMQPublisher> zmq_pub) : UI(zmq_pub)
{
    enable_zmq = false;
    setupWindow();
    selected_slave = 0;
}

TUI::~TUI()
{
    delwin(main_window);
    delwin(status_window);
    endwin();
}

void TUI::setupWindow()
{
    initscr();
    cbreak();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    timeout(0);
    refresh();
    int screen_rows, screen_cols;
    getmaxyx(stdscr, screen_rows, screen_cols);

    main_window = newwin(screen_rows - 1, screen_cols, 0, 0);
    wrefresh(main_window);

    std::string msg = "Quit: q, Left: <, Right: >";
    instructions_window = newwin(1, msg.size(), screen_rows - 1, screen_cols - msg.size());
    mvwprintw(instructions_window, 0, 0, msg.c_str());
    wrefresh(instructions_window);

    status_window = newwin(1, screen_cols-msg.size(), screen_rows - 1, 0);
    scrollok(status_window, TRUE);
    wrefresh(status_window);
    refresh();
}

void TUI::selectSource(const std::string &src)
{
    ecat_src = src;
}

void TUI::selectNetworkInterface(const std::string &iface)
{
    network_interface = iface;
}

void TUI::setConfigFile(const std::string &path)
{
    config_file_name = path;
}

void TUI::setPCAPFile(const std::string &path)
{
    pcap_file_name = path;
}

void TUI::enableZMQ(bool enable)
{
    enable_zmq = enable;
}

void TUI::dataCallback(const std::vector<std::shared_ptr<EthercatSlave>> &slaves)
{
    if (selected_slave < 0)
    {
        selected_slave = 0;
    }
    if (selected_slave >= slaves.size())
    {
        selected_slave = slaves.size() - 1;
    }
    werase(main_window);

    int width_per_column = 40;
    // heading line
    mvwhline(main_window, 1, 0, ACS_HLINE, 20 * slaves.size());
    // write names of all slaves
    for (int i = 0; i < slaves.size(); i++)
    {
        std::string name = slaves[i]->slave_info.name + " " + std::to_string(slaves[i]->slave_info.slave_number);
        if (i == selected_slave)
        {
            wattron(main_window, A_STANDOUT);
        }
        mvwprintw(main_window, 0, i*20, name.c_str());
        if (i == selected_slave)
        {
            wattroff(main_window, A_STANDOUT);
        }
    }
    // display data for selected slave
    const std::vector<std::string> &rx_vars = slaves[selected_slave]->getRxVariables();
    const std::vector<std::string> &tx_vars = slaves[selected_slave]->getTxVariables();
    std::vector<std::string> rx_vals = slaves[selected_slave]->getRxValues();
    std::vector<std::string> tx_vals = slaves[selected_slave]->getTxValues();
    for (int j = 0; j < rx_vars.size(); j++)
    {
        int length = rx_vals[j].length();
        int rx_end = width_per_column - 4;
        int val_start = rx_end - length;
        mvwprintw(main_window, j+2, 0, rx_vars[j].c_str());
        mvwprintw(main_window, j+2, val_start, rx_vals[j].c_str());
    }
    for (int j = 0; j < tx_vars.size(); j++)
    {
        int length = tx_vals[j].length();
        int tx_end = (width_per_column * 2) - 4;
        int val_start = tx_end - length;
        int var_start = width_per_column;
        mvwprintw(main_window, j+2, var_start, tx_vars[j].c_str());
        mvwprintw(main_window, j+2, val_start, tx_vals[j].c_str());
    }
    // vertical lines to separate rx and tx
    mvwvline(main_window, 2, width_per_column - 2, ACS_VLINE, tx_vars.size());
    mvwvline(main_window, 2, (2 * width_per_column) - 2, ACS_VLINE, tx_vars.size());

    wrefresh(main_window);
}

void TUI::start()
{
    ecat_data_source.reset();
    bool error = false;
    if (ecat_src == "ecat")
    {
        ecat_data_source = std::make_shared<EthercatMaster>(network_interface, zmq_pub);
    }
    else if (ecat_src == "sniffer")
    {
        std::string error_msg;
        ecat_data_source = std::make_shared<PacketSniffer>(network_interface, false, zmq_pub, error_msg);
        if (!error_msg.empty())
        {
            writeStatus(error_msg);
            error = true;
        }
        else if (!config_file_name.empty())
        {
            std::static_pointer_cast<PacketSniffer>(ecat_data_source)->setConfigFile(config_file_name, error_msg);
            if (!error_msg.empty())
            {
                writeStatus(error_msg);
                ecat_data_source.reset();
                error = true;
            }
        }
        else
        {
            writeStatus("Config file not specified");
            ecat_data_source.reset();
            error = true;
        }
    }
    else if (ecat_src == "pcap")
    {
        if (config_file_name.empty())
        {
            writeStatus("Config file not specified");
            error = true;
        }
        else if (pcap_file_name.empty())
        {
            writeStatus("PCAP file not specified");
            error = true;
        }
        else
        {
            std::string error_msg;
            ecat_data_source = std::make_shared<PacketSniffer>(pcap_file_name, true, zmq_pub, error_msg);
            if (!error_msg.empty())
            {
                writeStatus(error_msg);
                ecat_data_source.reset();
                error = true;
            }
            else
            {
                std::static_pointer_cast<PacketSniffer>(ecat_data_source)->setConfigFile(config_file_name, error_msg);
                if (!error_msg.empty())
                {
                    writeStatus(error_msg);
                    ecat_data_source.reset();
                    error = true;
                }
            }
        }
    }
    if (!error)
    {
        ecat_data_source->setZMQPublish(enable_zmq);
        ecat_data_source->setDataCallback(&UI::dataCallback, this);

        std::string error_msg;
        std::vector<std::shared_ptr<EthercatSlave>> slaves = ecat_data_source->getSlaves(error_msg);
        if (!error_msg.empty())
        {
            writeStatus(error_msg);
            ecat_data_source.reset();
            error = true;
        }
        else
        {
            std::string msg = "Found " + std::to_string(slaves.size()) + " slaves";
            writeStatus(msg);
            ecat_data_source->setDataCallback(&UI::dataCallback, this);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            ecat_data_source->start(error_msg);
            if (!error_msg.empty())
            {
                writeStatus(error_msg);
                ecat_data_source.reset();
                error = true;
            }
            else
            {
                // wait here until finish
                while (true)
                {
                    int input = getch();
                    if (input == 'q')
                    {
                        break;
                    }
                    if (input == KEY_LEFT)
                    {
                        selected_slave -= 1;
                    }
                    else if (input == KEY_RIGHT)
                    {
                        selected_slave += 1;
                    }
                }
                ecat_data_source->stop();
            }
        }
    }
    if (error)
    {
        while (true)
        {
            int input = getch();
            if (input == 'q')
            {
                break;
            }
        }
    }
}

void TUI::writeStatus(const std::string &msg)
{
    mvwprintw(status_window, 0, 0, msg.c_str());
    wrefresh(status_window);
}
