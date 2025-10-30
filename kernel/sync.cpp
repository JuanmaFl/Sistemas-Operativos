// kernel/sync.cpp
#include "sync.h"
#include <iomanip>

Semaphore::Semaphore(int initial_count) : count(initial_count) {}

bool Semaphore::wait(int pid) {
    count--;

    if (count < 0) {
        // El recurso no está disponible. El proceso debe bloquearse.
        blocked_queue.push_back(pid);
        return false; // Indica que el proceso debe ser BLOQUEADO por el Kernel
    }

    // El recurso está disponible. El proceso puede continuar.
    return true;
}

int Semaphore::signal() {
    count++;

    if (count <= 0) {
        // Hay procesos esperando. Despierta al primero en la cola (FIFO).
        if (!blocked_queue.empty()) {
            int pid_to_unblock = blocked_queue.front();
            blocked_queue.pop_front();
            return pid_to_unblock; // Retorna el PID que debe ser puesto en READY
        }
    }

    // Nadie estaba esperando.
    return -1;
}

void Semaphore::print_status(const std::string& name) const {
    std::cout << "  " << std::setw(10) << name << " | ";
    std::cout << "Contador: " << std::setw(3) << count << " | ";
    std::cout << "Bloqueados: [ ";

    // Muestra la cola de procesos bloqueados
    if (blocked_queue.empty()) {
        std::cout << "Vacía ";
    }
    else {
        for (int pid : blocked_queue) {
            std::cout << pid << " ";
        }
    }
    std::cout << "]" << std::endl;
}