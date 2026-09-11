/**
 * @file ring_buffer.c
 * @brief Implementation of lock-free circular ring buffer.
 * @author Sherifred Singh
 */

#include "ring_buffer.h"
#include <string.h>

static inline bool is_power_of_two(size_t x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

ring_buffer_status_t ring_buffer_init(ring_buffer_t *rb, uint8_t *memory_block, size_t capacity) {
    if (rb == NULL || memory_block == NULL) {
        return RING_BUFFER_ERROR_NULL;
    }

    if (!is_power_of_two(capacity) || capacity < 2) {
        return RING_BUFFER_ERROR_PARAM;
    }

    rb->buffer = memory_block;
    rb->capacity = capacity;
    rb->mask = capacity - 1;
    rb->head = 0;
    rb->tail = 0;
    rb->peak_usage = 0;

    return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_reset(ring_buffer_t *rb) {
    if (rb == NULL) {
        return RING_BUFFER_ERROR_NULL;
    }

    rb->head = 0;
    rb->tail = 0;
    rb->peak_usage = 0;

    return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_flush(ring_buffer_t *rb, size_t count) {
    if (rb == NULL) {
        return RING_BUFFER_ERROR_NULL;
    }

    size_t available = ring_buffer_get_count(rb);
    size_t to_flush = (count > available) ? available : count;

    rb->tail = (rb->tail + to_flush) & rb->mask;
    return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_push(ring_buffer_t *rb, uint8_t data) {
    if (rb == NULL || rb->buffer == NULL) {
        return RING_BUFFER_ERROR_NULL;
    }

    size_t current_head = rb->head;
    size_t current_tail = rb->tail;

    size_t count = (current_head - current_tail) & rb->mask;

    if (count == (rb->capacity - 1)) {
        return RING_BUFFER_ERROR_FULL;
    }

    rb->buffer[current_head] = data;
    rb->head = (current_head + 1) & rb->mask;

    size_t new_count = count + 1;
    if (new_count > rb->peak_usage) {
        rb->peak_usage = new_count;
    }

    return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_pop(ring_buffer_t *rb, uint8_t *data) {
    if (rb == NULL || rb->buffer == NULL || data == NULL) {
        return RING_BUFFER_ERROR_NULL;
    }

    size_t current_head = rb->head;
    size_t current_tail = rb->tail;

    if (current_head == current_tail) {
        return RING_BUFFER_ERROR_EMPTY;
    }

    *data = rb->buffer[current_tail];
    rb->tail = (current_tail + 1) & rb->mask;

    return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_peek(const ring_buffer_t *rb, uint8_t *data) {
    return ring_buffer_peek_at(rb, 0, data);
}

ring_buffer_status_t ring_buffer_peek_at(const ring_buffer_t *rb, size_t offset, uint8_t *data) {
    if (rb == NULL || rb->buffer == NULL || data == NULL) {
        return RING_BUFFER_ERROR_NULL;
    }

    size_t count = ring_buffer_get_count(rb);
    if (offset >= count) {
        return RING_BUFFER_ERROR_EMPTY;
    }

    size_t peek_index = (rb->tail + offset) & rb->mask;
    *data = rb->buffer[peek_index];
    return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_write_block(ring_buffer_t *rb, const uint8_t *src, size_t length, size_t *bytes_written) {
    if (rb == NULL || src == NULL || bytes_written == NULL) {
        return RING_BUFFER_ERROR_NULL;
    }

    *bytes_written = 0;
    for (size_t i = 0; i < length; i++) {
        if (ring_buffer_push(rb, src[i]) != RING_BUFFER_OK) {
            break;
        }
        (*bytes_written)++;
    }

    return (*bytes_written > 0) ? RING_BUFFER_OK : RING_BUFFER_ERROR_FULL;
}

ring_buffer_status_t ring_buffer_read_block(ring_buffer_t *rb, uint8_t *dst, size_t length, size_t *bytes_read) {
    if (rb == NULL || dst == NULL || bytes_read == NULL) {
        return RING_BUFFER_ERROR_NULL;
    }

    *bytes_read = 0;
    for (size_t i = 0; i < length; i++) {
        if (ring_buffer_pop(rb, &dst[i]) != RING_BUFFER_OK) {
            break;
        }
        (*bytes_read)++;
    }

    return (*bytes_read > 0) ? RING_BUFFER_OK : RING_BUFFER_ERROR_EMPTY;
}

size_t ring_buffer_get_count(const ring_buffer_t *rb) {
    if (rb == NULL) return 0;
    return (rb->head - rb->tail) & rb->mask;
}

size_t ring_buffer_get_free(const ring_buffer_t *rb) {
    if (rb == NULL) return 0;
    return (rb->capacity - 1) - ring_buffer_get_count(rb);
}

bool ring_buffer_is_empty(const ring_buffer_t *rb) {
    if (rb == NULL) return true;
    return (rb->head == rb->tail);
}

bool ring_buffer_is_full(const ring_buffer_t *rb) {
    if (rb == NULL) return false;
    return ring_buffer_get_count(rb) == (rb->capacity - 1);
}

size_t ring_buffer_get_peak(const ring_buffer_t *rb) {
    if (rb == NULL) return 0;
    return rb->peak_usage;
}
