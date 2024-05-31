#include <cynta.h>

#if (0 < CYNTA_GLOBAL_POOL_MANY_CAPACITY)

#include <string.h>

cynta_many_t __cynta_global_pool_many[CYNTA_GLOBAL_POOL_MANY_CAPACITY];
size_t __cynta_global_pool_many_index = 0;

static cynta_parser_error_t many_apply(cynta_parser_t *base,
                                       cynta_stream_t *stream, void *out) {
    cynta_many_t *self = (cynta_many_t *)base;
    if (self == NULL || stream == NULL || out == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    size_t total_size = 0;
    cynta_parser_error_t err;

    while (1) {
        cynta_stream_push_checkpoint(stream);
        err = cynta_parser_apply(self->parser, stream,
                                 (void *)&(self->incoming_buffer));
        switch (err) {
        case CYNTA_PARSER_SUCCESS:
            cynta_stream_discard_checkpoint(stream);
            break;

        case CYNTA_PARSER_ERROR_END_OF_STREAM:
            cynta_stream_discard_checkpoint(stream);
            goto commit;

        case CYNTA_PARSER_ERROR_UNEXPECTED_VALUE:
            cynta_stream_rewind(stream);
            goto commit;

        default:
            cynta_stream_discard_checkpoint(stream);
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

commit:
    (*(cynta_uint8_array_t *)out).size = total_size;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_error_t cynta_many_init(cynta_many_t *self, cynta_parser_t *p) {
    if (self == NULL || p == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    self->base.apply = many_apply;
    self->parser = p;

    return CYNTA_PARSER_SUCCESS;
}

#endif