# 📊 PFF (Page Fault Frequency)

**Ubicación:** `modules/mem/memory_manager.cpp`

## 1. Concepto

PFF ajusta dinámicamente el número de marcos asignados a cada proceso según su **tasa de fallos de página**.

## 2. Algoritmo
```
Si (tasa_fallos > UMBRAL_SUPERIOR):
    Asignar más marcos al proceso
Si (tasa_fallos < UMBRAL_INFERIOR):
    Remover marcos del proceso
```

**Umbrales:**
- UPPER: 0.5 (50% de fallos)
- LOWER: 0.2 (20% de fallos)
- VENTANA: 10 accesos

## 3. Ventajas vs FIFO/LRU

| Característica | FIFO | LRU | PFF |
|----------------|------|-----|-----|
| Previene thrashing | ❌ | ❌ | ✅ |
| Ajuste dinámico | ❌ | ❌ | ✅ |
| Considera carga | ❌ | ❌ | ✅ |
| Complejidad | Baja | Media | Alta |

## 4. Implementación

**Estado por proceso:**
```cpp
struct PFFState {
    int allocated_frames;
    int page_faults;
    double fault_rate;
};
```

**Actualización:**
- Cada acceso calcula: `fault_rate = 1 / tiempo_entre_fallos`
- Compara con umbrales
- Ajusta marcos asignados

## 5. Caso de Uso

Ideal para sistemas con:
- Multiprogramación alta
- Patrones de acceso variables
- Requisitos de QoS

## 6. Comparativa

**Secuencia:** 12 accesos, 4 marcos

| Algoritmo | Fallos | Marcos Dinámicos |
|-----------|--------|------------------|
| FIFO      | 9      | No               |
| LRU       | 6      | No               |
| PFF       | 7      | Sí (2→4)         |

PFF sacrifica eficiencia inicial por estabilidad a largo plazo.