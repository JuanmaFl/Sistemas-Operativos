# kernel-sim/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(kernel-sim VERSION 0.1.0 LANGUAGES CXX)

# Establecer el estándar de C++ a 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Directorios de Inclusión
# Permite que el código en cualquier módulo se incluya usando rutas relativas (ej. #include <kernel/core.h>)
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# ===================================================================
# 1. Definir Módulos (Bibliotecas Estáticas)
# Cada módulo se compila como una librería estática (.a)
# ===================================================================

# Módulo KERNEL
add_library(kernel_lib STATIC
    kernel/core.cpp
    kernel/scheduler.cpp
    kernel/syscall_handler.cpp
)

# Módulo CPU
add_library(cpu_lib STATIC
    modules/cpu/processor.cpp
    modules/cpu/context_switch.cpp
)

# Módulo MEM (Memoria Virtual)
add_library(mem_lib STATIC
    modules/mem/memory_manager.cpp
    modules/mem/page_table.cpp
    modules/mem/swap.cpp
)

# Módulo IO
add_library(io_lib STATIC
    modules/io/io_scheduler.cpp
    modules/io/device_driver.cpp
)

# Módulo DISK
add_library(disk_lib STATIC
    modules/disk/filesystem.cpp
    modules/disk/block_manager.cpp
)

# Agrupar todas las librerías en una variable
set(SIMULATOR_LIBS
    kernel_lib
    cpu_lib
    mem_lib
    io_lib
    disk_lib
)

# ===================================================================
# 2. Crear Ejecutable Principal (CLI)
# ===================================================================

# El ejecutable principal se llama 'kernel-sim'
add_executable(kernel-sim
    cli/main.cpp
    cli/commands.cpp
    cli/utils.cpp
)

# Enlazar el ejecutable con todos los módulos de la simulación
target_link_libraries(kernel-sim PRIVATE
    ${SIMULATOR_LIBS}
)

# ===================================================================
# 3. Configuración de Pruebas (Google Test)
# ===================================================================

# Buscar el paquete GTest (ahora instalado via pacman/MSYS2)
find_package(GTest CONFIG REQUIRED)

if (GTest_FOUND)
    # Agregar los directorios donde buscar archivos de prueba
    include_directories(tests)
    
    # IMPORTANTE: Incluye el módulo GoogleTest para habilitar gtest_discover_tests
    include(GoogleTest)
    
    # Crear un ejecutable para las pruebas unitarias
    add_executable(kernel_tests
        tests/test_integration.cpp
        # modules/cpu/tests/test_cpu.cpp  <-- Añadir los tests unitarios aquí cuando existan
    )
    
    # Enlazar los tests con los módulos de la simulación y GTest
    target_link_libraries(kernel_tests PRIVATE 
        ${SIMULATOR_LIBS} 
        GTest::gtest_main
    )

    # Habilitar la detección automática de tests por CTest
    include(CTest)
    gtest_discover_tests(kernel_tests)
else()
    message(WARNING "Google Test no encontrado. No se compilarán las pruebas.")
endif()