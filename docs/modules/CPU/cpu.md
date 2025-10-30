# Gesti�n de Procesos y Planificaci�n (M�dulo CPU)

**Ubicaci�n:** `kernel/scheduler.h`, `kernel/scheduler.cpp`

## 1. Dise�o del Planificador (`Scheduler`)

La clase `Scheduler` es responsable de la gesti�n de la cola de procesos listos (`ready_queue`) y la decisi�n sobre qu� proceso (`running_process`) debe obtener la CPU en un momento dado.

| Atributo Clave | Prop�sito |
| :--- | :--- |
| `algorithm` | Define el algoritmo activo (`ROUND_ROBIN` o `SJF`). |
| `quantum_size` | Tiempo m�ximo que un proceso puede usar la CPU en Round Robin. |
| `ready_queue` | Cola de doble extremo (`std::deque` o `std::list`) para procesos listos. |
| `blocked_queue` | Cola de procesos esperando un recurso (usada por Sincronizaci�n). |

## 2. Algoritmos de Planificaci�n

### 2.1. Round Robin (RR)

**Caracter�sticas:** Expropiativo, justo.
**Mecanismo:**

1.  El proceso en `RUNNING` se ejecuta por un m�ximo de `quantum_size` ticks.
2.  La expropiaci�n se eval�a en el m�todo `tick()`:
    ```cpp
    if (running_process->time_on_cpu >= quantum_size) {
        // Mueve a READY
        ready_queue.push_back(running_process);
        running_process = nullptr;
        dispatch();
    }
    ```
3.  **Comando de Configuraci�n:** `set_quantum <n>` permite modificar `quantum_size` en tiempo de ejecuci�n.

### 2.2. Shortest Job First (SJF) No Expropiativo

**Caracter�sticas:** No expropiativo, �ptimo en minimizar el tiempo de espera promedio.
**Mecanismo:**

1.  Se activa cuando la CPU est� libre (`running_process == nullptr`).
2.  El m�todo `dispatch()` recorre la `ready_queue` buscando el proceso con el menor valor de **`remaining_time`** y lo mueve a `RUNNING`.
3.  Una vez iniciado, el proceso no puede ser desalojado por el planificador (solo por terminaci�n o bloqueo de E/S).

## 3. M�tricas de Rendimiento

El planificador calcula las siguientes m�tricas al finalizar un proceso (`remaining_time == 0`):

* **Turnaround Time (Tiempo de Retorno):** Tiempo total que el proceso pas� en el sistema (Terminaci�n - Llegada).
* **Wait Time (Tiempo de Espera):** Tiempo total que el proceso pas� en la cola `READY`.

## 4. Pruebas y Comandos

| Comando CLI | Prop�sito |
| :--- | :--- |
| `new <burst>` | Crea un nuevo proceso. |
| `set_algo <rr|sjf>` | Selecciona el algoritmo de planificaci�n. |
| `set_quantum <n>` | Configura el quantum para RR. |
| `run <n>` / `tick` | Avanza el reloj e inicia el planificador. |
| `ps` | Muestra el estado actual y las m�tricas de los procesos. |