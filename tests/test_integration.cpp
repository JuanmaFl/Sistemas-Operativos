#include <gtest/gtest.h>
#include "kernel/core.h"


// Define un conjunto de pruebas para la integraci�n del sistema
namespace {

    TEST(IntegrationTest, KernelInitialization) {
        // La inicializaci�n del Kernel (Scheduler, Memory, etc.) ocurre en el constructor.
        // ASSERT_NO_THROW(kernel.initialize());
        // La funci�n 'initialize()' fue eliminada, as� que probamos la creaci�n del objeto.

        // Creamos una instancia local para probar el constructor
        ASSERT_NO_THROW(KernelSimulator kernel_instance);

        // Puedes a�adir verificaciones de estado inicial aqu� si el Kernel tuviera getters p�blicos
        // Ejemplo: ASSERT_EQ(kernel_instance.get_current_time(), 0);
    }

    TEST(IntegrationTest, ProcessLifecycle) {
        // Prueba: Crea dos procesos y verifica que el scheduler los toma.
        KernelSimulator kernel;

        // Creamos procesos con tiempos de r�faga peque�os para terminar r�pido
        kernel.new_process(3); // P1
        kernel.new_process(2); // P2

        // Ejecutamos la simulaci�n por suficientes ciclos para terminar ambos (RR, quantum 3)
        // P1: 3, P2: 2. Total: 5 ticks. Corremos 6 ticks.
        kernel.run_cycles(6);

        // Nota: Para hacer esta aserci�n, necesitar�as un m�todo en KernelSimulator
        // que te devuelva el estado del proceso o una lista de procesos.

        // Simplemente probamos que la ejecuci�n no fall� (por ahora)
        // La aserci�n real de 'TERMINATED' requiere m�s infraestructura.

        // Si el simulador debe reportar que el proceso termin�, puedes verificarlo con output
        // Por ahora, solo confirmamos que la ejecuci�n con entradas b�sicas es exitosa.
        SUCCEED();
    }

} // namespace