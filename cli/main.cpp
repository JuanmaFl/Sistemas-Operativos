// cli/main.cpp
#include <iostream>

// Incluir el Kernel para inicializar la simulación
#include "kernel/core.h" 

// Definir la función principal (implementada en cli/commands.cpp)
void run_cli_commands();

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  Simulador de Núcleo (Kernel-Sim) v0.1 " << std::endl;
    std::cout << "========================================" << std::endl;

    // TODO: Inicializar el Kernel
    KernelSimulator kernel;

    // TODO: Procesar argumentos de línea de comandos
    if (argc > 1) {
        // Lógica de CLI
        run_cli_commands();
    }
    else {
        std::cout << "Use 'kernel-sim --help' para ver los comandos." << std::endl;
    }

    return 0;
}