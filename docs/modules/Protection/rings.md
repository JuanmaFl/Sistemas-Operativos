#  Anillos de Protección

**Ubicación:** `kernel/protection.cpp`

## 1. Modelo de Privilegios
```
Ring 0 (Kernel) ────────────── Máximo privilegio
                               - Acceso completo
                               - Hardware directo
                               
Ring 3 (User) ──────────────── Mínimo privilegio
                               - Acceso limitado
                               - Syscalls requeridas
```

## 2. Segmentación

| Segmento | Base   | Límite | R | W | X | Ring |
|----------|--------|--------|---|---|---|------|
| CODE     | 0x0000 | 0x1000 | ✓ | ✗ | ✓ | 3    |
| DATA     | 0x1000 | 0x2000 | ✓ | ✓ | ✗ | 3    |
| STACK    | 0x2000 | 0x3000 | ✓ | ✓ | ✗ | 3    |
| KERNEL   | 0x3000 | 0x4000 | ✓ | ✓ | ✓ | 0    |

## 3. Syscalls

**Flujo:**
1. Proceso en Ring 3 invoca syscall
2. Trap al kernel (elevación a Ring 0)
3. Validación de privilegios
4. Ejecución de operación
5. Retorno a Ring 3

## 4. Violaciones

- **Segmentación:** Acceso fuera de límites
- **Privilegios:** Ring 3 intenta acceder a Ring 0
- **Permisos:** Escritura en CODE, ejecución en DATA

## 5. Implementación
```cpp
// Verificación
if (proc->privilege_level > required_level) {
    violation();
    return false;
}

// Syscall
proc->elevate_privilege();  // Ring 3 → Ring 0
execute_operation();
proc->lower_privilege();    // Ring 0 → Ring 3
```