#include "bot.h"
#include <optional>
#include <random>
#include <cstdlib>
#include <string>

TelegramBot::TelegramBot(unsigned seed) : gen_(seed) {
}

unsigned TelegramBot::GetRandom() {
    std::uniform_int_distribution<unsigned> dist;
    return dist(gen_);
}

std::string TelegramBot::GetWeather() const {
    return std::string(TelegramBot::kWeatherPrediction);
}

std::string TelegramBot::GetJoke() const {
    return std::string(TelegramBot::kCodeReviewJoke);
}

void TelegramBot::Stop() const {
    std::exit(0);
}

void TelegramBot::Crash() const {
    std::abort();
}

std::optional<std::string> TelegramBot::ExecuteCommand(const std::string& command) {
    if (command == "/random") {
        return std::to_string(GetRandom());
    } else if (command == "/weather") {
        return GetWeather();
    } else if (command == "/styleguide") {
        return GetJoke();
    } else if (command == "/stop") {
        Stop();
        return std::nullopt;
    } else if (command == "/crash") {
        Crash();
        return std::nullopt;
    } else {
        return std::nullopt;
    }
}
