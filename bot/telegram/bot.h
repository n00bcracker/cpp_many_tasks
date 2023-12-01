#pragma once
#include <string>
#include <string_view>
#include <random>
#include <optional>

class TelegramBot {
public:
    explicit TelegramBot(unsigned seed);
    std::optional<std::string> ExecuteCommand(const std::string& command);

private:
    unsigned GetRandom();
    std::string GetWeather() const;
    std::string GetJoke() const;
    void Stop() const;
    void Crash() const;

    std::mt19937 gen_;
    static constexpr std::string_view kWeatherPrediction = "Winter Is Coming";
    static constexpr std::string_view kCodeReviewJoke =
        "Знаете, почему некоторые люди любят проводить code review? "
        "Потому что им нравится чувствовать себя важными и умными, "
        "когда они находят ошибки в чужом коде.";
};