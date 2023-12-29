// ===========================================================================
// Program.cpp // Locking Mechanisms
// ===========================================================================

#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <algorithm>
#include <utility>


// Hmmmm... mit Gcc geht das alles ?????????????


// https://medium.com/@simontoth/daily-bit-e-of-c-std-scoped-lock-9cab4142f9d4

struct Player
{
private:
    std::string m_name;
    std::default_random_engine re_;
    std::uniform_int_distribution<int> m_dist;
    std::mutex m_mutex;
    int m_score;

public:
    Player(std::string name, uint64_t seed)    // uint64_t
        : m_name(std::move(name)), re_(seed), m_dist(1, 6), m_mutex{}, m_score{ }
    {}

    void play_with(Player& other) {

        if (&other == this)
            return;
        
        // to play a game we need to obtain both our lock and the lock
        // of the oponent which creates potential for deadlock
        std::scoped_lock lock(m_mutex, other.m_mutex);
        // Roll a dice until one player wins, then increase the score
        int our = 0, them = 0;
        do {
            our = roll();
            them = other.roll();
        } while (our == them);
        if (our > them)
            m_score++;
        else
            other.m_score++;
        // lock released
    }

    const std::string& name() const { return m_name; }

    int score() const { return m_score; }

private:
    // Roll a D6
    int roll() { return m_dist(re_); }

};

int main()
{
    std::random_device r;

    std::vector<std::unique_ptr<Player>> players;

    auto names = 
    { 
        "Player1", "Player2", "Player3", "Player4", "Player5",
        "Player6", "Player7", "Player8", "Player9" 
    };

    // generate players from the names
    std::ranges::transform(names, std::back_inserter(players),
        [&](const char* name) {
            return std::make_unique<Player>(name, r());
        }
    );

    // Run the tournament:
    // each player plays all other players in parallel
    std::vector<std::jthread> rounds;
    for (auto& v : players) {
        rounds.push_back(std::jthread([&players, &v] {
            for (auto& oponent : players) {
                v->play_with(*oponent);
            }
            }
        )
        );
    }

    rounds.clear(); // a.k.a. join all threads

    // Sort and print
    std::ranges::sort(players, std::greater<>{},
        [](const std::unique_ptr<Player>& p) {
            return p->score();
        }
    );

    for (const auto& v : players) {
        std::cout << v->name() << " " << v->score() << "\n";
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
