# Cena de los Filósofos

**Ubicación:** `kernel/philosophers.h`, `kernel/philosophers.cpp`

## 1. Problema Clásico

Cinco filósofos sentados en una mesa circular con un tenedor entre cada par. Para comer, necesitan DOS tenedores (izquierdo y derecho). Riesgo de **deadlock** si todos toman tenedor izquierdo simultáneamente.

## 2. Solución Implementada

**Prevención de deadlock:** Límite de 4 filósofos en mesa (semáforo `room_limit`).

### Algoritmo

1. **Esperar entrada a mesa** (`room_limit.wait`)
2. **Tomar tenedor izquierdo** (`forks[left].wait`)
3. **Tomar tenedor derecho** (`forks[right].wait`)
4. **Comer** (incrementar contador)
5. **Soltar tenedores** (`signal` en ambos)
6. **Salir de mesa** (`room_limit.signal`)

Si falla cualquier `wait`, el filósofo libera recursos ya adquiridos (prevención de bloqueo indefinido).

## 3. Estados

- **THINKING:** No necesita recursos
- **HUNGRY:** Intenta adquirir tenedores
- **EATING:** Tiene ambos tenedores

## 4. Comandos CLI
```bash
phil start           # Inicia simulación
phil status          # Estado de filósofos
phil eat <0-4>       # Filósofo intenta comer
phil think <0-4>     # Filósofo piensa
phil stop            # Detiene simulación
```

## 5. Métricas

- `times_eaten`: Veces que comió
- `times_blocked`: Veces bloqueado por recursos

## 6. Invariantes

- Máximo 4 filósofos en mesa simultáneamente
- Un tenedor solo puede ser usado por 1 filósofo
- No hay deadlock (garantizado por `room_limit`)
- Sin inanición (orden FIFO en semáforos)

## 7. Ejemplo de Ejecución
```bash
phil start
phil eat 0    # Éxito
phil eat 1    # Éxito
phil eat 2    # Éxito  
phil eat 3    # Éxito
phil eat 4    # BLOQUEADO (mesa llena)
phil status   # Ver estado
phil think 0  # Libera recursos
phil eat 4    # Ahora puede entrar
```