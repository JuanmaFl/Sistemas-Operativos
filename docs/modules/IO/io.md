#  Sincronización y Comunicación (Módulo IO)

**Ubicación:** `modules/io/io_manager.h`, `modules/io/io_manager.cpp` (o similar)

## 1. Mecanismos de Sincronización

La simulación implementa los mecanismos de sincronización utilizando primitivas de bajo nivel (semáforos o mutex simulados) para proteger recursos compartidos y coordinar procesos.

## 2. Problema Canónico: Productor-Consumidor (P/C)

El problema de Productor-Consumidor se modela sobre un **buffer compartido** simulado por el núcleo, al que solo puede acceder el proceso que actualmente está en estado `RUNNING`.

### 2.1. Diseño y Semáforos

El P/C utiliza tres (3) semáforos/mutex conceptuales para garantizar el acceso seguro y evitar desbordamientos/subdesbordamientos:

1.  **Mutex (`Mutex`):** Un mutex de exclusión mutua para garantizar que solo un proceso a la vez acceda al *buffer* (protege las variables compartidas).
2.  **`Empty` (Semáforo Contador):** Cuenta el número de slots vacíos en el *buffer*. Permite a los Productores operar si `Empty > 0`.
3.  **`Full` (Semáforo Contador):** Cuenta el número de items ocupados en el *buffer*. Permite a los Consumidores operar si `Full > 0`.

### 2.2. Flujo de Comandos

| Comando | Acción del Proceso RUNNING | Lógica de Sincronización |
| :--- | :--- | :--- |
| `produce` | Intenta añadir un item al *buffer*. | Espera en `Empty` (P), adquiere `Mutex`, libera `Full` (V). |
| `consume` | Intenta remover un item del *buffer*. | Espera en `Full` (P), adquiere `Mutex`, libera `Empty` (V). |
| `sync_stat` | Muestra el estado actual del *buffer* (items y tamaño). | Muestra los valores de `Empty` y `Full`. |

### 2.3. Invariantes

La corrección del módulo de sincronización se mantiene mediante los siguientes invariantes:

* **Integridad del Buffer:** `Número de Items en Buffer + Número de Slots Vacíos = Capacidad Total del Buffer`.
* **Exclusión Mutua:** En cualquier momento, solo un hilo (el Productor o el Consumidor) puede estar manipulando las variables de índice del *buffer* (`in` o `out`).
* **No Subdesbordamiento:** Un Consumidor solo puede proceder si `Full > 0` (es decir, si hay items disponibles).