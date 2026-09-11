# ⚡ Embedded C Lock-Free Circular Ring Buffer

[![C/C++ CI](https://github.com/Sherifred123/embedded-c-ring-buffer/actions/workflows/ci.yml/badge.svg)](https://github.com/Sherifred123/embedded-c-ring-buffer/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

A lightweight, deterministic, and lock-free **FIFO Circular Ring Buffer** written in C99, optimized for resource-constrained microcontrollers (**Microchip PIC18**, **STM32 ARM Cortex-M**, **ESP32**).

Designed specifically for **Single-Producer Single-Consumer (SPSC)** architectures, such as high-speed UART/SPI Interrupt Service Routines (ISRs) streaming data into a background processing loop without dropping bytes or requiring mutex locks.

---

## 📐 Architecture & Memory Layout

```text
               Tail (Read Index)         Head (Write Index)
                     │                         │
                     ▼                         ▼
┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
│ 0 │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 10│ 11│ 12│ 13│ 14│ 15│  (Capacity = 16)
└───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
                    ▲───────────────────────▲
                        Stored Data Bytes
```

### Key Engineering Features:
- **Lock-Free Concurrency:** Single-Producer Single-Consumer (SPSC) safe without disabling interrupts or using OS mutexes.
- **Fast Power-of-2 Bitwise Wrapping:** Uses `(head + 1) & (capacity - 1)` bitmasking instead of slow `%` modulo division hardware instructions.
- **Zero Dynamic Memory Allocation:** No `malloc` / `free` — safe for mission-critical industrial standards.
- **Watermark Telemetry:** Integrated `peak_usage` tracking to monitor real-time buffer saturation during lab debugging.
- **Buffer Flush & Offset Peek:** Non-destructive lookahead and selective packet discarding.

---

## 🚀 Quick Start Example

```c
#include "ring_buffer.h"
#include <stdio.h>

#define UART_RX_BUF_SIZE 128
static uint8_t raw_buffer[UART_RX_BUF_SIZE];
static ring_buffer_t uart_rb;

void main(void) {
    /* Initialize ring buffer */
    ring_buffer_init(&uart_rb, raw_buffer, UART_RX_BUF_SIZE);

    /* Push byte from UART RX Interrupt */
    ring_buffer_push(&uart_rb, 0x55);

    /* Read byte from Main Loop */
    uint8_t rx_data;
    if (ring_buffer_pop(&uart_rb, &rx_data) == RING_BUFFER_OK) {
        printf("Received byte: 0x%02X\n", rx_data);
    }
}
```

---

## 🧪 Running Unit Tests

Run the test harness with any standard GCC compiler:

```bash
make
```

### Expected Output:
```text
=======================================================
 🚀 C Embedded Ring Buffer -- Unit Test Suite
=======================================================

[TEST] Initialization & Power-of-2 Validation... PASSED [OK]
[TEST] Single Push & Pop Operations... PASSED [OK]
[TEST] Flush & Multi-Offset Peek Operations... PASSED [OK]
[TEST] Wrap-Around & Modulo Masking... PASSED [OK]
[TEST] Overflow Boundary & Peak Telemetry... PASSED [OK]
[TEST] Block Read / Write Batch Transfers... PASSED [OK]

=======================================================
 🎉 ALL UNIT TESTS PASSED SUCCESSFULLY! (6/6)
=======================================================
```

---

## 📜 License
Released under the [MIT License](LICENSE). Designed and maintained by **Sherifred Singh**.
