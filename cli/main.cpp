#include <iostream>
#include <string>
#include <vector>

// Incluir el Kernel y la función de manejo de comandos
#include "kernel/core.h"
#include "commands.h" 

//  IMPORTANTE: Instancia global del kernel
// Esto permite que la función handle_command (en commands.cpp) acceda al kernel.
KernelSimulator kernel;

int main(int argc, char* argv[]) {
    // El constructor de 'kernel' ya imprime la cabecera.

    // Si se dan argumentos (ej. ./kernel-sim.exe --load file.txt), podemos procesarlos.
    if (argc > 1) {
        // Por ahora, solo muestra que no hay soporte avanzado y ejecuta la lógica principal
        std::cout << "[INFO] Argumentos de línea de comandos detectados. Procesando..." << std::endl;
        // Más tarde, aquí se llamaría a una función para parsear 'argv'
    }

    std::cout << "\nUse 'help' para ver los comandos. Inicie con 'new <burst>'." << std::endl;

    
    std::string line;
    while (true) {
        std::cout << "sim> ";
        if (!std::getline(std::cin, line)) {
            break; // Sale si falla la lectura (ej. EOF)
        }

        // La función implementada en commands.cpp maneja la entrada
        handle_command(line);
    }

    return 0; // Se llama al destructor del 'kernel' al salir.
}