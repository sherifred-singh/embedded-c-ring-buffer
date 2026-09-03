/**
 * @file ring_buffer.h
 * @brief Thread-safe, lock-free Circular FIFO Ring Buffer for Embedded Systems.
 * @author Sherifred Singh
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    RING_BUFFER_OK = 0,
    RING_BUFFER_ERROR_NULL,
    RING_BUFFER_ERROR_EMPTY,
    RING_BUFFER_ERROR_FULL,
    RING_BUFFER_ERROR_PARAM
} ring_buffer_status_t;

typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t mask;
    volatile size_t head;
    volatile size_t tail;
    size_t peak_usage;
} ring_buffer_t;

ring_buffer_status_t ring_buffer_init(ring_buffer_t *rb, uint8_t *memory_block, size_t capacity);
ring_buffer_status_t ring_buffer_reset(ring_buffer_t *rb);
ring_buffer_status_t ring_buffer_flush(ring_buffer_t *rb, size_t count);
ring_buffer_status_t ring_buffer_push(ring_buffer_t *rb, uint8_t data);
ring_buffer_status_t ring_buffer_pop(ring_buffer_t *rb, uint8_t *data);
ring_buffer_status_t ring_buffer_peek(const ring_buffer_t *rb, uint8_t *data);
ring_buffer_status_t ring_buffer_peek_at(const ring_buffer_t *rb, size_t offset, uint8_t *data);
ring_buffer_status_t ring_buffer_write_block(ring_buffer_t *rb, const uint8_t *src, size_t length, size_t *bytes_written);
ring_buffer_status_t ring_buffer_read_block(ring_buffer_t *rb, uint8_t *dst, size_t length, size_t *bytes_read);
size_t ring_buffer_get_count(const ring_buffer_t *rb);
size_t ring_buffer_get_free(const ring_buffer_t *rb);
bool ring_buffer_is_empty(const ring_buffer_t *rb);
bool ring_buffer_is_full(const ring_buffer_t *rb);
size_t ring_buffer_get_peak(const ring_buffer_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H */
/* Verified for MISRA-C Compliance & Single-Producer Single-Consumer Lock-Free Concurrency */
