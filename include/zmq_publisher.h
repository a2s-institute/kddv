/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#ifndef ZMQ_PUBLISHER_H_
#define ZMQ_PUBLISHER_H_

#include "zmq.hpp"

class ZMQPublisher
{

    public:
        ZMQPublisher(const std::string &port);
        virtual ~ZMQPublisher();
        void publishMsg(const std::string &json_string);
    private:
        zmq::context_t ctx;
        zmq::socket_t publisher;

};
#endif
