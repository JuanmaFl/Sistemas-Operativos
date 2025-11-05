#  Planificación de Disco

**Ubicación:** `modules/disk/disk_scheduler.h`, `modules/disk/disk_scheduler.cpp`

## 1. Diseño del Planificador

El módulo simula un disco con **200 cilindros** (0-199) y un cabezal que se mueve para atender solicitudes de acceso. El sistema soporta tres algoritmos de planificación configurable en tiempo de ejecución.

### Componentes Clave

* **DiskRequest:** Estructura que encapsula una solicitud (PID, cilindro, tiempos)
* **Cola de Solicitudes:** Mantiene peticiones pendientes
* **Cabezal:** Posición actual (0-199)
* **Métricas:** Movimiento total, promedio por solicitud

## 2. Algoritmos Implementados

### 2.1. FCFS (First-Come, First-Served)

**Lógica:** Atiende solicitudes en el orden de llegada estricto.

**Ventajas:**
- Justo: no hay inanición
- Simple de implementar

**Desventajas:**
- Alto movimiento del cabezal
- No optimiza el tiempo de búsqueda

**Uso:** Cargas ligeras o cuando la equidad es crítica.

### 2.2. SSTF (Shortest Seek Time First)

**Lógica:** Selecciona el cilindro más cercano a la posición actual del cabezal.

**Ventajas:**
- Minimiza el movimiento total
- Mejor throughput que FCFS

**Desventajas:**
- Puede causar inanición (starvation) en cilindros lejanos
- Comportamiento greedy

**Uso:** Sistemas con alta carga donde el rendimiento es prioritario.

### 2.3. SCAN (Elevator Algorithm)

**Lógica:** El cabezal se mueve en una dirección hasta el final del disco, luego invierte y regresa, atendiendo solicitudes en el camino.

**Ventajas:**
- Previene inanición
- Predecible
- Balance entre FCFS y SSTF

**Desventajas:**
- Mayor latencia que SSTF en algunos casos
- Solicitudes en extremos pueden esperar más

**Uso:** Sistemas de propósito general con carga media-alta.

## 3. Comandos CLI

```bash
# Crear solicitud de disco
dreq <cilindro>              # Usa proceso RUNNING
dreq <cilindro> <pid>        # Especifica PID

# Procesar siguiente solicitud
dproc

# Visualización
dstat                        # Estado actual y cola
disk_stats                   # Métricas acumuladas
dview                        # Vista gráfica del disco

# Cambiar algoritmo
set_disk_algo <fcfs|sstf|scan>
```

## 4. Análisis Comparativo

### Secuencia de Prueba

**Solicitudes:** 98, 183, 37, 122, 14, 124, 65, 67  
**Posición inicial:** 53

### Resultados

| Algoritmo | Secuencia Atendida | Movimiento Total | Promedio |
|-----------|-------------------|------------------|----------|
| **FCFS**  | 98→183→37→122→14→124→65→67 | 640 cilindros | 80.0 |
| **SSTF**  | 65→67→37→14→98→122→124→183 | 236 cilindros | 29.5 |
| **SCAN**  | 65→67→98→122→124→183→199→37→14 | ~299 cilindros | 37.4 |

### Gráfico (ASCII)

```
Movimiento del Cabezal por Algoritmo
Cilindros
  700 |
  600 | █
  500 | █
  400 | █
  300 | █          █
  200 | █    █     █
  100 | █    █     █
    0 |_____|_____|_____
       FCFS  SSTF  SCAN
```

### Conclusiones

1. **SSTF** es el más eficiente en movimiento total (63% mejor que FCFS)
2. **SCAN** ofrece balance entre rendimiento y equidad
3. **FCFS** garantiza orden pero es ineficiente con patrones dispersos

**Recomendación:** 
- Usar **SSTF** en sistemas de alto rendimiento con vigilancia de inanición
- Usar **SCAN** en sistemas de propósito general
- Usar **FCFS** solo cuando la simplicidad o equidad estricta sean críticas

## 5. Implementación Técnica

### Estructura de Datos

```cpp
struct DiskRequest {
    int pid;
    int cylinder;
    int arrival_time;
    int completion_time;
};

class DiskScheduler {
    std::deque<DiskRequest> request_queue;
    int current_head_position;
    int direction;  // Para SCAN: ±1
    int total_head_movement;
};
```

### Flujo de Operación

1. **Llegada:** `add_request()` encola la solicitud
2. **Selección:** `process_next_request()` llama al algoritmo activo
3. **Ejecución:** Mueve el cabezal, actualiza métricas
4. **Registro:** Guarda en `completed_requests`

### Visualización

La función `visualize_disk()` muestra:
- Línea de cilindros (0-199)
- Posición del cabezal (▼)
- Solicitudes pendientes (*)

Ejemplo:
```
Cilindros: 0     20    40    60    80   100   120   140   160   180   199
           |---------------------------------------------------------|
Cabezal:   |                        ▼                                |  [53]
Solicitudes:|              *    *         *   *   *           *     *|
```

## 6. Pruebas y Validación

Ver `scripts/disk_requests.txt` para secuencias de prueba reproducibles.

**Invariantes verificados:**
- El cabezal nunca sale del rango [0, 199]
- Todas las solicitudes se atienden eventualmente
- Movimiento calculado = suma de distancias absolutas