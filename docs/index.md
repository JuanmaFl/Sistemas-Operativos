# Kernel-Sim: Simulador de Núcleo de Sistemas Operativos

**Proyecto Final: Sistemas Operativos**

**Integrantes:**
* Laura Sofía Aceros
* Juan Manuel Flórez

**Fecha:** 30 de Octubre de 2025

---

## Resumen Ejecutivo

Kernel-Sim es una simulación funcional de un núcleo de sistema operativo simplificado, desarrollado en C++. El objetivo principal es modelar y analizar el comportamiento de algoritmos esenciales en la gestión de **procesos**, **memoria virtual** y **sincronización** bajo una interfaz de línea de comandos (CLI).

Este repositorio contiene el código fuente, los scripts de prueba para la replicación de experimentos y el informe técnico detallado.

## Mapa del Proyecto

| Sección | Contenido Principal | Estado (Hito Parcial) |
| :--- | :--- | :--- |
| **Arquitectura** | Diseño modular y flujo de control. | Completado |
| **Módulo CPU** | Planificación de Procesos (RR y SJF). | Completado |
| **Módulo MEMORY** | Gestión de Memoria Virtual (FIFO y LRU). | Completado |
| **Módulo IO** | Sincronización (Productor-Consumidor). | Completado |
| **Módulo DISK** | Planificación de Disco (FCFS y SSTF/SCAN). | Pendiente |

---

## Interfaz de Línea de Comandos (CLI)

El simulador se interactúa a través del ejecutable `kernel-sim.exe`. Use el comando `help` dentro del *prompt* para ver la lista completa de opciones.

**Comandos Clave de la Demostración:**
* **Planificación:** `new <burst>`, `run <n>`, `set_algo <rr|sjf>`, `set_quantum <n>`.
* **Memoria:** `access <dir>`, `set_page_algo <fifo|lru>`, `memview`.
* **Sincronización:** `produce`, `consume`, `sync_stat`.