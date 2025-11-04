# Algoritmo del Banquero

**Ubicación:** `kernel/deadlock_detector.cpp`

## 1. Concepto

Evita deadlock mediante **prevención**: antes de asignar recursos, verifica si el sistema quedará en estado seguro.

## 2. Estructuras de Datos
```
Available[m]     // Recursos disponibles
Max[n][m]        // Máximo declarado por proceso
Allocation[n][m] // Actualmente asignado
Need[n][m]       // Necesita = Max - Allocation
```

## 3. Algoritmo
```
PARA cada proceso i:
  SI Need[i] <= Work:
    Work = Work + Allocation[i]
    Finish[i] = true
    Secuencia.add(i)

SI todos Finish[i] == true:
  Estado SEGURO
SINO:
  Estado INSEGURO (posible deadlock)
```

## 4. Ventajas

- **Sin deadlock garantizado**
- Detección proactiva
- Permite multiprogramación segura

## 5. Desventajas

- Requiere declaración de máximos
- Overhead de verificación
- Recursos estáticos

## 6. Ejemplo

**Recursos:** [3, 3, 2]

| PID | Max | Asignado | Necesita |
|-----|-----|----------|----------|
| P0  | [7,5,3] | [0,1,0] | [7,4,3] |
| P1  | [3,2,2] | [2,0,0] | [1,2,2] |
| P2  | [9,0,2] | [3,0,2] | [6,0,0] |

**Secuencia segura:** P1 → P0 → P2

## 7. Implementación

**Solicitud:**
1. Verificar Need >= Request
2. Verificar Available >= Request
3. Asignar temporalmente
4. Ejecutar algoritmo del banquero
5. Si seguro: confirmar; sino: revertir