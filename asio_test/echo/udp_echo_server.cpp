#include <iostream>
#include "asio/io_context.hpp"
#include "asio/ip/udp.hpp"

const int max_length = 1024;

void server(asio::io_context& io_context, unsigned short port) {
    asio::ip::udp::socket sock(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
    for (;;) {
        char data[max_length];
        asio::ip::udp::endpoint sender_endpoint;
        size_t length = sock.receive_from(asio::buffer(data, max_length), sender_endpoint);
        sock.send_to(asio::buffer(data, length), sender_endpoint);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: blocking_udp_echo_server <port>" << std::endl;
        return 1;
    }

    try {
        asio::io_context io_context;
        server(io_context, std::atoi(argv[1]));
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}