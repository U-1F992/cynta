#include <cynta.h>

#if (0 < CYNTA_GLOBAL_POOL_REPEAT_CAPACITY)

#include <string.h>

static cynta_parser_error_t repeat_apply(cynta_parser_t *base,
                                         cynta_stream_t *stream, void *out) {
    cynta_repeat_t *self = (cynta_repeat_t *)base;
    if (self == NULL || stream == NULL || out == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    size_t total_size = 0;
    cynta_parser_error_t err;

    for (size_t i = 0; i < self->count; i++) {
        err = cynta_parser_apply(self->parser, stream,
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

cynta_parser_error_t cynta_repeat_init(cynta_repeat_t *self,
                                       cynta_parser_t *parser, size_t count) {
    if (self == NULL || parser == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    self->base.apply = repeat_apply;
    self->parser = parser;
    self->count = count;

    return CYNTA_PARSER_SUCCESS;
}

#endif