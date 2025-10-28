// kernel/core.cpp
#include "core.h"

KernelSimulator::KernelSimulator() {
    // Inicializaci�n de componentes (Scheduler, Memoria, etc.)
    std::cout << "Kernel Simulator inicializado." << std::endl;
}

KernelSimulator::~KernelSimulator() {
    std::cout << "Kernel Simulator finalizado." << std::endl;
}

void KernelSimulator::initialize() {
    // L�gica de configuraci�n inicial
}

void KernelSimulator::run_cycle() {
    // L�gica principal de la simulaci�n (ejecutar un paso)
    // 1. Obtener proceso del Scheduler
    // 2. Ejecutar instrucci�n en la CPU
    // 3. Manejar interrupciones/Syscalls
}