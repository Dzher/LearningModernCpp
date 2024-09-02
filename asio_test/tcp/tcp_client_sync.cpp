#include <iostream>
#include "asio/connect.hpp"
#include "asio/ip/tcp.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Error: Host issue" << std::endl;
        return 1;
    }

    try
    {
        asio::io_context io_context;

        asio::ip::tcp::resolver resolver(io_context);
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "daytime");

        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        while (1)
        {
            std::array<char, 128> buf;
            std::error_code error;

            size_t len = socket.read_some(asio::buffer(buf), error);

            if (error == asio::error::eof)
            {
                break;  // Connection closed cleanly by peer.
            }
            else if (error)
            {
                throw std::system_error(error);  // Some other error.
            }
            std::cout.write(buf.data(), len);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}