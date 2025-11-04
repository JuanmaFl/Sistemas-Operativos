#include "syscall_handler.h"
#include <iostream>
#include <iomanip>

SyscallHandler::SyscallHandler(ProtectionManager* pm)
    : protection_manager(pm) {
    std::cout << "[SYSCALL] Handler inicializado." << std::endl;
}

bool SyscallHandler::handle_syscall(Process* proc, SyscallType type, const std::string& args) {
    total_syscalls++;
    proc->syscall_count++;

    std::cout << "[SYSCALL] PID " << proc->id << " invoca syscall (Ring "
        << (proc->is_kernel_mode() ? "0" : "3") << ")" << std::endl;

    proc->elevate_privilege();

    bool success = false;

    switch (type) {
    case SyscallType::READ_FILE:
        success = syscall_read_file(proc, args);
        break;
    case SyscallType::WRITE_FILE:
        success = syscall_write_file(proc, args);
        break;
    case SyscallType::ALLOCATE_MEMORY:
        success = syscall_allocate_memory(proc, std::stoi(args));
        break;
    case SyscallType::CREATE_PROCESS:
        success = syscall_create_process(proc);
        break;
    case SyscallType::EXIT_PROCESS:
        success = syscall_exit(proc);
        break;
    }

    proc->lower_privilege();

    syscall_stats[type].total_calls++;
    if (success) {
        syscall_stats[type].successful++;
    }
    else {
        syscall_stats[type].failed++;
    }

    return success;
}

bool SyscallHandler::syscall_read_file(Process* proc, const std::string& filename) {
    std::cout << "  [READ] PID " << proc->id << " lee archivo: " << filename << std::endl;
    return true;
}

bool SyscallHandler::syscall_write_file(Process* proc, const std::string& filename) {
    std::cout << "  [WRITE] PID " << proc->id << " escribe archivo: " << filename << std::endl;
    return true;
}

bool SyscallHandler::syscall_allocate_memory(Process* proc, int size) {
    std::cout << "  [ALLOC] PID " << proc->id << " solicita " << size << " bytes." << std::endl;
    return true;
}

bool SyscallHandler::syscall_create_process(Process* proc) {
    std::cout << "  [FORK] PID " << proc->id << " crea proceso hijo." << std::endl;
    return true;
}

bool SyscallHandler::syscall_exit(Process* proc) {
    std::cout << "  [EXIT] PID " << proc->id << " termina." << std::endl;
    return true;
}

void SyscallHandler::print_syscall_stats() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "      ESTADÍSTICAS DE SYSCALLS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total Syscalls: " << total_syscalls << std::endl;

    std::cout << "\nPor Tipo:" << std::endl;
    std::cout << std::setw(20) << "Syscall"
        << std::setw(10) << "Total"
        << std::setw(10) << "Éxito"
        << std::setw(10) << "Fallo" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    const char* names[] = { "READ", "WRITE", "ALLOC", "FORK", "EXIT" };
    int idx = 0;

    for (const auto& pair : syscall_stats) {
        const SyscallStats& stats = pair.second;
        std::cout << std::setw(20) << names[idx++]
            << std::setw(10) << stats.total_calls
            << std::setw(10) << stats.successful
            << std::setw(10) << stats.failed << std::endl;
    }

    std::cout << "========================================\n" << std::endl;
}