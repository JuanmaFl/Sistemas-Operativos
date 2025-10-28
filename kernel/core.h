// kernel/core.h
#ifndef KERNEL_CORE_H
#define KERNEL_CORE_H

#include <iostream>
#include <vector>

// Declaraciones de forward para evitar incluir headers innecesarios
// class Process; // Si defines una clase Process
// class Scheduler; // Si defines una clase Scheduler

class KernelSimulator {
public:
    KernelSimulator();
    ~KernelSimulator();

    void initialize();
    void run_cycle();

private:
    // Punteros a otros módulos (CPU, Memoria, Scheduler)
    // Scheduler* scheduler_;
    // Processor* cpu_;
};

#endif // KERNEL_CORE_H