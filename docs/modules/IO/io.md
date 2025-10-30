#  Sincronizaci�n y Comunicaci�n (M�dulo IO)

**Ubicaci�n:** `modules/io/io_manager.h`, `modules/io/io_manager.cpp` (o similar)

## 1. Mecanismos de Sincronizaci�n

La simulaci�n implementa los mecanismos de sincronizaci�n utilizando primitivas de bajo nivel (sem�foros o mutex simulados) para proteger recursos compartidos y coordinar procesos.

## 2. Problema Can�nico: Productor-Consumidor (P/C)

El problema de Productor-Consumidor se modela sobre un **buffer compartido** simulado por el n�cleo, al que solo puede acceder el proceso que actualmente est� en estado `RUNNING`.

### 2.1. Dise�o y Sem�foros

El P/C utiliza tres (3) sem�foros/mutex conceptuales para garantizar el acceso seguro y evitar desbordamientos/subdesbordamientos:

1.  **Mutex (`Mutex`):** Un mutex de exclusi�n mutua para garantizar que solo un proceso a la vez acceda al *buffer* (protege las variables compartidas).
2.  **`Empty` (Sem�foro Contador):** Cuenta el n�mero de slots vac�os en el *buffer*. Permite a los Productores operar si `Empty > 0`.
3.  **`Full` (Sem�foro Contador):** Cuenta el n�mero de items ocupados en el *buffer*. Permite a los Consumidores operar si `Full > 0`.

### 2.2. Flujo de Comandos

| Comando | Acci�n del Proceso RUNNING | L�gica de Sincronizaci�n |
| :--- | :--- | :--- |
| `produce` | Intenta a�adir un item al *buffer*. | Espera en `Empty` (P), adquiere `Mutex`, libera `Full` (V). |
| `consume` | Intenta remover un item del *buffer*. | Espera en `Full` (P), adquiere `Mutex`, libera `Empty` (V). |
| `sync_stat` | Muestra el estado actual del *buffer* (items y tama�o). | Muestra los valores de `Empty` y `Full`. |

### 2.3. Invariantes

La correcci�n del m�dulo de sincronizaci�n se mantiene mediante los siguientes invariantes:

* **Integridad del Buffer:** `N�mero de Items en Buffer + N�mero de Slots Vac�os = Capacidad Total del Buffer`.
* **Exclusi�n Mutua:** En cualquier momento, solo un hilo (el Productor o el Consumidor) puede estar manipulando las variables de �ndice del *buffer* (`in` o `out`).
* **No Subdesbordamiento:** Un Consumidor solo puede proceder si `Full > 0` (es decir, si hay items disponibles).