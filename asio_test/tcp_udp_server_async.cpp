#include <array>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <system_error>
#include "asio/buffer.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ip/udp.hpp"
#include "asio/write.hpp"

std::string daytime() {
    std::time_t now = std::time(nullptr);
    return std::ctime(&now);
}

class Session : public std::enable_shared_from_this<Session>
{
public:
    static std::shared_ptr<Session> create(asio::io_context& context) {
        // return std::make_shared<Session>(std::move(context));
        return std::shared_ptr<Session>(new Session(context));
    }

    asio::ip::tcp::socket& socket() {
        return socket_;
    }

    void start() {
        message_ = daytime();
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(message_), [this, self](std::error_code, std::size_t) {});
    }

private:
    Session(asio::io_context& context) : socket_(context) {
    }

private:
    asio::ip::tcp::socket socket_;
    std::string message_;
};

class TCPServer
{
public:
    TCPServer(asio::io_context& io_context)
        : io_context_(io_context), acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 13)) {
    }

private:
    void startAccept() {
        auto new_session = Session::create(io_context_);
        acceptor_.async_accept(new_session->socket(),
                               [&new_session, this](const std::error_code& error)
                               {
                                   if (!error) {
                                       new_session->start();
                                   }
                                   startAccept();
                               });
    }

private:
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
};

class UDPServer
{
public:
    UDPServer(asio::io_context& io_context)
        : socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 13)), endpoint_(asio::ip::udp::v4(), 13) {
        startReceive();
    }

private:
    void startReceive() {
        socket_.async_receive_from(asio::buffer(recv_buffer_), endpoint_,
                                   [this](const std::error_code& error, std::size_t)
                                   {
                                       if (error) {
                                           return;
                                       }
                                       std::shared_ptr<std::string> message(new std::string(daytime()));

                                       socket_.async_send_to(asio::buffer(*message), endpoint_,
                                                             [](const std::error_code& error, std::size_t) {});

                                       startReceive();
                                   });
    }

private:
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint endpoint_;
    std::array<char, 1> recv_buffer_;
};

int main() {
    try {
        asio::io_context io_context;
        TCPServer server1(io_context);
        UDPServer server2(io_context);
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}