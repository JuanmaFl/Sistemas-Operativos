#  Gestión de Memoria Virtual y Paginación

**Ubicación:** `modules/mem/memory_manager.h`, `modules/mem/memory_manager.cpp`

## 1. Diseño de la Paginación

La simulación implementa un esquema de **Memoria Virtual** basado en **Paginación** simple.

* **Página Virtual:** Una unidad lógica de memoria dentro del espacio de direcciones de un proceso.
* **Marco Físico (Frame):** Una unidad de memoria física disponible para la asignación.
* **Tabla de Páginas:** Mantenida por proceso (dentro del PCB). Almacena el mapeo de las páginas virtuales a los marcos físicos.
* **Fallos de Página (Page Faults):** Ocurren cuando un proceso accede a una página que no está actualmente cargada en un marco físico. Esto desencadena el proceso de reemplazo.

## 2. Algoritmos de Reemplazo Implementados

El módulo soporta la configuración en tiempo de ejecución de dos algoritmos de reemplazo de páginas, utilizando el comando `set_page_algo <fifo|lru>`.

### 2.1. FIFO (First-In, First-Out)

**Lógica:** Desaloja la página que lleva más tiempo en la memoria (el primer marco que fue asignado), independientemente de su uso reciente.
**Implementación:** Se utiliza una cola o una lista ligada donde los marcos se insertan al final y se remueven del frente.
**Métrica:** Tiende a mostrar un rendimiento pobre si los patrones de acceso demuestran localidad.

### 2.2. LRU (Least Recently Used)

**Lógica:** Desaloja la página que fue utilizada hace más tiempo, bajo la suposición de que lo menos usado en el pasado será lo menos usado en el futuro.
**Implementación:** Se utiliza una lista donde las páginas accedidas son movidas al frente (más reciente) y la página a desalojar es la del final (menos reciente).

## 3. Métricas y Pruebas

Para el análisis de rendimiento, se registran las siguientes métricas:

* **Total de Fallos de Página:** Número total de veces que ocurrió un fallo durante la traza de accesos.
* **Tasa de Aciertos (Hit Rate):** Porcentaje de accesos a memoria que encuentran la página en un marco.
* **Análisis de la Trama de Referencia:** Se usa una secuencia de acceso predefinida (ver `scripts/mem_benchmarks.txt`) para generar datos para la gráfica.

## 4. Análisis Comparativo de Rendimiento (FIFO vs. LRU)

Para validar la eficiencia de los algoritmos de reemplazo, se ejecutó una traza de acceso a páginas con diferentes tamaños de memoria física (Marcos) y se contaron los Fallos de Página resultantes.

### Trama de Referencia Utilizada

La traza de prueba utilizada fue: `1, 2, 3, 1, 2, 4, 5, 1, 3, 6, 4, 2` (12 accesos).

### Gráfica de Rendimiento

A continuación, se presenta el gráfico comparativo:

![Gráfico Fallos de Página vs. Tamaño de Marcos (FIFO vs. LRU)]

### Conclusiones del Análisis

El gráfico de "Fallos de Página vs. Tamaño de Marcos" permite extraer las siguientes conclusiones:

1.  **Rendimiento Superior de LRU:** El algoritmo **LRU** (Least Recently Used) demuestra un rendimiento superior a **FIFO** (First-In, First-Out) para la traza de referencia utilizada. Esto se observa claramente en que la curva de LRU se mantiene consistentemente por debajo de la de FIFO a partir de los 3 marcos.
2.  **Uso Eficiente de la Localidad:** LRU, al priorizar la permanencia de las páginas usadas recientemente, hace un uso más eficiente de la **localidad temporal** inherente a la traza, resultando en 8 fallos para 3 marcos, comparado con los 9 fallos de FIFO.
3.  **Punto de Estabilidad (LRU):** LRU alcanza su límite de fallos (6 fallos) con solo **4 marcos**. Esto sugiere que 4 marcos son suficientes para contener el *Working Set* (conjunto de trabajo) mínimo de la traza, mientras que FIFO continúa experimentando fallos debido a su falta de conciencia sobre la frecuencia de uso.

**Conclusión General:** LRU es el algoritmo preferido en la simulación debido a su capacidad para minimizar el tiempo de espera asociado a las operaciones de E/S causadas por los fallos de página.