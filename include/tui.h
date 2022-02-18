/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#ifndef TUI_H_
#define TUI_H_

#include "ui.h"
#include <ncurses.h>

class TUI : public UI
{
    public:
        TUI(std::shared_ptr<ZMQPublisher> zmq_pub);
        virtual ~TUI();
        void selectSource(const std::string &src);
        void selectNetworkInterface(const std::string &iface);
        void setConfigFile(const std::string &path);
        void setPCAPFile(const std::string &path);
        void enableZMQ(bool enable);
        void start();
        void dataCallback(const std::vector<std::shared_ptr<EthercatSlave>> &slaves);
    private:
        std::string network_interface;
        std::string ecat_src;
        bool enable_zmq;

        WINDOW *main_window;
        WINDOW *status_window;
        WINDOW *instructions_window;

        int selected_slave;

        void setupWindow();
        void writeStatus(const std::string &msg);
};

#endif
