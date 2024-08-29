

#include <winsock2.h>
#include <cstring>
#include <deque>
#include <exception>
#include <iostream>
#include <system_error>
#include <thread>
#include "asio/connect.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/post.hpp"
#include "asio/read.hpp"
#include "asio/write.hpp"
#include "chat_message.hpp"

class ChatClient
{
    using ChatMsgQueue = std::deque<ChatMessage>;

public:
    ChatClient(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
        : io_context_(io_context), socket_(io_context_)
    {
        connect(endpoints);
    }

    void write(const ChatMessage& msgs)
    {
        asio::post(io_context_,
                   [this, msgs]()
                   {
                       bool write_in_progress = !write_messages_.empty();
                       write_messages_.push_back(msgs);
                       if (!write_in_progress)
                       {
                           write();
                       }
                   });
    }

    void close()
    {
        asio::post(io_context_, [this]() { socket_.close(); });
    }

private:
    void connect(const asio::ip::tcp::resolver::results_type& endpoints)
    {
        asio::async_connect(socket_, endpoints,
                            [this](std::error_code error, asio::ip::tcp::endpoint)
                            {
                                if (!error)
                                {
                                    readHeader();
                                }
                            });
    }

    void readHeader()
    {
        asio::async_read(socket_, asio::buffer(read_message_.data(), read_message_.headerLength()),
                         [this](std::error_code error, std::size_t)
                         {
                             if (!error && read_message_.decodeHeader())
                             {
                                 readBody();
                             }
                             else
                             {
                                 socket_.close();
                             }
                         });
    }

    void readBody()
    {
        asio::async_read(socket_, asio::buffer(read_message_.body(), read_message_.bodyLength()),
                         [this](std::error_code error, std::size_t)
                         {
                             if (!error)
                             {
                                 std::cout.write(read_message_.body(), read_message_.bodyLength()) << std::endl;
                                 readHeader();
                             }
                             else
                             {
                                 socket_.close();
                             }
                         });
    }

    void write()
    {
        asio::async_write(socket_, asio::buffer(write_messages_.front().data(), write_messages_.front().dataLength()),
                          [this](std::error_code error, std::size_t)
                          {
                              if (!error)
                              {
                                  write_messages_.pop_front();
                                  if (!write_messages_.empty())
                                  {
                                      write();
                                  }
                              }
                              else
                              {
                                  socket_.close();
                              }
                          });
    }

private:
    asio::io_context& io_context_;
    asio::ip::tcp::socket socket_;
    ChatMessage read_message_;
    ChatMsgQueue write_messages_;
};

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: chat_client <host> <port>" << std::endl;
        return 1;
    }

    try
    {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        asio::ip::basic_resolver_results endpoints = resolver.resolve(argv[1], argv[2]);
        ChatClient client{io_context, endpoints};

        std::thread thread([&io_context]() { io_context.run(); });
        char line[ChatMessage::bodyMaxLength() + 1];
        while (std::cin.getline(line, ChatMessage::bodyMaxLength() + 1))
        {
            ChatMessage msg;
            msg.updateBodyLength(std::strlen(line));
            std::memcpy(msg.body(), line, msg.bodyLength());
            msg.encodeHeader();
            client.write(msg);
        }

        client.close();
        thread.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}