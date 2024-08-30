#include <memory>
#include <set>
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
