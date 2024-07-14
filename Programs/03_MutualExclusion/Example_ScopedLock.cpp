// ===========================================================================
// Examples_RecursiveMutex.cpp // std::recursive_mutex
// ===========================================================================

#include <iostream>
#include <print>
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

namespace Scoped_Lock_Example
{
    class Player
    {
    private:
        std::string_view                      m_name;
        std::mutex                            m_mutex;
        std::default_random_engine            m_random_engine;
        std::uniform_int_distribution<size_t> m_dist;
        size_t                                m_score;

    public:
        // c'tor
        Player(std::string_view name, unsigned int seed)
            : m_name{ std::move(name) }, m_random_engine{ seed }, m_dist{ 1, 6 }, m_score{}
        {}

        // getter
        std::string_view name() const { return m_name; }

        size_t getScore() const { return m_score; }

        // public interface
        void incrementScore(size_t points) {

            m_score += points;

            Logger::log(std::cout, name(), " got ", points, " points => Score: ", m_score);
        }

        void playWith(Player& other) {

            if (&other == this) {
                return;
            }

            // retrieve our lock and the lock of the opponent
            std::scoped_lock<std::mutex, std::mutex> lock{ m_mutex, other.m_mutex };

            Logger::log(std::cout, name(), " plays against ", other.name());

            // roll dice until one player wins, then increase the score of the winner
            size_t points{};
            size_t otherPoints{};

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

            // locks are released automatically
        }

    private:
        // roll a six-sided dice
        size_t roll() {
            return m_dist(m_random_engine);
        }
    };
}

void example_scoped_lock()
{
    using namespace Scoped_Lock_Example;

    std::random_device device{};

    std::vector<std::unique_ptr<Player>> players{};

    std::initializer_list<std::string_view> names =
    {
        "Player1", "Player2", "Player3", "Player4", "Player5",
        "Player6", "Player7", "Player8", "Player9"
    };

    // generate players from the names using 'std::transform' algorithm
    std::transform(
        std::begin(names),
        std::end(names),
        std::back_inserter(players),
        [&] (std::string_view name) {
            return std::make_unique<Player>(name, device());
        }
    );

    std::println("Run the game: ");

    // each player plays against all other players in parallel
    std::vector<std::jthread> rounds;
    for (const auto& player : players) {
        
        auto round = [&] () {
            for (const auto& opponent : players) {
                player->playWith(*opponent);
            }
        };
        
        rounds.push_back(std::jthread { std::move(round) });
    }

    // join all the threads
    std::for_each(
        rounds.begin(),
        rounds.end(),
        [](auto& t) { t.join(); }
    );

    std::println("Done.");

    // sort
    std::sort(
        players.begin(),
        players.end(),
        [] (const auto& elem1, const auto& elem2) {
            return elem1->getScore() > elem2->getScore();
        }
    );

    std::println("Final score:");
    for (const auto& player : players) {
        std::println("{}: {}  points.", player->name(), player->getScore());
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
