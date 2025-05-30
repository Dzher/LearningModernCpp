#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include "asio/connect.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/read.hpp"
#include "asio/read_until.hpp"
#include "asio/streambuf.hpp"
#include "asio/write.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: sync_client <server> <path>" << std::endl;
        std::cout << "Example: http_sync_client www.boost.org /LICENSE_1_0.txt" << std::endl;
        return 1;
    }

    try {
        asio::io_context io_context;

        asio::ip::tcp::resolver resolver(io_context);
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "http");

        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET " << argv[2] << " HTTP/1.0\r\n";
        request_stream << "Host: " << argv[1] << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        asio::write(socket, request);

        asio::streambuf response;
        asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            std::cout << "Invalid response\n";
            return 1;
        }
        if (status_code != 200) {
            std::cout << "Response returned with status code " << status_code << "\n";
            return 1;
        }

        asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
            std::cout << header << "\n";
        }
        std::cout << "\n";

        // Write whatever content we already have to output.
        if (response.size() > 0) {
            std::cout << &response;
        }
        // Read until EOF, writing data to output as we go.
        std::error_code error;
        while (asio::read(socket, response, asio::transfer_at_least(1), error)) {
            std::cout << &response;
        }
        if (error != asio::error::eof) {
            throw std::system_error(error);
        }
    }
    catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}