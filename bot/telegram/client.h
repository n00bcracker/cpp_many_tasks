#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <queue>
#include "api.h"

struct Message {
    Message(uint64_t timestamp, int64_t chat, std::string message_text);
    virtual std::unique_ptr<Message> Reply(std::string reply,
                                           bool mention_prev_message = false) const;

    const uint64_t timestamp_s;
    const int64_t chat_id;
    const std::string text;
};

struct TelegramMessage : public Message {
    TelegramMessage(uint64_t timestamp, int64_t chat, std::string message_text, uint64_t upd_id,
                    uint64_t msg_id, std::optional<uint64_t> reply_to = std::nullopt);
    std::unique_ptr<Message> Reply(std::string reply,
                                   bool mention_prev_message = false) const override;

    const uint64_t update_id;
    const uint64_t message_id;
    const std::optional<uint64_t> reply_to_message_id;
};

enum class MessengerTypes {
    Telegram,
};

class MessengerClient {
public:
    explicit MessengerClient(MessengerTypes type);
    virtual std::unique_ptr<Message> ReadMessage() = 0;
    virtual void SendMessage(std::unique_ptr<Message> message) = 0;
    virtual ~MessengerClient() = default;

private:
    const MessengerTypes type_;

protected:
    std::queue<std::unique_ptr<Message>> messages_queue_;
};

class TelegramClient : public MessengerClient {
public:
    TelegramClient(std::string api_url, std::string api_token);
    std::unique_ptr<Message> ReadMessage() override;
    void SendMessage(std::unique_ptr<Message> message) override;

private:
    uint64_t cur_update_id_;
    const TelegramAPI api_;
    const std::string name_;
    static constexpr std::string_view kOffsetFilePath = "./offset.tmp";
};
