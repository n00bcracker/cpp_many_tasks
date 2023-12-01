#include "client.h"
#include "api.h"
#include <fstream>

Message::Message(uint64_t timestamp, int64_t chat, std::string message_text)
    : timestamp_s(timestamp), chat_id(chat), text(std::move(message_text)) {
}

std::unique_ptr<Message> Message::Reply(std::string reply, bool mention_prev_message) const {
    return std::unique_ptr<Message>(new Message(0, chat_id, std::move(reply)));
}

TelegramMessage::TelegramMessage(uint64_t timestamp, int64_t chat, std::string message_text,
                                 uint64_t upd_id, uint64_t msg_id, std::optional<uint64_t> reply_to)
    : Message(timestamp, chat, std::move(message_text)),
      update_id(upd_id),
      message_id(msg_id),
      reply_to_message_id(reply_to) {
}

std::unique_ptr<Message> TelegramMessage::Reply(std::string reply,
                                                bool mention_prev_message) const {
    if (mention_prev_message) {
        return std::unique_ptr<Message>(
            new TelegramMessage(0, chat_id, std::move(reply), update_id, 0, message_id));
    } else {
        return std::unique_ptr<Message>(
            new TelegramMessage(0, chat_id, std::move(reply), update_id, 0));
    }
}

MessengerClient::MessengerClient(MessengerTypes type) : type_(type) {
}

TelegramClient::TelegramClient(std::string api_url, std::string api_token)
    : MessengerClient(MessengerTypes::Telegram),
      cur_update_id_(0),
      api_(api_url, api_token),
      name_(api_.GetMe().first_name) {
    std::ifstream offset_file(kOffsetFilePath);
    if (offset_file.is_open()) {
        offset_file >> cur_update_id_;
        offset_file.close();
    }
}

std::unique_ptr<Message> TelegramClient::ReadMessage() {
    if (messages_queue_.empty()) {
        TelegramAPI::GetUpdatesResponse update;
        while (update.updates.empty()) {
            update = api_.GetUpdates(cur_update_id_ + 1);
        }

        while (!update.updates.empty()) {
            auto& message = update.updates.back();
            messages_queue_.emplace(new TelegramMessage{message.date, message.chat_id,
                                                        std::move(message.text), message.update_id,
                                                        message.message_id});
            update.updates.pop_back();
        }
    }

    auto message = std::move(messages_queue_.front());
    cur_update_id_ = dynamic_cast<TelegramMessage*>(message.get())->update_id;
    std::ofstream offset_file(kOffsetFilePath);
    if (offset_file.is_open()) {
        offset_file << cur_update_id_;
        offset_file.flush();
        offset_file.close();
    }

    messages_queue_.pop();
    return message;
}

void TelegramClient::SendMessage(std::unique_ptr<Message> message) {
    TelegramMessage& msg = dynamic_cast<TelegramMessage&>(*message);
    api_.SendMessage({msg.chat_id, std::move(msg.text), msg.reply_to_message_id});
}