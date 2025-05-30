#include <ctime>
#include <iostream>
#include "asio/detail/chrono.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/steady_timer.hpp"
#include "asio/write.hpp"

std::string getDaytime() {
    std::time_t now = std::time(0);
    return std::ctime(&now);
}

int main() {
    try {
        asio::io_context io_context;

        asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 13));

        asio::ip::tcp::socket socket(io_context);
        acceptor.accept(socket);

        asio::steady_timer timer{io_context, asio::chrono::seconds(1)};
        while (1) {
            std::string message = getDaytime();
            std::error_code ignored_error;
            asio::write(socket, asio::buffer(message), ignored_error);
            timer.wait();
            timer.expires_at(timer.expiry() + asio::chrono::seconds(1));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}