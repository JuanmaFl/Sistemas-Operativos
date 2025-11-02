#include "philosophers.h"
#include <iostream>
#include <iomanip>

Philosopher::Philosopher(int id)
    : id(id), state(PhilosopherState::THINKING), times_eaten(0), times_blocked(0) {
}

DiningPhilosophers::DiningPhilosophers()
    : room_limit(NUM_PHILOSOPHERS - 1), simulation_active(false) {

    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.push_back(std::make_unique<Philosopher>(i));
        forks.push_back(Semaphore(1));
    }
}

void DiningPhilosophers::start_simulation() {
    simulation_active = true;
    std::cout << "[PHILOSOPHERS] Simulación iniciada (Máximo " << NUM_PHILOSOPHERS - 1 << " filósofos en mesa)." << std::endl;
}

void DiningPhilosophers::stop_simulation() {
    simulation_active = false;
    for (auto& p : philosophers) {
        p->state = PhilosopherState::THINKING;
    }
    std::cout << "[PHILOSOPHERS] Simulación detenida." << std::endl;
}

void DiningPhilosophers::philosopher_think(int id) {
    if (!simulation_active || id < 0 || id >= NUM_PHILOSOPHERS) return;

    auto& phil = philosophers[id];
    phil->state = PhilosopherState::THINKING;
    std::cout << "[PHIL-" << id << "] PENSANDO..." << std::endl;
}

void DiningPhilosophers::philosopher_eat(int id) {
    if (!simulation_active || id < 0 || id >= NUM_PHILOSOPHERS) {
        std::cout << "[ERROR] Filósofo " << id << " inválido o simulación inactiva." << std::endl;
        return;
    }

    auto& phil = philosophers[id];
    phil->state = PhilosopherState::HUNGRY;
    std::cout << "[PHIL-" << id << "] HAMBRIENTO - Intenta entrar a la mesa..." << std::endl;

    if (!room_limit.wait(id)) {
        std::cout << "  > Mesa llena. PHIL-" << id << " espera fuera (prevención deadlock)." << std::endl;
        phil->times_blocked++;
        return;
    }

    std::cout << "  > PHIL-" << id << " entra a la mesa." << std::endl;

    int left = left_fork(id);
    int right = right_fork(id);

    if (!forks[left].wait(id)) {
        std::cout << "  > Tenedor izquierdo [" << left << "] ocupado. PHIL-" << id << " BLOQUEADO." << std::endl;
        phil->times_blocked++;
        room_limit.signal();
        return;
    }
    std::cout << "  > PHIL-" << id << " toma tenedor IZQUIERDO [" << left << "]." << std::endl;

    if (!forks[right].wait(id)) {
        std::cout << "  > Tenedor derecho [" << right << "] ocupado. PHIL-" << id << " suelta tenedor izquierdo." << std::endl;
        phil->times_blocked++;
        forks[left].signal();
        room_limit.signal();
        return;
    }
    std::cout << "  > PHIL-" << id << " toma tenedor DERECHO [" << right << "]." << std::endl;

    phil->state = PhilosopherState::EATING;
    phil->times_eaten++;
    std::cout << "  > [COMIENDO] PHIL-" << id << " está comiendo (Total: " << phil->times_eaten << " veces)." << std::endl;

    forks[left].signal();
    forks[right].signal();
    room_limit.signal();

    std::cout << "  > PHIL-" << id << " suelta ambos tenedores y sale de la mesa." << std::endl;
    phil->state = PhilosopherState::THINKING;
}

void DiningPhilosophers::print_status() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "   ESTADO DE LOS FILÓSOFOS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Estado: " << (simulation_active ? "ACTIVA" : "INACTIVA") << std::endl;
    std::cout << "Límite de mesa: " << NUM_PHILOSOPHERS - 1 << " filósofos" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    std::cout << std::setw(6) << "ID"
        << std::setw(12) << "Estado"
        << std::setw(10) << "Comidas"
        << std::setw(10) << "Bloqueos" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (const auto& p : philosophers) {
        std::string state_str;
        switch (p->state) {
        case PhilosopherState::THINKING: state_str = "PENSANDO"; break;
        case PhilosopherState::HUNGRY: state_str = "HAMBRIENTO"; break;
        case PhilosopherState::EATING: state_str = "COMIENDO"; break;
        }

        std::cout << std::setw(6) << p->id
            << std::setw(12) << state_str
            << std::setw(10) << p->times_eaten
            << std::setw(10) << p->times_blocked << std::endl;
    }

    std::cout << "========================================\n" << std::endl;
}