#include <ctime>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <system_error>
#include "asio/io_context.hpp"
#include "asio/ip/udp.hpp"
#include "asio/placeholders.hpp"

std::string getDaytimeStr()
{
    std::time_t now = std::time(nullptr);
    return std::ctime(&now);
}

class UDPServer
{
public:
    UDPServer(asio::io_context& io_context)
        : io_context_(io_context), socket_(io_context_, asio::ip::udp::endpoint(asio::ip::udp::v4(), 13))
    {
        startReceive();
    }

private:
    void startReceive()
    {
        socket_.async_receive_from(asio::buffer(recv_buffer_), endpoint_,
                                   std::bind(&UDPServer::handleReceive, this, asio::placeholders::error,
                                             asio::placeholders::bytes_transferred));
    }

    void handleReceive(const std::error_code& error, std::size_t /*bytes_transferred*/)
    {
        if (!error)
        {
            std::shared_ptr<std::string> message(new std::string(getDaytimeStr()));

            socket_.async_send_to(asio::buffer(*message), endpoint_,
                                  std::bind(&UDPServer::handleSend, this, message, asio::placeholders::error,
                                            asio::placeholders::bytes_transferred));

            startReceive();
        }
    }

    void handleSend(std::shared_ptr<std::string> /*message*/, const std::error_code& /*error*/,
                     std::size_t /*bytes_transferred*/)
    {
    }

private:
    asio::io_context& io_context_;
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint endpoint_;
    std::array<char, 1> recv_buffer_;
};

int main()
{
    try
    {
        asio::io_context io_context;
        UDPServer server(io_context);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}