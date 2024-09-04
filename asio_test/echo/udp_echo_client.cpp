#include <cstddef>
#include <exception>
#include <iostream>
#include "asio/io_context.hpp"
#include "asio/ip/udp.hpp"

const int max_length = 1024;

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Error: client <host> <port>" << std::endl;
        return 1;
    }

    try
    {
        asio::io_context io_context;
        asio::ip::udp::resolver resolver{io_context};
        auto endpoint = resolver.resolve(asio::ip::udp::v4(), argv[1], argv[2])->endpoint();

        std::cout << "Enter Message: " << std::endl;
        char send_message[max_length];
        std::cin.getline(send_message, max_length);
        size_t send_len = std::strlen(send_message);
        asio::ip::udp::socket socket{io_context};
        socket.open(asio::ip::udp::v4());
        socket.send_to(asio::buffer(send_message, send_len), endpoint);

        char reveive_message[max_length];
        asio::ip::udp::endpoint sender_endpoint;
        size_t receive_length = socket.receive_from(asio::buffer(reveive_message, max_length), sender_endpoint);
        std::cout << "Reply is: ";
        std::cout.write(reveive_message, receive_length);
        std::cout << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}