text

# Carl Kernel Architecture Diagram

## Boot Sequence (Secuencia de Inicio)

```
┌─────────────────────────────────────────────────────────────┐
│ 1. bootloader.asm (Real Mode 16-bit)                        │
│    - Inicializa memoria básica                              │
│    - Carga kernel en memoria                                │
│    - Salta a modo protegido                                 │
└─────────────────────────────────────────────────────────────┘
                             ↓
┌─────────────────────────────────────────────────────────────┐
│ 2. Protected Mode Entry                                      │
│    - Habilita A20 line                                       │
│    - Carga GDT temporal                                      │
│    - Habilita modo protegido (CR0 PE bit)                   │
│    - Salta a start_kernel()                                 │
└─────────────────────────────────────────────────────────────┘
                             ↓
┌─────────────────────────────────────────────────────────────┐
│ 3. start_kernel() en main.c                                 │
└─────────────────────────────────────────────────────────────┘
    ├─→ vga_init()           [Inicializa pantalla]
    ├─→ init_gdt()           [Configura segmentación]
    ├─→ init_idt()           [Configura interrupciones]
    ├─→ init_timer()         [Configura reloj del sistema]
    ├─→ init_keyboard()      [Configura entrada]
    ├─→ init_interrupts()    [Habilita interrupciones]
    └─→ kernel_shell()       [Shell interactivo]
```

## Memory Layout (Distribución de Memoria)

```
4 GB    ┌──────────────────┐
        │  Kernel Space    │
        │  (Ring 0)        │
        │                  │
        │  ┌────────────┐  │
        │  │   Kernel  │  │
        │  │   Code    │  │
        │  └────────────┘  │
        │                  │
        │  ┌────────────┐  │
        │  │   Kernel  │  │
        │  │   Data    │  │
        │  └────────────┘  │
        │                  │
        │  ┌────────────┐  │
        │  │   GDT/IDT │  │
        │  └────────────┘  │
        │                  │
      3 GB┝─────────────────┤
        │                  │
        │  User Space      │
        │  (Ring 3)        │
        │                  │
      0 GB┴──────────────────┴
        
    Special Addresses:
    - 0x00000000: IVT (Interrupt Vector Table) - Real Mode
    - 0x0000B800: Video Memory (0xB8000) - 80x25 text mode
    - 0x00100000: Kernel Load Address (1 MB)
```

## Interrupt/Exception Vectors

```
Vector #  | Type           | Handler
────────────────────────────────────────
    0     | Divide by Zero | isr0
    1     | Debug          | isr1
    2     | NMI            | isr2
    3     | Breakpoint     | isr3
    4     | Overflow       | isr4
    5     | Bound Range    | isr5
    6     | Invalid Opcode | isr6
    7     | Device N/A     | isr7
    8     | Double Fault   | isr8
   ...    | ...            | ...
   13     | GPF            | isr13
   14     | Page Fault     | isr14
   32     | Timer (IRQ0)   | timer_handler
   33     | Keyboard (IRQ1)| keyboard_handler
   ...    | ...            | ...
```

## Hardware I/O Ports

```
PORT    | DEVICE                  | USAGE
────────────────────────────────────────────────
0x40-42 | Programmable Interval   | Timer control
        | Timer (PIT 8254)        |
────────────────────────────────────────────────
0x43    | PIT Control Register    | Mode/divisor
────────────────────────────────────────────────
0x60    
