#pragma once

#include <array>
#include <stdexcept>
#include <queue>
#include <iostream>

enum class Winner { kFirst, kSecond, kNone };

struct GameResult {
    Winner winner;
    int turn;
};

std::queue<int> FillQueue(const std::array<int, 6>& deck) {
    std::queue<int> queue;
    for (const auto& card : deck) {
        queue.push(card);
    }
    return queue;
}

bool CompareCards(int card1, int card2) {
    if ((card1 < card2 && (card1 != 0 || card2 != 11)) || (card1 == 11 && card2 == 0)) {
        return true;
    } else {
        return false;
    }
}

GameResult SimulateWarGame(const std::array<int, 6>& first_deck,
                           const std::array<int, 6>& second_deck) {
    std::queue<int> deck1 = FillQueue(first_deck);
    std::queue<int> deck2 = FillQueue(second_deck);

    int turn = 0;
    while (!deck1.empty() && !deck2.empty() && turn <= 1'000'000) {
        int card1 = deck1.front();
        deck1.pop();
        int card2 = deck2.front();
        deck2.pop();

        if (CompareCards(card1, card2)) {
            deck2.push(card1);
            deck2.push(card2);
        } else {
            deck1.push(card1);
            deck1.push(card2);
        }

        ++turn;
    }

    GameResult res;
    res.turn = turn;

    if (deck1.empty()) {
        res.winner = Winner::kSecond;
    } else if (deck2.empty()) {
        res.winner = Winner::kFirst;
    } else {
        res.winner = Winner::kNone;
    }
    return res;
}
