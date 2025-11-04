# kernel-sim/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(kernel-sim VERSION 0.1.0 LANGUAGES CXX)

# Establecer el estándar de C++ a 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


if(MSVC)
    # Visual Studio
    add_compile_options(/utf-8)
else()
    # GCC/Clang/MinGW
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# Directorios de Inclusión
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# ===================================================================
# 1. Definir Módulos (Bibliotecas Estáticas)
# ===================================================================

# Módulo KERNEL
add_library(kernel_lib STATIC
    kernel/core.cpp
    kernel/deadlock_detector.cpp
    kernel/scheduler.cpp
    kernel/sync.cpp
    kernel/philosophers.cpp
    kernel/protection.cpp
    kernel/syscall_handler.cpp
)

# Módulo MEM (Memoria Virtual) - LIMPIADO
add_library(mem_lib STATIC
    modules/mem/memory_manager.cpp
    modules/mem/page_table.cpp
)

# Módulo IO - LIMPIADO
add_library(io_lib STATIC
    modules/io/io_manager.cpp
)

# Módulo DISK - LIMPIADO
add_library(disk_lib STATIC
    modules/disk/disk_scheduler.cpp
)

# Agrupar todas las librerías (CPU_LIB REMOVIDA)
set(SIMULATOR_LIBS
    kernel_lib
    mem_lib
    io_lib
    disk_lib
)

# ===================================================================
# 2. Crear Ejecutable Principal (CLI) - LIMPIADO
# ===================================================================

add_executable(kernel-sim
    cli/main.cpp
    cli/commands.cpp
)

# Enlazar el ejecutable con todos los módulos
target_link_libraries(kernel-sim PRIVATE
    ${SIMULATOR_LIBS}
)

# ===================================================================
# 3. Configuración de Pruebas (Google Test)
# ===================================================================

find_package(GTest CONFIG REQUIRED)

if (GTest_FOUND)
    include_directories(tests)
    include(GoogleTest)
    
    add_executable(kernel_tests
        tests/test_integration.cpp
    )
    
    target_link_libraries(kernel_tests PRIVATE 
        ${SIMULATOR_LIBS} 
        GTest::gtest_main
    )
    
    include(CTest)
    gtest_discover_tests(kernel_tests)
else()
    message(WARNING "Google Test no encontrado. No se compilarán las pruebas.")
endif()