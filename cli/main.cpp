// cli/main.cpp
#include <iostream>

// Incluir el Kernel para inicializar la simulaci�n
#include "kernel/core.h" 

// Definir la funci�n principal (implementada en cli/commands.cpp)
void run_cli_commands();

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  Simulador de N�cleo (Kernel-Sim) v0.1 " << std::endl;
    std::cout << "========================================" << std::endl;

    // TODO: Inicializar el Kernel
    KernelSimulator kernel;

    // TODO: Procesar argumentos de l�nea de comandos
    if (argc > 1) {
        // L�gica de CLI
        run_cli_commands();
    }
    else {
        std::cout << "Use 'kernel-sim --help' para ver los comandos." << std::endl;
    }

    return 0;
}