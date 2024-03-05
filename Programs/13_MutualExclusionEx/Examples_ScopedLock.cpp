// ===========================================================================
// Examples_RecursiveMutex.cpp // std::recursive_mutex
// ===========================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <mutex>
#include <random>
#include <algorithm>
#include <memory>
#include <functional>

#include "../Logger/Logger.h"

// https://medium.com/@simontoth/daily-bit-e-of-c-std-scoped-lock-9cab4142f9d4

class Player
{
private:
    std::string_view m_name;
    std::mutex m_mutex;
    std::default_random_engine m_random_engine;
    std::uniform_int_distribution<int> m_dist;
    int m_score;

public:
    // c'tor
    Player(std::string_view name, int seed)
        : m_name(std::move(name)), m_random_engine(seed), m_dist(1, 6), m_score{}
    {}

    // getter
    std::string_view name() const { return m_name; }

    int getScore() const { return m_score; }

    void incrementScore(int points) {

        m_score += points;
        Logger::log(std::cout, name(), " got point => ", m_score);
    }

    // public interface
    void playWith(Player& other) {

        if (&other == this)
            return;

        // retrieve our lock and the lock of the opponent
        std::scoped_lock lock{ m_mutex, other.m_mutex };

        Logger::log(std::cout, name(), " plays against ", other.name());

        // roll a dice until one player wins,
        // then increase the score of the winner
        int points{};
        int otherPoints{};

        while (points == otherPoints)
        {
            points = roll();
            otherPoints = other.roll();
        }

        if (points > otherPoints) {
            incrementScore(points);
        }
        else {
            other.incrementScore(otherPoints);
        }

        // locks released automatically
    }

private:
    // roll a six-sided dice
    int roll() { return m_dist(m_random_engine); }
};

void examples_scoped_lock()
{
    std::random_device device;

    std::vector<std::unique_ptr<Player>> players;

    std::initializer_list<std::string_view> names =
    {
        "Player1", "Player2", "Player3", "Player4", "Player5",
        "Player6", "Player7", "Player8", "Player9"
    };

    // generate players from the names using transform algorithm
    std::transform(
        std::begin(names),
        std::end(names),
        std::back_inserter(players),
        [&] (std::string_view name) {
            return std::make_unique<Player>(name, device());
        }
    );

    std::vector<std::jthread> rounds;

    // run the game:
    // each player plays against all other players in parallel
    for (auto& player : players) {
        
        auto round = [&] () {
            for (auto& opponent : players) {
                player->playWith(*opponent);
            }
        };
        
        rounds.push_back(std::jthread { std::move(round) });
    }

    // join all the threads
    std::for_each(
        rounds.begin(),
        rounds.end(),
        std::mem_fn(&std::jthread::join)
    );

    // sort
    std::sort(
        players.begin(),
        players.end(),
        [] (const auto& elem1, const auto& elem2) {
            return elem1->getScore() > elem2->getScore();
        }
    );

    // print
    std::cout << std::endl << "Final score:" << std::endl;
    for (const auto& player : players) {
        std::cout 
            << player->name() << ":\t" << player->getScore()
            << " points." << std::endl;
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
