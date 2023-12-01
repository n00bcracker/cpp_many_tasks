#include "api.h"
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/URI.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <cstdint>
#include <algorithm>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <memory>

using Poco::URI;
using Poco::JSON::Array;
using Poco::JSON::Object;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPSClientSession;

TelegramAPI::GetMeResponse::GetMeResponse(int64_t user_id, bool r_is_bot, std::string r_username,
                                          std::string r_first_name)
    : id(user_id),
      is_bot(r_is_bot),
      username(std::move(r_username)),
      first_name(std::move(r_first_name)) {
}

TelegramAPI::Update::Update(uint64_t upd_id, uint64_t msg_id, int64_t chat, uint64_t msg_date,
                            std::string msg_text)
    : update_id(upd_id),
      message_id(msg_id),
      chat_id(chat),
      date(msg_date),
      text(std::move(msg_text)) {
}

TelegramAPI::Update::Update(TelegramAPI::Update&& other)
    : update_id(other.update_id),
      message_id(other.update_id),
      chat_id(other.chat_id),
      date(other.date),
      text(std::move(other.text)) {
}

TelegramAPI::GetUpdatesResponse::GetUpdatesResponse(TelegramAPI::GetUpdatesResponse&& other)
    : updates(std::move(other.updates)) {
}

const TelegramAPI::GetUpdatesResponse& TelegramAPI::GetUpdatesResponse::operator=(
    TelegramAPI::GetUpdatesResponse&& other) {
    updates = std::move(other.updates);
    return *this;
}

TelegramAPI::SendMessageRequest::SendMessageRequest(int64_t chat, std::string msg_text,
                                                    std::optional<uint64_t> reply_to)
    : chat_id(chat), text(std::move(msg_text)), reply_to_message_id(reply_to) {
}

TelegramAPI::TelegramAPI(std::string api_url, std::string api_token)
    : api_url_(api_url), api_token_(api_token) {
}

TelegramAPI::GetMeResponse TelegramAPI::GetMe() const {
    URI request_uri(api_url_);
    request_uri.setPath("/bot" + api_token_ + "/getMe");

    std::unique_ptr<HTTPClientSession> session_ptr;
    if (request_uri.getScheme() == "https") {
        session_ptr = std::unique_ptr<HTTPClientSession>(
            new HTTPSClientSession(request_uri.getHost(), request_uri.getPort()));
    } else if (request_uri.getScheme() == "http") {
        session_ptr = std::unique_ptr<HTTPClientSession>(
            new HTTPClientSession(request_uri.getHost(), request_uri.getPort()));
    }

    auto request = HTTPRequest(HTTPRequest::HTTP_GET, request_uri.getPathAndQuery(),
                               Poco::Net::HTTPMessage::HTTP_1_1);
    session_ptr->sendRequest(request);

    auto response = HTTPResponse();
    auto& rs = session_ptr->receiveResponse(response);
    if (const int code = response.getStatus(); code != HTTPResponse::HTTP_OK) {
        throw APIError(code, response.getReason());
    } else {
        Poco::JSON::Parser parser;
        auto result = parser.parse(rs);
        auto result_object = result.extract<Object::Ptr>()->get("result");

        int64_t user_id;
        bool is_bot;
        std::string username, first_name;
        result_object.extract<Object::Ptr>()->get("id").convert(user_id);
        result_object.extract<Object::Ptr>()->get("is_bot").convert(is_bot);
        result_object.extract<Object::Ptr>()->get("username").convert(username);
        result_object.extract<Object::Ptr>()->get("first_name").convert(first_name);
        return TelegramAPI::GetMeResponse(user_id, is_bot, std::move(username),
                                          std::move(first_name));
    }
}

TelegramAPI::GetUpdatesResponse TelegramAPI::GetUpdates(std::optional<uint64_t> offset,
                                                        std::optional<uint64_t> limit,
                                                        std::optional<uint64_t> timeout) const {
    URI request_uri(api_url_);
    request_uri.setPath("/bot" + api_token_ + "/getUpdates");
    if (offset.has_value()) {
        request_uri.addQueryParameter("offset", std::to_string(*offset));
    }

    if (limit.has_value()) {
        request_uri.addQueryParameter("limit", std::to_string(*limit));
    }

    if (timeout.has_value()) {
        request_uri.addQueryParameter("timeout", std::to_string(*timeout));
    }

    std::unique_ptr<HTTPClientSession> session_ptr;
    if (request_uri.getScheme() == "https") {
        session_ptr = std::unique_ptr<HTTPClientSession>(
            new HTTPSClientSession(request_uri.getHost(), request_uri.getPort()));
    } else if (request_uri.getScheme() == "http") {
        session_ptr = std::unique_ptr<HTTPClientSession>(
            new HTTPClientSession(request_uri.getHost(), request_uri.getPort()));
    }

    auto request = HTTPRequest(HTTPRequest::HTTP_GET, request_uri.getPathAndQuery(),
                               Poco::Net::HTTPMessage::HTTP_1_1);

    TelegramAPI::GetUpdatesResponse update_response;
    session_ptr->sendRequest(request);

    auto response = HTTPResponse();
    auto& rs = session_ptr->receiveResponse(response);
    if (const int code = response.getStatus(); code != HTTPResponse::HTTP_OK) {
        throw APIError(code, response.getReason());
    } else {
        Poco::JSON::Parser parser;
        auto result = parser.parse(rs);
        auto result_object = result.extract<Object::Ptr>()->get("result");
        for (const auto& update_object : *result_object.extract<Array::Ptr>()) {
            uint64_t update_id, message_id, date;
            int64_t chat_id;
            std::string text;
            update_object.extract<Object::Ptr>()->get("update_id").convert(update_id);

            auto message_object = update_object.extract<Object::Ptr>()->get("message");
            message_object.extract<Object::Ptr>()->get("date").convert(date);
            message_object.extract<Object::Ptr>()->get("message_id").convert(message_id);
            if (message_object.extract<Object::Ptr>()->has("text")) {
                message_object.extract<Object::Ptr>()->get("text").convert(text);
            }

            auto chat_object = message_object.extract<Object::Ptr>()->get("chat");
            chat_object.extract<Object::Ptr>()->get("id").convert(chat_id);
            update_response.updates.emplace_back(update_id, message_id, chat_id, date,
                                                 std::move(text));
        }
    }

    return update_response;
}

void TelegramAPI::SendMessage(SendMessageRequest message) const {
    URI request_uri(api_url_);
    request_uri.setPath("/bot" + api_token_ + "/sendMessage");

    std::unique_ptr<HTTPClientSession> session_ptr;
    if (request_uri.getScheme() == "https") {
        session_ptr = std::unique_ptr<HTTPClientSession>(
            new HTTPSClientSession(request_uri.getHost(), request_uri.getPort()));
    } else if (request_uri.getScheme() == "http") {
        session_ptr = std::unique_ptr<HTTPClientSession>(
            new HTTPClientSession(request_uri.getHost(), request_uri.getPort()));
    }

    auto request = HTTPRequest(HTTPRequest::HTTP_POST, request_uri.getPathAndQuery(),
                               Poco::Net::HTTPMessage::HTTP_1_1);
    request.setContentType("application/json");

    Object json;
    json.set("chat_id", message.chat_id);
    json.set("text", std::move(message.text));
    if (message.reply_to_message_id.has_value()) {
        json.set("reply_to_message_id", *message.reply_to_message_id);
    }

    std::ostringstream body;
    json.stringify(body);
    request.setContentLength(body.str().size());

    session_ptr->sendRequest(request) << body.str();
    auto response = HTTPResponse();
    auto& rs = session_ptr->receiveResponse(response);
    if (const int code = response.getStatus(); code != HTTPResponse::HTTP_OK) {
        throw APIError(code, response.getReason());
    }
}