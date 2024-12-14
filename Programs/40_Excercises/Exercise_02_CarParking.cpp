// ===========================================================================
// Exercise_02_CarParking.cpp
// ===========================================================================

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#include "../Logger/Logger.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <semaphore>
#include <thread>
#include <vector>

// ===========================================================================

constexpr size_t NumParkingLots = 3;

namespace Car_Parking {

    struct ParkingArea
    {
        virtual void enter() = 0;
        virtual void leave() = 0;
    };

    class FirstParkingArea : public ParkingArea
    {
    private:
        std::counting_semaphore<> m_emptyLots;

    public:
        FirstParkingArea()
            : m_emptyLots{ NumParkingLots } 
        {
            Logger::log(std::cout, "FirstParkingArea has ", NumParkingLots, " empty lots.");
        }

        void enter() {
            m_emptyLots.acquire();
        }

        void leave() {
            m_emptyLots.release();
        }
    };

    class AnotherParkingArea : public ParkingArea
    {
    private:
        std::mutex              m_mutex;
        std::condition_variable m_condition;
        int                     m_emptyLots;

    public:
        AnotherParkingArea() 
            : m_emptyLots{ NumParkingLots }
        {
            Logger::log(std::cout, "AnotherParkingArea has ", NumParkingLots, " empty lots.");
        }

        void enter() {

            {
                std::unique_lock<std::mutex> guard{ m_mutex };

                m_condition.wait(
                    guard,
                    [this]() {
                        return m_emptyLots > 0;
                    }
                );

                --m_emptyLots;
            }
        }

        void leave() {

            {
                std::lock_guard<std::mutex> guard{ m_mutex };
                ++m_emptyLots;
            }

            m_condition.notify_one();
        }
    };

    class Car
    {
    private:
        std::random_device& m_device;
        std::thread::id     m_tid;
        ParkingArea&        m_parkingArea;
        size_t              m_id;

    public:
        Car(std::random_device& device, ParkingArea& parkingArea, size_t id)
            : m_device{ device }, m_parkingArea{ parkingArea }, m_id{ id }
        {
        }

        void driving(std::stop_token token) {

            m_tid = std::this_thread::get_id();

            while (!token.stop_requested())
            {
                Logger::log(std::cout, "Car ", m_id, " is driving");

                // random value from 0 up to 3000
                std::random_device::result_type offset{ m_device() % 3000 };
                std::this_thread::sleep_for(std::chrono::milliseconds{ 3000ll + offset });

                Logger::log(std::cout, "Want to park car ", m_id, " now");

                m_parkingArea.enter();

                Logger::log(std::cout, "Car ", m_id, " is parking now!");

                std::this_thread::sleep_for(std::chrono::milliseconds{ 2000 });

                Logger::log(std::cout, "Want to leave parking area with car ", m_id);

                m_parkingArea.leave();

                Logger::log(std::cout, "Car", m_id, " has left parking area");
            }

            Logger::log(std::cout, "Car ", m_id, " finished driving!");
        }
    };

    class TrafficSimulation
    {
    private:
        ParkingArea&                       m_parkingArea;
        std::vector<std::jthread>          m_threads;
        std::vector<std::unique_ptr<Car>>  m_cars;
        std::random_device                 m_device;
        static size_t                      s_nextId;

    public:
        TrafficSimulation(ParkingArea& parkingArea)
            : m_parkingArea{ parkingArea }
        {}

        void addCar() {

            s_nextId++;
            std::unique_ptr<Car> car{ std::make_unique<Car>(m_device, m_parkingArea, s_nextId) };

            // https://stackoverflow.com/questions/65700593/stdjthread-runs-a-member-function-from-another-member-function
            std::jthread jt{ std::bind_front(&Car::driving, *car) };

            m_cars.push_back(std::move(car));
            m_threads.push_back(std::move(jt));
        }

        void startSimulation() {
            Logger::log(std::cout, "Starting Simulation:");
        }

        void stopSimulation() {

            Logger::log(std::cout, "Issuing Stop Requests ...");

            for (auto& thread : m_threads) {
                thread.request_stop();
            }

            for (auto& thread : m_threads) {
                thread.join();
            }
            Logger::log(std::cout, "Stopped Simulation:");
        }
    };

    size_t TrafficSimulation::s_nextId{};
}


static void carParking() {

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    using namespace Car_Parking;

    FirstParkingArea m_parkingArea;
    TrafficSimulation simulation{ m_parkingArea };
    simulation.startSimulation();
    simulation.addCar();
    simulation.addCar();
    simulation.addCar();
    simulation.addCar();
    simulation.addCar();
    simulation.addCar();

    std::this_thread::sleep_for(std::chrono::milliseconds{ 20000 });

    simulation.stopSimulation();
}

static void anotherCarParking() {

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    using namespace Car_Parking;

    AnotherParkingArea m_parkingArea;
    TrafficSimulation simulation{ m_parkingArea };
    simulation.startSimulation();
    simulation.addCar();
    simulation.addCar();
    simulation.addCar();
    simulation.addCar();
    simulation.addCar();
    simulation.addCar();

    std::this_thread::sleep_for(std::chrono::milliseconds{ 20000 });

    simulation.stopSimulation();
}

void exercise_car_parking() {

    carParking();
    anotherCarParking();
}

// ===========================================================================
// End-of-File
// ===========================================================================
