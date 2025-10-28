# 💻 Simulador de Núcleo de Sistema Operativo (Kernel-Sim)

Este proyecto es una simulación modular de los componentes clave de un sistema operativo simplificado, desarrollado en C++17. Incluye módulos para la gestión de procesos, planificación (Round Robin, SJF), memoria virtual con paginación (FIFO, LRU), sincronización y E/S.

## ⚙️ Stack Tecnológico

* **Lenguaje:** C++17
* **Sistema de Build:** CMake (v3.10+)
* **Compilador:** MinGW-w64 (GCC)
* **Pruebas:** Google Test (GTest)

---

## 📂 Estructura del Proyecto

El código está organizado modularmente, reflejando las capas de un Kernel:

| Directorio | Propósito |
| :--- | :--- |
| `kernel/` | Lógica central del sistema (Core, Scheduler, Syscalls). |
| `modules/` | Contiene subsistemas principales (CPU, Memoria, I/O, Disco). |
| `cli/` | El punto de entrada de la aplicación y la interfaz de línea de comandos. |
| `tests/` | Pruebas de integración y unitarias (utiliza GTest). |
| `docs/` | Archivos de documentación y arquitectura. |

---

## 🛠️ Requisitos e Instalación

Para compilar y ejecutar el proyecto, se necesita un entorno de desarrollo C++ compatible con MSYS2/MinGW.

1.  **Instalar MSYS2:** Siga las instrucciones oficiales de MSYS2.
2.  **Instalar Herramientas de Desarrollo:** En la terminal **MSYS2 UCRT64** (Recomendado), ejecute los siguientes comandos para instalar el compilador, CMake y Google Test:

    ```bash
    # 1. Actualizar la base de datos de paquetes
    pacman -Sy
    
    # 2. Instalar el toolchain (g++, make)
    pacman -S mingw-w64-ucrt-x86_64-toolchain
    
    # 3. Instalar CMake (para generar archivos de build)
    pacman -S mingw-w64-ucrt-x86_64-cmake
    
    # 4. Instalar Google Test (para la suite de pruebas)
    pacman -S mingw-w64-ucrt-x86_64-gtest
    ```

---

## 🚀 Compilación y Ejecución

Siga estos pasos desde la terminal **MSYS2 UCRT64**.

### 1. Preparar la Compilación (CMake)

Navegue a la carpeta raíz del proyecto y use CMake para generar los archivos de *build* (`Makefiles`).

```bash
# Navegar a la carpeta del proyecto (ajuste la ruta según sea necesario)
cd /c/Users/jmflo/kernel-sim/

# Crear y entrar al directorio de build
mkdir build
cd build

# Ejecutar CMake para generar los Makefiles
cmake ..

### 2. Compilar el Código

Use el comando mingw32-make para compilar todos los módulos y el ejecutable principal.

Bash

# Compilar el proyecto completo
mingw32-make

### 3. Ejecutar el Simulador

Una vez compilado, el ejecutable kernel-sim.exe estará en el directorio build/.

Bash

# Ejecutar la CLI del simulador
./kernel-sim.exe 

# Ejemplo de uso (una vez implementados los comandos)
./kernel-sim.exe --load processes.txt --scheduler RR

4. Ejecutar las Pruebas
Puede usar CTest (incluido con CMake) para ejecutar automáticamente todas las pruebas de GTest definidas en el proyecto.

Bash

# Ejecutar la suite de pruebas
ctest
📚 Documentación
Consulte la carpeta docs/ para obtener una descripción más detallada de la arquitectura de cada módulo y los algoritmos implementados.