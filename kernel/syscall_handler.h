#ifndef KERNEL_SYSCALL_HANDLER_H
#define KERNEL_SYSCALL_HANDLER_H

#include <string>
#include <map>
#include "process.h"
#include "protection.h"

enum class SyscallType {
    READ_FILE,
    WRITE_FILE,
    ALLOCATE_MEMORY,
    CREATE_PROCESS,
    EXIT_PROCESS
};

struct SyscallStats {
    int total_calls = 0;
    int successful = 0;
    int failed = 0;
};

class SyscallHandler {
private:
    ProtectionManager* protection_manager;
    std::map<SyscallType, SyscallStats> syscall_stats;

    int total_syscalls = 0;

public:
    SyscallHandler(ProtectionManager* pm);

    bool handle_syscall(Process* proc, SyscallType type, const std::string& args);

    void print_syscall_stats() const;

private:
    bool syscall_read_file(Process* proc, const std::string& filename);
    bool syscall_write_file(Process* proc, const std::string& filename);
    bool syscall_allocate_memory(Process* proc, int size);
    bool syscall_create_process(Process* proc);
    bool syscall_exit(Process* proc);
};

#endif