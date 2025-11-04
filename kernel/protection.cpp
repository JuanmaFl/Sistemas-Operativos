#include "protection.h"
#include <iostream>
#include <iomanip>

ProtectionManager::ProtectionManager() {
    register_segment(MemorySegment::CODE, 0x0000, 0x1000, true, false, true, PrivilegeLevel::USER);
    register_segment(MemorySegment::DATA, 0x1000, 0x2000, true, true, false, PrivilegeLevel::USER);
    register_segment(MemorySegment::STACK, 0x2000, 0x3000, true, true, false, PrivilegeLevel::USER);
    register_segment(MemorySegment::KERNEL, 0x3000, 0x4000, true, true, true, PrivilegeLevel::KERNEL);

    std::cout << "[PROTECTION] Sistema de protección inicializado (Rings: 0=Kernel, 3=User)." << std::endl;
}

void ProtectionManager::register_segment(MemorySegment seg, int base, int limit,
    bool r, bool w, bool x, PrivilegeLevel level) {
    SegmentDescriptor desc;
    desc.base_address = base;
    desc.limit = limit;
    desc.readable = r;
    desc.writable = w;
    desc.executable = x;
    desc.required_level = level;

    segment_table[seg] = desc;
}

bool ProtectionManager::check_memory_access(Process* proc, int address, AccessType access) {
    total_checks++;

    MemorySegment target_seg;
    if (address < 0x1000) target_seg = MemorySegment::CODE;
    else if (address < 0x2000) target_seg = MemorySegment::DATA;
    else if (address < 0x3000) target_seg = MemorySegment::STACK;
    else target_seg = MemorySegment::KERNEL;

    const SegmentDescriptor& desc = segment_table[target_seg];

    if (address < desc.base_address || address >= desc.limit) {
        violations++;
        std::cout << "[PROTECTION] ✗ Violación: PID " << proc->id
            << " - Dirección fuera de límites (0x" << std::hex << address << std::dec << ")" << std::endl;
        return false;
    }

    if (proc->privilege_level > desc.required_level) {
        violations++;
        std::cout << "[PROTECTION] ✗ Violación: PID " << proc->id
            << " - Privilegios insuficientes para segmento KERNEL." << std::endl;
        return false;
    }

    switch (access) {
    case AccessType::READ:
        if (!desc.readable) {
            violations++;
            std::cout << "[PROTECTION] ✗ Violación: PID " << proc->id
                << " - Lectura no permitida." << std::endl;
            return false;
        }
        break;
    case AccessType::WRITE:
        if (!desc.writable) {
            violations++;
            std::cout << "[PROTECTION] ✗ Violación: PID " << proc->id
                << " - Escritura no permitida." << std::endl;
            return false;
        }
        break;
    case AccessType::EXECUTE:
        if (!desc.executable) {
            violations++;
            std::cout << "[PROTECTION] ✗ Violación: PID " << proc->id
                << " - Ejecución no permitida." << std::endl;
            return false;
        }
        break;
    }

    std::cout << "[PROTECTION] ✓ Acceso permitido: PID " << proc->id
        << " (0x" << std::hex << address << std::dec << ")" << std::endl;
    return true;
}

bool ProtectionManager::check_privilege(Process* proc, PrivilegeLevel required) {
    if (proc->privilege_level > required) {
        violations++;
        std::cout << "[PROTECTION] ✗ Privilegios insuficientes: PID " << proc->id << std::endl;
        return false;
    }
    return true;
}

void ProtectionManager::print_segment_table() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "       TABLA DE SEGMENTOS" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << std::setw(10) << "Segmento"
        << std::setw(10) << "Base"
        << std::setw(10) << "Límite"
        << std::setw(8) << "R"
        << std::setw(8) << "W"
        << std::setw(8) << "X"
        << std::setw(10) << "Ring" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    const char* seg_names[] = { "CODE", "DATA", "STACK", "KERNEL" };
    int idx = 0;

    for (const auto& pair : segment_table) {
        const SegmentDescriptor& desc = pair.second;

        std::cout << std::setw(10) << seg_names[idx++]
            << std::hex
            << std::setw(10) << desc.base_address
            << std::setw(10) << desc.limit
            << std::dec
            << std::setw(8) << (desc.readable ? "Sí" : "No")
            << std::setw(8) << (desc.writable ? "Sí" : "No")
            << std::setw(8) << (desc.executable ? "Sí" : "No")
            << std::setw(10) << (desc.required_level == PrivilegeLevel::KERNEL ? "0" : "3")
            << std::endl;
    }

    std::cout << "========================================\n" << std::endl;
}

void ProtectionManager::print_protection_stats() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     ESTADÍSTICAS DE PROTECCIÓN" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total Verificaciones: " << total_checks << std::endl;
    std::cout << "Violaciones: " << violations << std::endl;
    std::cout << "Escalaciones de Privilegio: " << privilege_escalations << std::endl;

    if (total_checks > 0) {
        double violation_rate = (static_cast<double>(violations) / total_checks) * 100.0;
        std::cout << "Tasa de Violaciones: " << std::fixed << std::setprecision(2)
            << violation_rate << "%" << std::endl;
    }

    std::cout << "========================================\n" << std::endl;
}