/**
 * @file test_main.c
 * @brief Comprehensive Unit Test Suite for Ring Buffer.
 * @author Sherifred Singh
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "ring_buffer.h"

#define BUFFER_CAPACITY 64
static uint8_t raw_memory[BUFFER_CAPACITY];
static ring_buffer_t rb;

void test_initialization(void) {
    printf("[TEST] Initialization & Power-of-2 Validation... ");
    assert(ring_buffer_init(NULL, raw_memory, BUFFER_CAPACITY) == RING_BUFFER_ERROR_NULL);
    assert(ring_buffer_init(&rb, raw_memory, 50) == RING_BUFFER_ERROR_PARAM);
    assert(ring_buffer_init(&rb, raw_memory, BUFFER_CAPACITY) == RING_BUFFER_OK);
    assert(ring_buffer_is_empty(&rb) == true);
    assert(ring_buffer_is_full(&rb) == false);
    assert(ring_buffer_get_count(&rb) == 0);
    assert(ring_buffer_get_free(&rb) == (BUFFER_CAPACITY - 1));
    printf("PASSED [OK]\n");
}

void test_push_pop_single(void) {
    printf("[TEST] Single Push & Pop Operations... ");
    ring_buffer_reset(&rb);

    uint8_t tx_val = 0xA5;
    uint8_t rx_val = 0x00;

    assert(ring_buffer_push(&rb, tx_val) == RING_BUFFER_OK);
    assert(ring_buffer_get_count(&rb) == 1);
    assert(ring_buffer_is_empty(&rb) == false);

    assert(ring_buffer_peek(&rb, &rx_val) == RING_BUFFER_OK);
    assert(rx_val == 0xA5);
    assert(ring_buffer_get_count(&rb) == 1);

    assert(ring_buffer_pop(&rb, &rx_val) == RING_BUFFER_OK);
    assert(rx_val == 0xA5);
    assert(ring_buffer_get_count(&rb) == 0);
    assert(ring_buffer_is_empty(&rb) == true);

    assert(ring_buffer_pop(&rb, &rx_val) == RING_BUFFER_ERROR_EMPTY);
    printf("PASSED [OK]\n");
}

void test_flush_and_peek_offset(void) {
    printf("[TEST] Flush & Multi-Offset Peek Operations... ");
    ring_buffer_reset(&rb);

    for (uint8_t i = 0; i < 10; i++) {
        assert(ring_buffer_push(&rb, i + 10) == RING_BUFFER_OK);
    }
    assert(ring_buffer_get_count(&rb) == 10);

    uint8_t val = 0;
    assert(ring_buffer_peek_at(&rb, 4, &val) == RING_BUFFER_OK);
    assert(val == 14); /* 5th element */

    /* Flush 5 bytes */
    assert(ring_buffer_flush(&rb, 5) == RING_BUFFER_OK);
    assert(ring_buffer_get_count(&rb) == 5);
    assert(ring_buffer_peek(&rb, &val) == RING_BUFFER_OK);
    assert(val == 15);

    printf("PASSED [OK]\n");
}

void test_wrap_around(void) {
    printf("[TEST] Wrap-Around & Modulo Masking... ");
    ring_buffer_reset(&rb);

    for (int cycle = 0; cycle < 500; cycle++) {
        for (uint8_t i = 0; i < 30; i++) {
            assert(ring_buffer_push(&rb, i) == RING_BUFFER_OK);
        }
        for (uint8_t i = 0; i < 30; i++) {
            uint8_t val;
            assert(ring_buffer_pop(&rb, &val) == RING_BUFFER_OK);
            assert(val == i);
        }
    }
    assert(ring_buffer_is_empty(&rb) == true);
    printf("PASSED [OK]\n");
}

void test_overflow_protection(void) {
    printf("[TEST] Overflow Boundary & Peak Telemetry... ");
    ring_buffer_reset(&rb);

    for (size_t i = 0; i < (BUFFER_CAPACITY - 1); i++) {
        assert(ring_buffer_push(&rb, (uint8_t)i) == RING_BUFFER_OK);
    }

    assert(ring_buffer_is_full(&rb) == true);
    assert(ring_buffer_get_free(&rb) == 0);
    assert(ring_buffer_push(&rb, 0xFF) == RING_BUFFER_ERROR_FULL);
    assert(ring_buffer_get_peak(&rb) == (BUFFER_CAPACITY - 1));

    printf("PASSED [OK]\n");
}

void test_block_transfer(void) {
    printf("[TEST] Block Read / Write Batch Transfers... ");
    ring_buffer_reset(&rb);

    const uint8_t test_msg[] = "INDUSTRIAL_FIRMWARE_UART_TEST_PACKET";
    size_t msg_len = strlen((const char *)test_msg);
    size_t bytes_written = 0;
    size_t bytes_read = 0;
    uint8_t rx_buffer[64] = {0};

    assert(ring_buffer_write_block(&rb, test_msg, msg_len, &bytes_written) == RING_BUFFER_OK);
    assert(bytes_written == msg_len);
    assert(ring_buffer_get_count(&rb) == msg_len);

    assert(ring_buffer_read_block(&rb, rx_buffer, msg_len, &bytes_read) == RING_BUFFER_OK);
    assert(bytes_read == msg_len);
    assert(memcmp(test_msg, rx_buffer, msg_len) == 0);
    assert(ring_buffer_is_empty(&rb) == true);

    printf("PASSED [OK]\n");
}

int main(void) {
    printf("\n=======================================================\n");
    printf(" 🚀 C Embedded Ring Buffer -- Unit Test Suite\n");
    printf("=======================================================\n\n");

    test_initialization();
    test_push_pop_single();
    test_flush_and_peek_offset();
    test_wrap_around();
    test_overflow_protection();
    test_block_transfer();

    printf("\n=======================================================\n");
    printf(" 🎉 ALL UNIT TESTS PASSED SUCCESSFULLY! (6/6)\n");
    printf("=======================================================\n\n");

    return 0;
}
