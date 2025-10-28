// tests/test_integration.cpp
#include <gtest/gtest.h>
#include "kernel/core.h"
// Incluye otros headers que necesites probar

// Define un conjunto de pruebas para la integración del sistema
namespace {

    TEST(IntegrationTest, KernelInitialization) {
        // Prueba que el Kernel se inicializa sin fallar
        KernelSimulator kernel;

        // Si la inicialización interna falla, esta prueba debería fallar
        ASSERT_NO_THROW(kernel.initialize());
    }

    TEST(IntegrationTest, ProcessLifecycle) {
        // TODO: Implementar un test donde un proceso se crea, ejecuta y termina

        // Ejemplo:
        // KernelSimulator kernel;
        // Process p(1, 10); // Proceso con 10 ráfagas
        // kernel.submit_process(p);
        // kernel.run_simulation(10);
        // ASSERT_TRUE(kernel.is_process_terminated(1));
    }

} // namespace

// La macro RUN_ALL_TESTS es manejada por GTest::gtest_main en el CMakeLists.txt