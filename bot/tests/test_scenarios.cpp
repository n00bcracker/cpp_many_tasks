#include <tests/test_scenarios.h>
#include <catch2/catch_test_macros.hpp>
#include <telegram/api.h>
#include <string>
#include <optional>

void TestSingleGetMe(std::string_view url) {
    auto api = TelegramAPI(std::string(url), "123");
    auto me = api.GetMe();

    REQUIRE(me.id == 1234567);
    REQUIRE(me.is_bot);
    REQUIRE(me.username == "test_bot");
    REQUIRE(me.first_name == "Test Bot");
}

void TestGetMeErrorHandling(std::string_view url) {
    auto api = TelegramAPI(std::string(url), "123");
    const auto& http_error_test = [&](int expected_code) {
        try {
            api.GetMe();
        } catch (const APIError& error) {
            REQUIRE(error.http_code == expected_code);
            throw error;
        } catch (...) {
            throw;
        }
    };
    REQUIRE_THROWS_AS(http_error_test(500), APIError);
    REQUIRE_THROWS_AS(http_error_test(401), APIError);
}

void TestSingleGetUpdatesAndSendMessages(std::string_view url) {
    auto api = TelegramAPI(std::string(url), "123");
    TelegramAPI::GetUpdatesResponse updates;
    REQUIRE_NOTHROW(updates = api.GetUpdates(std::nullopt, std::nullopt, std::nullopt));
    REQUIRE(updates.updates.size() == 4);

    const auto& message1 = updates.updates[0];
    REQUIRE(message1.message_id == 1);
    REQUIRE(message1.date == 1510493105);
    REQUIRE(message1.update_id == 851793506);
    REQUIRE(message1.chat_id == 104519755);
    REQUIRE(message1.text == "/start");

    const auto& message2 = updates.updates[1];
    REQUIRE(message2.message_id == 2);
    REQUIRE(message2.date == 1510493105);
    REQUIRE(message2.update_id == 851793507);
    REQUIRE(message2.chat_id == 104519755);
    REQUIRE(message2.text == "/end");

    const auto& message3 = updates.updates[2];
    REQUIRE(message3.message_id == 10);
    REQUIRE(message3.date == 1510519971);
    REQUIRE(message3.update_id == 851793507);
    REQUIRE(message3.chat_id == -274574250);
    REQUIRE(message3.text.empty());

    const auto& message4 = updates.updates[3];
    REQUIRE(message4.message_id == 11);
    REQUIRE(message4.date == 1510520023);
    REQUIRE(message4.update_id == 851793508);
    REQUIRE(message4.chat_id == -274574250);
    REQUIRE(message4.text == "/1234");

    REQUIRE_NOTHROW(api.SendMessage({message2.chat_id, "Hi!"}));
    REQUIRE_NOTHROW(api.SendMessage({message2.chat_id, "Reply", message2.message_id}));
    REQUIRE_NOTHROW(api.SendMessage({message2.chat_id, "Reply", message2.message_id}));
}

void TestHandleGetUpdatesOffset(std::string_view url) {
    auto api = TelegramAPI(std::string(url), "123");
    TelegramAPI::GetUpdatesResponse updates;
    REQUIRE_NOTHROW(updates = api.GetUpdates(std::nullopt, std::nullopt, 5));
    REQUIRE(updates.updates.size() == 2);

    const auto& message1 = updates.updates[0];
    REQUIRE(message1.message_id == 1);
    REQUIRE(message1.date == 1510493105);
    REQUIRE(message1.update_id == 851793506);
    REQUIRE(message1.chat_id == 104519755);
    REQUIRE(message1.text == "/start");

    const auto& message2 = updates.updates[1];
    REQUIRE(message2.message_id == 2);
    REQUIRE(message2.date == 1510493105);
    REQUIRE(message2.update_id == 851793507);
    REQUIRE(message2.chat_id == 104519755);
    REQUIRE(message2.text == "/end");

    auto next_update_id = std::max(message1.update_id, message2.update_id) + 1;
    REQUIRE_NOTHROW(updates = api.GetUpdates(next_update_id, std::nullopt, 5));
    REQUIRE(updates.updates.empty());

    REQUIRE_NOTHROW(updates = api.GetUpdates(next_update_id, std::nullopt, 5));
    REQUIRE(updates.updates.size() == 1);

    const auto& message3 = updates.updates[0];
    REQUIRE(message3.message_id == 11);
    REQUIRE(message3.date == 1510520023);
    REQUIRE(message3.update_id == 851793508);
    REQUIRE(message3.chat_id == -274574250);
    REQUIRE(message3.text == "/1234");
}
