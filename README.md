# Simulador de Núcleo de Sistema Operativo (Kernel-Sim)

**Proyecto Final - Sistemas Operativos**  
**Universidad EAFIT**  
**Equipo:** Laura Sofía Aceros & Juan Manuel Flórez

Este proyecto es una simulación modular de los componentes clave de un sistema operativo simplificado, desarrollado en C++17. Implementa gestión de procesos, planificación de CPU, memoria virtual con paginación, planificación de disco, sincronización, detección de deadlock y sistema de protección.

---

## 1.Tabla de Contenidos

- [Stack Tecnológico](#stack-tecnológico)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Requisitos e Instalación](#requisitos-e-instalación)
- [Compilación y Ejecución](#compilación-y-ejecución)
- [Componentes Implementados](#componentes-implementados)
- [Uso del Simulador](#uso-del-simulador)
- [Scripts de Experimentación](#scripts-de-experimentación)
- [Resultados y Análisis](#resultados-y-análisis)
- [Documentación](#documentación)
- [Equipo](#equipo)

---

## Stack Tecnológico

| Componente | Versión/Tecnología |
|------------|-------------------|
| **Lenguaje** | C++17 |
| **Build System** | CMake 3.10+ |
| **Compilador** | MinGW-w64 (GCC) |
| **Testing** | Google Test (GTest) |
| **Plataforma** | Windows (MSYS2/UCRT64) |

---

##  Estructura del Proyecto

```
kernel-sim/
├── kernel/               # Núcleo del sistema
│   ├── core.cpp         # Simulador principal
│   ├── scheduler.cpp    # Planificador de CPU
│   ├── process.h        # PCB (Process Control Block)
│   ├── deadlock_detector.cpp  # Algoritmo del Banquero
│   ├── protection.cpp   # Sistema de protección (Rings)
│   └── syscall_handler.cpp    # Manejador de syscalls
│
├── modules/             # Módulos del sistema
│   ├── cpu/            # Planificación de procesos
│   ├── mem/            # Gestión de memoria virtual
│   │   ├── memory_manager.cpp
│   │   ├── page_table.cpp
│   │   └── buddy_allocator.cpp
│   ├── disk/           # Planificación de disco
│   │   └── disk_scheduler.cpp
│   └── io/             # Sistema de E/S
│       └── io_manager.cpp
│
├── cli/                # Interfaz de línea de comandos
│   ├── main.cpp
│   └── commands.cpp    # 40+ comandos
│
├── scripts/            # Scripts de experimentación
│   ├── demo_complete.txt
│   ├── mem_*.txt
│   ├── disk_*.txt
│   └── proc_*.txt
│
├── docs/               # Documentación
│   ├── diagrams/       # Diagramas Mermaid
│   └── modules/        # Documentación por módulo
│
└── tests/              # Pruebas unitarias
    └── test_*.cpp
```

---

## Requisitos e Instalación

### 1. Instalar MSYS2

Descarga e instala desde: https://www.msys2.org/

### 2. Instalar Herramientas de Desarrollo

Abre la terminal **MSYS2 UCRT64** y ejecuta:

```bash
# Actualizar base de datos de paquetes
pacman -Sy

# Instalar toolchain (g++, make)
pacman -S mingw-w64-ucrt-x86_64-toolchain

# Instalar CMake
pacman -S mingw-w64-ucrt-x86_64-cmake

# Instalar Google Test
pacman -S mingw-w64-ucrt-x86_64-gtest
```

---

##  Compilación y Ejecución

### 1. Clonar el Repositorio

```bash
git clone https://github.com/tu-usuario/kernel-sim.git
cd kernel-sim
```

### 2. Generar Archivos de Build

```bash
mkdir build
cd build
cmake ..
```

### 3. Compilar

```bash
mingw32-make
```

### 4. Ejecutar

```bash
# Modo interactivo
./kernel-sim.exe

#Ejecutar para correr todas las funciones dentro del simulador
sim> exec ../scripts/demo_complete.txt
```


---

##  Componentes Implementados

### 1. Gestión de Procesos
- ✅ Creación, suspensión, reanudación y terminación
- ✅ Estados: NEW, READY, RUNNING, WAITING, TERMINATED
- ✅ Process Control Block (PCB) completo

### 2. Planificación de CPU
- ✅ **Round Robin** (RR) con quantum configurable
- ✅ **Shortest Job First** (SJF) no expropiativo
- ✅ Métricas: Wait Time, Turnaround Time

### 3. Memoria Virtual
- ✅ Paginación bajo demanda (16 marcos físicos)
- ✅ **FIFO** (First In First Out)
- ✅ **LRU** (Least Recently Used)
- ✅ **PFF** (Page Fault Frequency) - Avanzado
- ✅ Visualización de marcos y estadísticas

### 4. Planificación de Disco
- ✅ **FCFS** (First Come First Served)
- ✅ **SSTF** (Shortest Seek Time First)
- ✅ **SCAN** (Elevator Algorithm)
- ✅ Visualización de cilindros y movimiento del cabezal

### 5. Sistema de E/S
- ✅ Cola de prioridades
- ✅ Simulación de dispositivos (PRINTER)
- ✅ Bloqueo/desbloqueo de procesos

### 6. Sincronización
- ✅ **Productor-Consumidor** con semáforos
- ✅ **Cena de los Filósofos** (prevención de deadlock)
- ✅ Mutex, Empty y Full semaphores

### 7. Detección de Deadlock
- ✅ **Algoritmo del Banquero**
- ✅ Verificación de estado seguro
- ✅ Estadísticas de seguridad

### 8. Sistema de Protección
- ✅ **Rings de Privilegio** (Ring 0 Kernel, Ring 3 User)
- ✅ Segmentación (CODE, DATA, STACK, KERNEL)
- ✅ **Syscalls** con elevación de privilegios
- ✅ Verificación de permisos (R/W/X)

### 9. Asignador de Memoria (Heap)
- ✅ **Buddy System Allocator**
- ✅ Medición de fragmentación
- ✅ Análisis de latencia

### 10. Interfaz CLI
- ✅ 40+ comandos interactivos
- ✅ Ejecución de scripts (.txt)
- ✅ Sistema de ayuda integrado

---

##  Uso del Simulador

### Comandos Principales

#### Gestión de Procesos
```bash
new 50          # Crear proceso con burst=50
ps              # Listar procesos
kill 1          # Terminar proceso PID 1
```

#### Planificación de CPU
```bash
set_algo rr     # Cambiar a Round Robin
set_quantum 3   # Establecer quantum=3
run 10          # Ejecutar 10 ciclos
stats           # Ver estadísticas
```

#### Memoria Virtual
```bash
set_page_algo lru    # Cambiar a LRU
access 2048 1        # Acceder dirección 2048 (PID 1)
memview              # Ver estado de marcos
```

#### Planificación de Disco
```bash
set_disk_algo sstf   # Cambiar a SSTF
dreq 98 1            # Solicitud a cilindro 98 (PID 1)
dproc                # Procesar solicitud
dview                # Visualizar disco
disk_stats           # Estadísticas
```

#### Sincronización
```bash
produce ItemA        # Producir recurso
consume              # Consumir recurso
sync_stat            # Estado del buffer

phil start           # Iniciar filósofos
phil eat 0           # Filósofo 0 intenta comer
phil status          # Ver estado
```

#### Deadlock
```bash
dlinit 0 3 2 1       # Inicializar recursos P0
dlreq 0 1 0 0        # Solicitar recursos
dldetect             # Detectar deadlock
dlstat               # Ver estado del banquero
```

#### Protección
```bash
protect 1 0x500 read    # Verificar acceso lectura
syscall 1 read file.txt # Invocar syscall
segments                # Ver tabla de segmentos
protstats               # Estadísticas de protección
```

#### Scripts
```bash
exec demo_complete.txt     # Ejecutar demo completo
exec scripts/mem_fifo.txt  # Experimento FIFO
help                       # Ver todos los comandos
```

---

## 📊 Scripts de Experimentación

### Memoria Virtual
- `mem_fifo.txt` - Experimento con FIFO
- `mem_lru.txt` - Experimento con LRU
- `mem_pff.txt` - Experimento con PFF (avanzado)

### Planificación de Disco
- `disk_fcfs.txt` - FCFS benchmark
- `disk_sstf.txt` - SSTF benchmark
- `disk_scan.txt` - SCAN benchmark

### Planificación de CPU
- `proc_rr.txt` - Round Robin tests
- `proc_sjf.txt` - SJF tests

### Demostración Completa
- `demo_complete.txt` - Prueba de TODAS las funcionalidades (120+ comandos)

**Ejecutar experimentos:**
```bash
./kernel-sim.exe
sim> exec ../scripts/demo_complete.txt
```

---

## Resultados y Análisis

### Memoria Virtual (31 accesos)

| Algoritmo | Fallos | Aciertos | Tasa Hit |
|-----------|--------|----------|----------|
| FIFO      | 15     | 16       | 51.6%    |
| LRU       | 12     | 19       | 61.3%    |
| PFF       | 14     | 17       | 54.8%    |

**Conclusión:** LRU ofrece mejor rendimiento aprovechando localidad temporal.

### Planificación de Disco (8 solicitudes desde cilindro 50)

| Algoritmo | Movimiento Total | Promedio por Solicitud |
|-----------|------------------|------------------------|
| FCFS      | 640 cilindros    | 80.0 cyl/req          |
| SSTF      | 236 cilindros    | 29.5 cyl/req          |
| SCAN      | 299 cilindros    | 37.4 cyl/req          |

**Conclusión:** SSTF reduce movimiento en 63% vs FCFS. SCAN ofrece mejor balance entre rendimiento y fairness.

### Planificación de CPU

| Algoritmo | Wait Time Promedio | Turnaround Promedio |
|-----------|--------------------|---------------------|
| RR (Q=2)  | 8.0 ciclos        | 14.5 ciclos         |
| SJF       | 5.0 ciclos        | 13.0 ciclos         |

**Conclusión:** SJF minimiza wait time pero puede causar inanición. RR garantiza fairness.

### Buddy System Allocator

| Tamaño Solicitado | Tamaño Asignado | Fragmentación |
|-------------------|-----------------|---------------|
| 100 bytes         | 128 bytes       | 28%           |
| 500 bytes         | 512 bytes       | 2.4%          |
| 1000 bytes        | 1024 bytes      | 2.4%          |

**Fragmentación Promedio:** 15.2%

### Sistema de Protección

| Métrica | Valor |
|---------|-------|
| Verificaciones Totales | 3 |
| Violaciones | 2 |
| Tasa de Violaciones | 66.67% |
| Syscalls Exitosas | 4/4 (100%) |

### Algoritmo del Banquero

| Métrica | Valor |
|---------|-------|
| Total Solicitudes | 3 |
| Estados Seguros | 2 |
| Estados Inseguros | 1 |
| Tasa de Seguridad | 66.67% |

---

## 📚 Documentación

### Estructura de Documentación

```
docs/
├── diagrams/                    # Diagramas Mermaid
│   ├── 01_arquitectura_general.mmd
│   ├── 02_flujo_procesos.mmd
│   ├── 03_memoria_virtual.mmd
│   ├── 04-12_*.mmd
│   └── README.md
│
└── modules/                     # Docs por módulo
    ├── cpu/
    │   └── scheduler.md
    ├── mem/
    │   ├── paging.md
    │   └── buddy.md
    ├── disk/
    │   └── algorithms.md
    ├── io/
    │   └── synchronization.md
    ├── deadlock/
    │   └── banker.md
    └── protection/
        └── rings.md
```

### Visualizar Diagramas

Los diagramas están en formato Mermaid (.mmd):

 **GitHub:** Renderiza automáticamente archivos .mmd

### Documentos Principales

- `README.md` - Este archivo
- `INFORME_TECNICO.pdf` - Informe completo del proyecto
- `docs/diagrams/README.md` - Guía de diagramas

---

##  Cumplimiento de Requisitos del Proyecto

| Requisito | Estado | Implementación |
|-----------|--------|----------------|
| **1. Gestión de Procesos** | ✅ 100% | Creación, suspensión, reanudación, terminación |
| **2. Planificación (RR + SJF)** | ✅ 100% | Round Robin y SJF implementados |
| **3. Memoria (FIFO + LRU + Avanzado)** | ✅ 100% | FIFO, LRU y PFF implementados |
| **4. Sincronización (P/C + Filósofos)** | ✅ 100% | Productor-Consumidor y Cena de Filósofos |
| **5. E/S (Cola prioridades)** | ✅ 100% | Sistema de E/S con prioridades |
| **6. Disco (FCFS + SSTF/SCAN)** | ✅ 100% | FCFS, SSTF y SCAN implementados |
| **7. CLI** | ✅ 100% | 40+ comandos funcionales |
| **8. Scripts de Experimentación** | ✅ 100% | Scripts completos para reproducir experimentos |
| **9. Informe Técnico** | ✅ 100% | Documentación completa con gráficos |
| **10. Diagramas** | ✅ 100% | 12 diagramas en Mermaid |
| **Extras (Bonus)** | ✅ | Deadlock (Banker), Protección (Rings), Buddy Allocator |

---

##  Equipo

**Universidad EAFIT**  
**Curso:** Sistemas Operativos  
**Profesor:** Diego Iván Cruz Ordiéres

### Desarrolladores

- **Laura Sofía Aceros**
  - Implementación de módulos de memoria y sincronización
  - Diseño de arquitectura del sistema
  
- **Juan Manuel Flórez**
  - Implementación de planificadores y protección
  - Desarrollo de CLI y scripts de experimentación

---

##  Licencia

Este proyecto es de carácter académico y fue desarrollado como proyecto final del curso de Sistemas Operativos de la Universidad EAFIT.

---

## 🔗Enlaces Útiles

- **Repositorio:** https://github.com/JuanmaFl/Sistemas-Operativos.git
- **Documentación Completa:** Ver carpeta `docs/`
- **Informe Técnico:** `INFORME_TECNICO.pdf`
- **Diagramas:** `docs/diagrams/`


---


**Kernel-Sim v1.0** - Proyecto Final Sistemas Operativos - Universidad EAFIT - 2025
