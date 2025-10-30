# Gestión de Procesos y Planificación (Módulo CPU)

**Ubicación:** `kernel/scheduler.h`, `kernel/scheduler.cpp`

## 1. Diseño del Planificador (`Scheduler`)

La clase `Scheduler` es responsable de la gestión de la cola de procesos listos (`ready_queue`) y la decisión sobre qué proceso (`running_process`) debe obtener la CPU en un momento dado.

| Atributo Clave | Propósito |
| :--- | :--- |
| `algorithm` | Define el algoritmo activo (`ROUND_ROBIN` o `SJF`). |
| `quantum_size` | Tiempo máximo que un proceso puede usar la CPU en Round Robin. |
| `ready_queue` | Cola de doble extremo (`std::deque` o `std::list`) para procesos listos. |
| `blocked_queue` | Cola de procesos esperando un recurso (usada por Sincronización). |

## 2. Algoritmos de Planificación

### 2.1. Round Robin (RR)

**Características:** Expropiativo, justo.
**Mecanismo:**

1.  El proceso en `RUNNING` se ejecuta por un máximo de `quantum_size` ticks.
2.  La expropiación se evalúa en el método `tick()`:
    ```cpp
    if (running_process->time_on_cpu >= quantum_size) {
        // Mueve a READY
        ready_queue.push_back(running_process);
        running_process = nullptr;
        dispatch();
    }
    ```
3.  **Comando de Configuración:** `set_quantum <n>` permite modificar `quantum_size` en tiempo de ejecución.

### 2.2. Shortest Job First (SJF) No Expropiativo

**Características:** No expropiativo, óptimo en minimizar el tiempo de espera promedio.
**Mecanismo:**

1.  Se activa cuando la CPU está libre (`running_process == nullptr`).
2.  El método `dispatch()` recorre la `ready_queue` buscando el proceso con el menor valor de **`remaining_time`** y lo mueve a `RUNNING`.
3.  Una vez iniciado, el proceso no puede ser desalojado por el planificador (solo por terminación o bloqueo de E/S).

## 3. Métricas de Rendimiento

El planificador calcula las siguientes métricas al finalizar un proceso (`remaining_time == 0`):

* **Turnaround Time (Tiempo de Retorno):** Tiempo total que el proceso pasó en el sistema (Terminación - Llegada).
* **Wait Time (Tiempo de Espera):** Tiempo total que el proceso pasó en la cola `READY`.

## 4. Pruebas y Comandos

| Comando CLI | Propósito |
| :--- | :--- |
| `new <burst>` | Crea un nuevo proceso. |
| `set_algo <rr|sjf>` | Selecciona el algoritmo de planificación. |
| `set_quantum <n>` | Configura el quantum para RR. |
| `run <n>` / `tick` | Avanza el reloj e inicia el planificador. |
| `ps` | Muestra el estado actual y las métricas de los procesos. |