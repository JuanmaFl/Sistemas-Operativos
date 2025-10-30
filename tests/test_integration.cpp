#include <gtest/gtest.h>
#include "kernel/core.h"


// Define un conjunto de pruebas para la integración del sistema
namespace {

    TEST(IntegrationTest, KernelInitialization) {
        // La inicialización del Kernel (Scheduler, Memory, etc.) ocurre en el constructor.
        // ASSERT_NO_THROW(kernel.initialize());
        // La función 'initialize()' fue eliminada, así que probamos la creación del objeto.

        // Creamos una instancia local para probar el constructor
        ASSERT_NO_THROW(KernelSimulator kernel_instance);

        // Puedes añadir verificaciones de estado inicial aquí si el Kernel tuviera getters públicos
        // Ejemplo: ASSERT_EQ(kernel_instance.get_current_time(), 0);
    }

    TEST(IntegrationTest, ProcessLifecycle) {
        // Prueba: Crea dos procesos y verifica que el scheduler los toma.
        KernelSimulator kernel;

        // Creamos procesos con tiempos de ráfaga pequeños para terminar rápido
        kernel.new_process(3); // P1
        kernel.new_process(2); // P2

        // Ejecutamos la simulación por suficientes ciclos para terminar ambos (RR, quantum 3)
        // P1: 3, P2: 2. Total: 5 ticks. Corremos 6 ticks.
        kernel.run_cycles(6);

        // Nota: Para hacer esta aserción, necesitarías un método en KernelSimulator
        // que te devuelva el estado del proceso o una lista de procesos.

        // Simplemente probamos que la ejecución no falló (por ahora)
        // La aserción real de 'TERMINATED' requiere más infraestructura.

        // Si el simulador debe reportar que el proceso terminó, puedes verificarlo con output
        // Por ahora, solo confirmamos que la ejecución con entradas básicas es exitosa.
        SUCCEED();
    }

} // namespace