#ifndef CLI_COLORS_H
#define CLI_COLORS_H

#include <string>
#include <iostream>

// Códigos ANSI para colores en Windows Terminal
namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";

    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";

    // Fondos
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN = "\033[46m";
}

// Funciones de utilidad para impresión con colores
inline void print_error(const std::string& msg) {
    std::cout << Colors::RED << Colors::BOLD << "[✗ ERROR] "
        << Colors::RESET << Colors::RED << msg
        << Colors::RESET << std::endl;
}

inline void print_success(const std::string& msg) {
    std::cout << Colors::GREEN << Colors::BOLD << "[✓ OK] "
        << Colors::RESET << Colors::GREEN << msg
        << Colors::RESET << std::endl;
}

inline void print_info(const std::string& msg) {
    std::cout << Colors::CYAN << Colors::BOLD << "[ℹ INFO] "
        << Colors::RESET << Colors::CYAN << msg
        << Colors::RESET << std::endl;
}

inline void print_warning(const std::string& msg) {
    std::cout << Colors::YELLOW << Colors::BOLD << "[⚠ AVISO] "
        << Colors::RESET << Colors::YELLOW << msg
        << Colors::RESET << std::endl;
}

inline void print_header(const std::string& text) {
    std::cout << Colors::BOLD << Colors::CYAN << text
        << Colors::RESET << std::endl;
}

inline void print_section(const std::string& text) {
    std::cout << "\n" << Colors::BOLD << Colors::MAGENTA << text
        << Colors::RESET << std::endl;
}

inline std::string colorize(const std::string& text, const std::string& color) {
    return color + text + Colors::RESET;
}

#endif // CLI_COLORS_H