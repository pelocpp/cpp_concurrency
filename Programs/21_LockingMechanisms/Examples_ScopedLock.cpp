#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <mutex>
#include <random>
#include <algorithm>
#include <utility>
#include <memory>
#include <functional>

// https://medium.com/@simontoth/daily-bit-e-of-c-std-scoped-lock-9cab4142f9d4

struct Player
{
private:
    // std::string m_name;
    std::string_view m_name;
    std::mutex m_mutex;
    std::default_random_engine m_random_engine;
    std::uniform_int_distribution<int> m_dist;
    int m_score;

public:
    Player(std::string_view name, int seed)
        : m_name(std::move(name)), m_random_engine(seed), m_dist(1, 6), m_mutex{}, m_score{}
    {}

    // getter
    std::string_view name() const { return m_name; }
    int score() const { return m_score; }

    // public interface
    void playWith(Player& other) {

        if (&other == this)
            return;

        // to play a game we need to obtain both our lock and the lock
        // of the oponent which creates potential for deadlock
        std::scoped_lock lock{ m_mutex, other.m_mutex };

        // roll a dice until one player wins, then increase the score
        int our{};
        int them{};

        do {
            our = roll();
            them = other.roll();
        } while (our == them);

        if (our > them) {
            ++m_score;
        }
        else {
            ++other.m_score;
        }

        // locks released
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

    // generate players from the names
    //std::ranges::transform(names, std::back_inserter(players),
    //    [&](const char* name) {
    //        return std::make_unique<Player>(name, device());
    //    }
    //);

    // generate players from the names using transform algorithm
    std::transform(
        std::begin(names),
        std::end(names),
        std::back_inserter(players),
        [&](std::string_view name) {
            return std::make_unique<Player>(name, device());
        }
    );


    //// Run the tournament:
    //// each player plays all other players in parallel
    //std::vector<std::jthread> rounds;
    //for (auto& v : players) {
    //    rounds.push_back(std::jthread([&players, &v] {
    //        for (auto& oponent : players) {
    //            v->playWith(*oponent);
    //        }
    //        }
    //    )
    //    );
    //}

    // Run the tournament:
    // each player plays all other players in parallel
    std::vector<std::jthread> rounds;

    
    for (auto& player : players) {
        
        auto round = [&]() {
            for (auto& oponent : players) {
                player->playWith(*oponent);
            }
        };
        
        rounds.push_back(std::jthread { round });
        
        //rounds.push_back(std::jthread([&players, &v] {
        //    for (auto& oponent : players) {
        //        v->playWith(*oponent);
        //    }
        //    }
        //)
        //);
    }

       std::for_each(
           rounds.begin(),
           rounds.end(),
std::mem_fn(&std::thread::join)
);

  //  rounds.clear(); // a.k.a. join all threads

    // Sort and print
    //std::ranges::sort(players, std::greater<>{},
    //    [](const std::unique_ptr<Player>& p) {
    //        return p->score();
    //    }
    //);

    for (const auto& v : players) {
        std::cout << v->name() << " " << v->score() << "\n";
    }
}

//void examples_scoped_lock()
//{
//    std::random_device device;
//
//    std::vector<std::unique_ptr<Player>> players;
//
//    auto names =
//    {
//        "Player1", "Player2", "Player3", "Player4", "Player5",
//        "Player6", "Player7", "Player8", "Player9"
//    };
//
//    // generate players from the names
//    std::ranges::transform(names, std::back_inserter(players),
//        [&](const char* name) {
//            return std::make_unique<Player>(name, device());
//        }
//    );
//
//    // Run the tournament:
//    // each player plays all other players in parallel
//    std::vector<std::jthread> rounds;
//    for (auto& v : players) {
//        rounds.push_back(std::jthread([&players, &v] {
//            for (auto& oponent : players) {
//                v->playWith(*oponent);
//            }
//            }
//        )
//        );
//    }
//
//    rounds.clear(); // a.k.a. join all threads
//
//    // Sort and print
//    //std::ranges::sort(players, std::greater<>{},
//    //    [](const std::unique_ptr<Player>& p) {
//    //        return p->score();
//    //    }
//    //);
//
//    for (const auto& v : players) {
//        std::cout << v->name() << " " << v->score() << "\n";
//    }
//}
