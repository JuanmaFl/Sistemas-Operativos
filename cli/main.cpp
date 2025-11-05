#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "kernel/core.h"
#include "commands.h"

// Instancia global del kernel
KernelSimulator kernel;

void setup_console_utf8() {
#ifdef _WIN32
    // Configurar UTF-8 en consola Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Habilitar secuencias ANSI (colores)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#else
    
    std::cout.imbue(std::locale(""));
#endif
}

int main(int argc, char* argv[]) {
    // Configurar UTF-8 y colores
    setup_console_utf8();

    // El constructor de 'kernel' ya imprime la cabecera (ahora con UTF-8)

    // Procesar argumentos de línea de comandos
    if (argc > 1) {
        std::cout << "\033[36m[INFO]\033[0m Argumentos de línea de comandos detectados. Procesando..." << std::endl;
    }

    std::cout << "\n\033[1;32m✓ Sistema listo.\033[0m Use 'help' para ver los comandos. Inicie con 'new <burst>'." << std::endl;

    std::string line;
    while (true) {
        std::cout << "\033[1;34msim>\033[0m ";
        if (!std::getline(std::cin, line)) {
            break; // Sale si falla la lectura (ej. EOF)
        }

        handle_command(line);
    }

    return 0;
}