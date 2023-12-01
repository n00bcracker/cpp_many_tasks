#include "client.h"
#include "bot.h"
#include <memory>

int main(int argc, char* argv[]) {
    std::unique_ptr<MessengerClient> messenger_ptr(new TelegramClient(
        "https://api.telegram.org", "6739870667:AAEV7axYX_6uKef6Nz9gcwKIIkKNclJdyCg"));
    TelegramBot bot(42);
    while (true) {
        auto command = messenger_ptr->ReadMessage();
        auto result = bot.ExecuteCommand(command->text);
        if (result.has_value()) {
            auto response = command->Reply(*result);
            messenger_ptr->SendMessage(std::move(response));
        }
    }
}
