#include <iostream>
#include <memory>
#include <set>
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/read.hpp"
#include "asio/write.hpp"
#include "chat_message.hpp"

class ChatParticipant
{
public:
    virtual ~ChatParticipant()
    {
    }

    virtual void deliver(const ChatMessage& msg) = 0;
};

using ChatParticipantPtr = std::shared_ptr<ChatParticipant>;

class ChatRoom
{
public:
    void join(ChatParticipantPtr participant)
    {
        participants_.insert(participant);
        for (auto msg : recent_msgs_)
        {
            participant->deliver(msg);
        }
    }

    void leave(ChatParticipantPtr participant)
    {
        participants_.erase(participant);
    }

    void deliver(const ChatMessage& msg)
    {
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > kMaxRecentMsgs)
        {
            recent_msgs_.pop_front();
        }

        for (auto participant : participants_)
        {
            participant->deliver(msg);
        }
    }

private:
    std::set<ChatParticipantPtr> participants_;
    ChatMsgQueue recent_msgs_;
    const int kMaxRecentMsgs = 100;
};

class ChatSession : public ChatParticipant, public std::enable_shared_from_this<ChatSession>
{
public:
    ChatSession(asio::ip::tcp::socket socket, ChatRoom& room) : socket_(std::move(socket)), room_(room)
    {
    }

    void start()
    {
        room_.join(shared_from_this());
        readHeader();
    }

    void deliver(const ChatMessage& msg)
    {
        bool writing = !write_messages_.empty();
        write_messages_.push_back(msg);
        if (!writing)
        {
            write();
        }
    }

    void readHeader()
    {
        auto self(shared_from_this());
        asio::async_read(socket_, asio::buffer(read_message_.data(), ChatMessage::headerLength()),
                         [this, self](std::error_code ec, std::size_t /*length*/)
                         {
                             if (!ec && read_message_.decodeHeader())
                             {
                                 readBody();
                             }
                             else
                             {
                                 room_.leave(shared_from_this());
                             }
                         });
    }

    void readBody()
    {
        auto self(shared_from_this());
        asio::async_read(socket_, asio::buffer(read_message_.body(), read_message_.bodyLength()),
                         [this, self](std::error_code ec, std::size_t /*length*/)
                         {
                             if (!ec)
                             {
                                 room_.deliver(read_message_);
                                 readHeader();
                             }
                             else
                             {
                                 room_.leave(shared_from_this());
                             }
                         });
    }

    void write()
    {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(write_messages_.front().data(), write_messages_.front().dataLength()),
                          [this, self](std::error_code ec, std::size_t /*length*/)
                          {
                              if (!ec)
                              {
                                  write_messages_.pop_front();
                                  if (!write_messages_.empty())
                                  {
                                      write();
                                  }
                              }
                              else
                              {
                                  room_.leave(shared_from_this());
                              }
                          });
    }

private:
    asio::ip::tcp::socket socket_;
    ChatRoom& room_;
    ChatMessage read_message_;
    ChatMsgQueue write_messages_;
};

class ChatServer
{
public:
    ChatServer(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint) : acceptor_(io_context, endpoint)
    {
        accept();
    }

private:
    void accept()
    {
        acceptor_.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<ChatSession>(std::move(socket), room_)->start();
                }

                accept();
            });
    }

    asio::ip::tcp::acceptor acceptor_;
    ChatRoom room_;
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: chat_server <port> [<port> ...]\n";
        return 1;
    }

    try
    {
        asio::io_context io_context;

        std::list<ChatServer> servers;
        for (int port_index = 1; port_index < argc; ++port_index)
        {
            asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), std::atoi(argv[port_index]));
            servers.emplace_back(io_context, endpoint);
        }

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}