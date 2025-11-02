#ifndef KERNEL_PHILOSOPHERS_H
#define KERNEL_PHILOSOPHERS_H

#include "sync.h"
#include <vector>
#include <memory>

const int NUM_PHILOSOPHERS = 5;

enum class PhilosopherState {
    THINKING,
    HUNGRY,
    EATING
};

class Philosopher {
public:
    int id;
    PhilosopherState state;
    int times_eaten;
    int times_blocked;

    Philosopher(int id);
};

class DiningPhilosophers {
private:
    std::vector<std::unique_ptr<Philosopher>> philosophers;
    std::vector<Semaphore> forks;
    Semaphore room_limit;
    bool simulation_active;

public:
    DiningPhilosophers();

    void start_simulation();
    void stop_simulation();
    bool is_active() const { return simulation_active; }

    void philosopher_think(int id);
    void philosopher_eat(int id);

    void print_status() const;

private:
    int left_fork(int id) const { return id; }
    int right_fork(int id) const { return (id + 1) % NUM_PHILOSOPHERS; }
};

#endif