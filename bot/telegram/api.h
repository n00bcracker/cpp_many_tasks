#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <exception>

class TelegramAPI {
public:
    struct GetMeResponse {
        GetMeResponse(int64_t user_id, bool r_is_bot, std::string r_username,
                      std::string r_first_name);

        const int64_t id;
        const bool is_bot;
        const std::string username, first_name;
    };

    struct Update {
        Update(uint64_t upd_id, uint64_t msg_id, int64_t chat, uint64_t msg_date,
               std::string msg_text);
        Update(const Update& other) = default;
        Update(Update&& other);

        const uint64_t update_id;
        const uint64_t message_id;
        const int64_t chat_id;
        const uint64_t date;
        const std::string text;
    };

    struct GetUpdatesResponse {
        GetUpdatesResponse() = default;
        GetUpdatesResponse(GetUpdatesResponse&& other);
        const GetUpdatesResponse& operator=(GetUpdatesResponse&& other);

        std::vector<Update> updates;
    };

    struct SendMessageRequest {
        SendMessageRequest(int64_t chat, std::string msg_text,
                           std::optional<uint64_t> reply_to = std::nullopt);

        const int64_t chat_id;
        const std::string text;
        const std::optional<uint64_t> reply_to_message_id;
    };

    TelegramAPI(std::string api_url, std::string api_token);
    GetMeResponse GetMe() const;
    GetUpdatesResponse GetUpdates(std::optional<uint64_t> offset = std::nullopt,
                                  std::optional<uint64_t> limit = 10,
                                  std::optional<uint64_t> timeout = 300) const;
    void SendMessage(SendMessageRequest message) const;

private:
    const std::string api_url_;
    const std::string api_token_;
};

struct APIError : std::runtime_error {
    APIError(int http_code, std::string details)
        : std::runtime_error{"api error: code=" + std::to_string(http_code) +
                             " details=" + details},
          http_code{http_code},
          details{std::move(details)} {
    }

    int http_code;
    std::string details;
};