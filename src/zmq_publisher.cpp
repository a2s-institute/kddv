/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include "zmq_publisher.h"
#include <iostream>


ZMQPublisher::ZMQPublisher(const std::string &port) : publisher(ctx, ZMQ_PUB)
{
    std::string bind_str = "tcp://*:" + port;
    publisher.bind(bind_str);
}
ZMQPublisher::~ZMQPublisher()
{
    publisher.close();
    ctx.shutdown();
    ctx.close();
}

void ZMQPublisher::publishMsg(const std::string &json_string)
{
    zmq::message_t message(json_string.length());
    std::memcpy(message.data(), json_string.c_str(), json_string.length());
    publisher.send(message, zmq::send_flags::dontwait);
}
