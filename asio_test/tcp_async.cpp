#include <ctime>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <system_error>
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/placeholders.hpp"
#include "asio/write.hpp"

std::string getDaytimeStr()
{
    std::time_t now = std::time(nullptr);
    return std::ctime(&now);
}

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
public:
    static std::shared_ptr<TCPConnection> create(asio::io_context& io_context)
    {
        return std::shared_ptr<TCPConnection>(new TCPConnection(io_context));
    }

    asio::ip::tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        message_ = getDaytimeStr();
        asio::async_write(socket_, asio::buffer(message_),
                          std::bind(&TCPConnection::handleWrite, shared_from_this(), asio::placeholders::error,
                                    asio::placeholders::bytes_transferred));
    }

private:
    TCPConnection(asio::io_context& io_context) : socket_(io_context)
    {
    }

    void handleWrite(const std::error_code&, size_t)
    {
    }

private:
    asio::ip::tcp::socket socket_;
    std::string message_;
};

class TCPServer
{
public:
    TCPServer(asio::io_context& io_context)
        : io_context_(io_context), acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 13))
    {
        startAccept();
    }

private:
    void startAccept()
    {
        auto new_connection = TCPConnection::create(io_context_);
        acceptor_.async_accept(new_connection->socket(),
                               std::bind(&TCPServer::handleAccept, this, new_connection, asio::placeholders::error));
    }

    void handleAccept(std::shared_ptr<TCPConnection> new_connection, const std::error_code& error)
    {
        if (!error)
        {
            new_connection->start();
        }
        startAccept();
    }

private:
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        asio::io_context io_context;
        TCPServer server(io_context);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}