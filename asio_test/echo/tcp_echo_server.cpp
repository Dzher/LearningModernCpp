#include <exception>
#include <iostream>
#include <system_error>
#include <thread>
#include "asio/error.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/address_v4.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/write.hpp"

const int max_length = 1024;

void session(asio::ip::tcp::socket socket) {

    try {
        while (true) {
            char message[max_length];
            std::error_code error;

            size_t len = socket.read_some(asio::buffer(message), error);
            if (error == asio::error::eof) {
                break;
            }
            if (error) {
                throw std::system_error(error);
            }

            asio::write(socket, asio::buffer(message, len));
            // ? think why not use: socket.write_some(asio::buffer(message));
        }
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void server(asio::io_context& io_context, unsigned short port) {
    asio::ip::tcp::acceptor acceptor{io_context, asio::ip::tcp::endpoint{asio::ip::tcp::v4(), port}};
    while (true) {
        std::thread{session, acceptor.accept()}.detach();
    }
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
        return 1;
    }

    try {
        asio::io_context io_context;
        server(io_context, std::atoi(argv[1]));
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}