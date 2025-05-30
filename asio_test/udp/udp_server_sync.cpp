#include <array>
#include <ctime>
#include <exception>
#include <iostream>
#include <string>
#include <system_error>
#include "asio/buffer.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/udp.hpp"

std::string getDaytime() {
    std::time_t now = std::time(0);
    return std::ctime(&now);
}

int main() {
    try {
        asio::io_context io_context;
        asio::ip::udp::socket socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 13));

        while (1) {
            std::array<char, 1> recv_buf;
            asio::ip::udp::endpoint remote_endpoint;
            socket.receive_from(asio::buffer(recv_buf), remote_endpoint);

            std::string message = getDaytime();

            std::error_code ignored_error;
            socket.send_to(asio::buffer(message), remote_endpoint, 0, ignored_error);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}