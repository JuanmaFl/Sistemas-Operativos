#ifndef KERNEL_PROTECTION_H
#define KERNEL_PROTECTION_H

#include <string>
#include <map>
#include <vector>
#include "process.h"

enum class MemorySegment {
    CODE,
    DATA,
    STACK,
    KERNEL
};

enum class AccessType {
    READ,
    WRITE,
    EXECUTE
};

struct SegmentDescriptor {
    int base_address;
    int limit;
    bool readable;
    bool writable;
    bool executable;
    PrivilegeLevel required_level;
};

class ProtectionManager {
private:
    std::map<MemorySegment, SegmentDescriptor> segment_table;

    int total_checks = 0;
    int violations = 0;
    int privilege_escalations = 0;

public:
    ProtectionManager();

    bool check_memory_access(Process* proc, int address, AccessType access);
    bool check_privilege(Process* proc, PrivilegeLevel required);

    void register_segment(MemorySegment seg, int base, int limit,
        bool r, bool w, bool x, PrivilegeLevel level);

    void print_segment_table() const;
    void print_protection_stats() const;

    int get_violations() const { return violations; }
};

#endif