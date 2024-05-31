#include <cynta.h>

#if (0 < CYNTA_SEQUENCE_POOL_CAPACITY)

#include <string.h>

static cynta_parser_error_t sequence_apply(cynta_parser_t *base,
                                           cynta_stream_t *stream, void *out) {
    cynta_sequence_t *self = (cynta_sequence_t *)base;
    if (self == NULL || stream == NULL || out == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    size_t total_size = 0;
    cynta_parser_error_t err;

    for (size_t i = 0; i < self->parsers_size; i++) {
        err = cynta_parser_apply(self->parsers[i], stream,
                                 (void *)&(self->incoming_buffer));
        if (err != CYNTA_PARSER_SUCCESS) {
            return err;
        }
        if (total_size + self->incoming_buffer.size >
            CYNTA_UINT8_ARRAY_CAPACITY) {
            return CYNTA_PARSER_ERROR_OUT_OF_CAPACITY;
        }
        memcpy((*(cynta_uint8_array_t *)out).data + total_size,
               self->incoming_buffer.data, self->incoming_buffer.size);
        total_size += self->incoming_buffer.size;
    }

    (*(cynta_uint8_array_t *)out).size = total_size;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_error_t cynta_internal_sequence_init(cynta_sequence_t *self,
                                                  size_t size, va_list args) {
    static cynta_parser_vtable_t vtbl = {.apply = sequence_apply};

    if (self == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    if (size < 1 || CYNTA_SEQUENCE_ARGS_CAPACITY < size) {
        return CYNTA_PARSER_ERROR_OUT_OF_CAPACITY;
    }

    self->base.vtbl = &vtbl;
    for (size_t i = 0; i < size; i++) {
        if ((self->parsers[i] = va_arg(args, cynta_parser_t *)) == NULL) {
            return CYNTA_PARSER_ERROR_NULL_POINTER;
        }
    }
    self->parsers_size = size;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_error_t cynta_sequence_init(cynta_sequence_t *self, size_t size,
                                         ...) {
    va_list args;
    va_start(args, size);
    cynta_parser_error_t ret = cynta_internal_sequence_init(self, size, args);
    va_end(args);
    return ret;
}

#endif