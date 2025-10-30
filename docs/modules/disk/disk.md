# Planificaci�n de Disco y E/S (M�dulo DISK)

**Ubicaci�n:** `modules/disk/disk_controller.h`, `modules/disk/disk_controller.cpp`

## 1. Dise�o del Subsistema de Disco

La simulaci�n modela un cabezal de disco y una cola de peticiones de E/S.

* **Cabezal de Disco:** Su posici�n est� definida por el `cilindro_actual`.
* **Petici�n de Disco:** Una solicitud para acceder a un n�mero de cilindro espec�fico.
* **M�trica:** El rendimiento se mide por el **Recorrido Total del Cabezal** (suma de todos los movimientos entre cilindros).

## 2. Algoritmos de Planificaci�n Implementados

### 2.1. FCFS (First-Come, First-Served)

**L�gica:** Procesa las peticiones en el estricto orden en que llegaron a la cola.
**M�trica:** F�cil de implementar, pero es el menos eficiente en t�rminos de movimiento del cabezal.

### 2.2. SSTF (Shortest Seek Time First)

**L�gica:** El cabezal se mueve al cilindro m�s cercano en la cola de peticiones actual, minimizando el tiempo de b�squeda inmediato.
**Desventaja:** Puede causar inanici�n (starvation) a peticiones ubicadas en los extremos del disco.

## 3. An�lisis Comparativo

**Gr�fico de An�lisis (Para el Informe Final):**

> 
> *Se espera que SSTF ofrezca un ahorro significativo en el movimiento total del cabezal en comparaci�n con FCFS, a costa de una posible inequidad en el tiempo de respuesta.*