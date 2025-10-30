# Kernel-Sim: Simulador de N�cleo de Sistemas Operativos

**Proyecto Final: Sistemas Operativos**

**Integrantes:**
* Laura Sof�a Aceros
* Juan Manuel Fl�rez

**Fecha:** 30 de Octubre de 2025

---

## Resumen Ejecutivo

Kernel-Sim es una simulaci�n funcional de un n�cleo de sistema operativo simplificado, desarrollado en C++. El objetivo principal es modelar y analizar el comportamiento de algoritmos esenciales en la gesti�n de **procesos**, **memoria virtual** y **sincronizaci�n** bajo una interfaz de l�nea de comandos (CLI).

Este repositorio contiene el c�digo fuente, los scripts de prueba para la replicaci�n de experimentos y el informe t�cnico detallado.

## Mapa del Proyecto

| Secci�n | Contenido Principal | Estado (Hito Parcial) |
| :--- | :--- | :--- |
| **Arquitectura** | Dise�o modular y flujo de control. | Completado |
| **M�dulo CPU** | Planificaci�n de Procesos (RR y SJF). | Completado |
| **M�dulo MEMORY** | Gesti�n de Memoria Virtual (FIFO y LRU). | Completado |
| **M�dulo IO** | Sincronizaci�n (Productor-Consumidor). | Completado |
| **M�dulo DISK** | Planificaci�n de Disco (FCFS y SSTF/SCAN). | Pendiente |

---

## Interfaz de L�nea de Comandos (CLI)

El simulador se interact�a a trav�s del ejecutable `kernel-sim.exe`. Use el comando `help` dentro del *prompt* para ver la lista completa de opciones.

**Comandos Clave de la Demostraci�n:**
* **Planificaci�n:** `new <burst>`, `run <n>`, `set_algo <rr|sjf>`, `set_quantum <n>`.
* **Memoria:** `access <dir>`, `set_page_algo <fifo|lru>`, `memview`.
* **Sincronizaci�n:** `produce`, `consume`, `sync_stat`.