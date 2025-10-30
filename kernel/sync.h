// kernel/sync.h
#ifndef KERNEL_SYNC_H
#define KERNEL_SYNC_H

#include <deque>
#include <iostream>

/**
 * @brief Simula la primitiva de Semáforo para la simulación del kernel.
 * El semáforo gestiona un contador y una cola de IDs de procesos bloqueados.
 */
class Semaphore {
private:
    int count; // Contador del semáforo (recursos disponibles)
    std::deque<int> blocked_queue; // Cola de PCBs IDs bloqueados esperando el recurso

public:
    /**
     * @brief Constructor.
     * @param initial_count Valor inicial del contador (ej. 1 para mutex, N para recursos).
     */
    Semaphore(int initial_count = 1);

    /**
     * @brief Operación P (Proberen/Wait): Decrementa el contador.
     * Si el contador es negativo, el proceso actual se bloquea.
     * @param pid ID del proceso actual que realiza la operación.
     * @return true si el proceso debe continuar (no bloqueado), false si debe bloquearse.
     */
    bool wait(int pid);

    /**
     * @brief Operación V (Verhogen/Signal): Incrementa el contador.
     * Si el contador es no-positivo, despierta un proceso de la cola de bloqueados.
     * @return El PID del proceso despertado (READY), o -1 si no se despertó a nadie.
     */
    int signal();

    /**
     * @brief Muestra el estado actual del semáforo (para el comando 'stat').
     */
    void print_status(const std::string& name) const;
};

#endif // KERNEL_SYNC_H