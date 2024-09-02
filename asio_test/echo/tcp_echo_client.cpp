#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include "asio/connect.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/read.hpp"
#include "asio/write.hpp"

int main(int argc, const char* argv[])
{
    const int max_length = 1024;
    if (argc != 3)
    {
        std::cerr << "Error: client.exe <host> <port>" << std::endl;
    }

    try
    {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver{io_context};
        asio::ip::tcp::socket socket{io_context};

        asio::connect(socket, resolver.resolve(argv[1], argv[2]));

        std::cout << "Enter Message:" << std::endl;
        char send_message[max_length];

        std::cin.getline(send_message, max_length);
        size_t send_len = std::strlen(send_message);
        asio::write(socket, asio::buffer(send_message, send_len));

        char receive_message[max_length];
        size_t receive_len = send_len;
        asio::read(socket, asio::buffer(receive_message, receive_len));

        std::cout << "Reply is: ";
        std::cout.write(receive_message, receive_len);
        std::cout << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}