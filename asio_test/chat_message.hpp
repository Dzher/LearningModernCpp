#ifndef _CHAT_MESSAGE_H_
#define _CHAT_MESSAGE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
class ChatMessage
{
public:
    explicit ChatMessage() : body_length_(0)
    {
    }

    char* data()
    {
        return data_;
    }

    unsigned int dataLength()
    {
        return kHeaderLength + body_length_;
    }

    char* body()
    {
        return data_ + kHeaderLength;
    }

    unsigned int bodyLength()
    {
        return body_length_;
    }

    void updateBodyLength(unsigned int length)
    {
        body_length_ = length <= kMaxBodyLength ? length : kMaxBodyLength;
    }

    bool decodeHeader()
    {
        char header[kHeaderLength + 1] = "";
        std::strncat(header, data_, kHeaderLength);
        body_length_ = std::atoi(header);
        if (body_length_ > kMaxBodyLength)
        {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encodeHeader()
    {
        char header[kHeaderLength + 1] = "";
        std::sprintf(header, "%4d", body_length_);
        std::memcpy(data_, header, kHeaderLength);
    }

private:
    static constexpr int kHeaderLength = 4;
    static constexpr int kMaxBodyLength = 256;
    char data_[kHeaderLength + kMaxBodyLength];
    unsigned int body_length_;
};

#endif