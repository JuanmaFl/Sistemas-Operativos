# Planificación de Disco y E/S (Módulo DISK)

**Ubicación:** `modules/disk/disk_controller.h`, `modules/disk/disk_controller.cpp`

## 1. Diseño del Subsistema de Disco

La simulación modela un cabezal de disco y una cola de peticiones de E/S.

* **Cabezal de Disco:** Su posición está definida por el `cilindro_actual`.
* **Petición de Disco:** Una solicitud para acceder a un número de cilindro específico.
* **Métrica:** El rendimiento se mide por el **Recorrido Total del Cabezal** (suma de todos los movimientos entre cilindros).

## 2. Algoritmos de Planificación Implementados

### 2.1. FCFS (First-Come, First-Served)

**Lógica:** Procesa las peticiones en el estricto orden en que llegaron a la cola.
**Métrica:** Fácil de implementar, pero es el menos eficiente en términos de movimiento del cabezal.

### 2.2. SSTF (Shortest Seek Time First)

**Lógica:** El cabezal se mueve al cilindro más cercano en la cola de peticiones actual, minimizando el tiempo de búsqueda inmediato.
**Desventaja:** Puede causar inanición (starvation) a peticiones ubicadas en los extremos del disco.

## 3. Análisis Comparativo

**Gráfico de Análisis (Para el Informe Final):**

> 
> *Se espera que SSTF ofrezca un ahorro significativo en el movimiento total del cabezal en comparación con FCFS, a costa de una posible inequidad en el tiempo de respuesta.*