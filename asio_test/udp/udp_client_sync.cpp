#include <array>
#include <exception>
#include <iostream>
#include "asio/io_context.hpp"
#include "asio/ip/udp.hpp"

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Error: Host Issue!" << std::endl;
        return 1;
    }

    try {
        asio::io_context context;

        asio::ip::udp::resolver resolver{context};
        asio::ip::udp::endpoint receiver_endpoint =
            resolver.resolve(asio::ip::udp::v4(), argv[1], "daytime").begin()->endpoint();

        std::array<char, 1> send_buf = {{0}};
        asio::ip::udp::socket socket{context};
        socket.open(asio::ip::udp::v4());
        socket.send_to(asio::buffer(send_buf), receiver_endpoint);

        std::array<char, 128> recv_buf;
        asio::ip::udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(asio::buffer(recv_buf), sender_endpoint);

        std::cout.write(recv_buf.data(), len);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}