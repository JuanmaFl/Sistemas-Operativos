# 🏛️ Arquitectura del Simulador de Núcleo (Kernel-Sim)

**Autores:** Laura Sofía Aceros, Juan Manuel Flórez

**Versión:** 1.0 (Hito de Clase)

## 1. Alcance y Objetivos

El objetivo de Kernel-Sim es simular los componentes fundamentales de un sistema operativo (Planificación, Gestión de Memoria, Sincronización) a través de una Interfaz de Línea de Comandos (CLI), permitiendo el análisis del comportamiento de diferentes algoritmos.

## 2. Modelo de Arquitectura

El simulador sigue un **Modelo Modular por Capas** centrado en la clase `KernelSimulator`. Esta clase actúa como la **Interfaz de Llamada al Sistema (System Call Interface)**, centralizando la comunicación entre la CLI y los módulos internos del núcleo.

* **Capa de Presentación (CLI):** Procesa la entrada del usuario (`cli/commands.cpp`) y expone las funcionalidades del núcleo.
* **Capa del Núcleo (KernelSimulator):** Coordina los módulos, mantiene el estado global (tiempo, lista de procesos), y traduce los comandos a operaciones del sistema.
* **Capas de Servicios (Modules):** Implementan la lógica de bajo nivel (planificación, gestión de memoria, E/S).

## 3. Estructuras Centrales de Datos

### Bloque de Control de Proceso (PCB)

La clase `Process` (el PCB simulado) encapsula todo el estado necesario para un proceso:

| Campo | Tipo | Propósito |
| :--- | :--- | :--- |
| `id` | `int` | Identificador único del proceso. |
| `state` | `enum` | Estado actual: `NEW`, `READY`, `RUNNING`, `WAITING`, `TERMINATED`. |
| `burst` | `int` | Tiempo total de CPU requerido inicialmente. |
| `remaining_time` | `int` | Tiempo de CPU restante. |
| `time_on_cpu` | `int` | Contador de ticks en el estado `RUNNING` (para Round Robin). |
| **Métricas** | `int` | `arrival_time`, `wait_time`, `turnaround_time`. |
| **Memoria** | `TablaDePaginas` | La estructura que mapea páginas virtuales a marcos físicos. |

## 4. Flujo de Control (Creación y Ejecución)

El flujo se rige por el reloj del sistema (`current_time`) manejado por el comando `run n` o `tick`:

1.  **CLI:** El usuario ingresa `new <burst>`.
2.  **`KernelSimulator`:** Crea una instancia de `Process` (PCB) y lo pasa al **Scheduler** a través de `Scheduler::add_process()`. El estado es `READY`.
3.  **Ciclo de Reloj (`tick`):**
    * Si hay un proceso `RUNNING`, se decrementa `remaining_time` y se incrementa `time_on_cpu`.
    * Si no hay proceso `RUNNING`, el **Scheduler** llama a **`dispatch()`** para seleccionar el siguiente proceso (`READY` → `RUNNING`) según el algoritmo activo (RR o SJF).
    * El ciclo continúa hasta que se agota el `quantum` (RR) o `remaining_time` llega a cero (TERMINATED).