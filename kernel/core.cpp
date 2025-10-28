// kernel/core.cpp
#include "core.h"

KernelSimulator::KernelSimulator() {
    // Inicialización de componentes (Scheduler, Memoria, etc.)
    std::cout << "Kernel Simulator inicializado." << std::endl;
}

KernelSimulator::~KernelSimulator() {
    std::cout << "Kernel Simulator finalizado." << std::endl;
}

void KernelSimulator::initialize() {
    // Lógica de configuración inicial
}

void KernelSimulator::run_cycle() {
    // Lógica principal de la simulación (ejecutar un paso)
    // 1. Obtener proceso del Scheduler
    // 2. Ejecutar instrucción en la CPU
    // 3. Manejar interrupciones/Syscalls
}