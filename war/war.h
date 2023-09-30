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

std::queue<int> fillQueue(const std::array<int, 6>& deck) {
    std::queue<int> queue;
    for (const auto& card : deck) {
        queue.push(card);
    }
    return queue;
}

bool compareCards(int card1, int card2) {
    if ((card1 < card2 && (card1 != 0 || card2 != 11)) || (card1 == 11 && card2 == 0)) {
        return true;
    } else {
        return false;
    }
}

GameResult SimulateWarGame(const std::array<int, 6>& first_deck,
                           const std::array<int, 6>& second_deck) {
    std::queue<int> deck1 = fillQueue(first_deck);
    std::queue<int> deck2 = fillQueue(second_deck);

    int turn = 0;
    while(deck1.size() > 0 && deck2.size() > 0 && turn <= 1'000'000) {
        // std::cout << "Deck1: " << deck1 << std::endl;
        // std::cout << "Deck2: " << deck2 << std::endl;
        int card1 = deck1.front();
        deck1.pop();
        int card2 = deck2.front();
        deck2.pop();

        if (compareCards(card1, card2)) {
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

    if (deck1.size() == 0) {
        res.winner = Winner::kSecond;
    } else if (deck2.size() == 0) {
        res.winner = Winner::kFirst;
    } else {
        res.winner = Winner::kNone;
    }
    return res;
}
