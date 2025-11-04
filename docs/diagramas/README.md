# Diagramas del Kernel Simulator

Esta carpeta contiene todos los diagramas del proyecto en formato Mermaid.

## Archivos

1. **01_arquitectura_general.mmd** - Arquitectura completa del sistema
2. **02_flujo_procesos.mmd** - Diagrama de estados de procesos
3. **03_memoria_virtual.mmd** - Flujo de acceso a memoria
4. **04_resultados_memoria.mmd** - Estadísticas de algoritmos de memoria
5. **05_disco_fcfs.mmd** - Planificación FCFS
6. **06_disco_sstf.mmd** - Planificación SSTF
7. **07_disco_scan.mmd** - Planificación SCAN
8. **08_sincronizacion_pc.mmd** - Productor-Consumidor
9. **09_filosofos.mmd** - Cena de los Filósofos
10. **10_banker_deadlock.mmd** - Algoritmo del Banquero
11. **11_proteccion_rings.mmd** - Sistema de protección
12. **12_estadisticas_finales.mmd** - Resumen de resultados

## Cómo Visualizar

### Opción 1: Mermaid Live Editor
1. Visita https://mermaid.live/
2. Copia y pega el contenido de cualquier archivo .mmd
3. Visualiza y exporta como PNG/SVG

### Opción 2: VS Code
1. Instala extensión "Markdown Preview Mermaid Support"
2. Abre archivo .mmd
3. Presiona Ctrl+Shift+V

### Opción 3: GitHub
Los archivos .mmd se renderizan automáticamente en GitHub

## Uso en Informe

Estos diagramas están listos para incluirse en el informe técnico.
Exporta como PNG (300 DPI) para documentos impresos.

---

# Estructura Final
```

kernel-sim/
├── docs/
│   ├── diagrams/                   
│   │   ├── README.md
│   │   ├── 01_arquitectura_general.mmd
│   │   ├── 02_flujo_procesos.mmd
│   │   ├── 03_memoria_virtual.mmd
│   │   ├── 04_resultados_memoria.mmd
│   │   ├── 05_disco_fcfs.mmd
│   │   ├── 06_disco_sstf.mmd
│   │   ├── 07_disco_scan.mmd
│   │   ├── 08_sincronizacion_pc.mmd
│   │   ├── 09_filosofos.mmd
│   │   ├── 10_banker_deadlock.mmd
│   │   ├── 11_proteccion_rings.mmd
│   │   └── 12_estadisticas_finales.mmd
│   └── modules/
├── scripts/
├── kernel/
└── ...
```
