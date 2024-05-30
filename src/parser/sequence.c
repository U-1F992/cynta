#include <cynta.h>

#if 0 < CYNTA_SEQUENCE_POOL_CAPACITY

#include <string.h>

cynta_sequence_t __cynta_sequence_pool[CYNTA_SEQUENCE_POOL_CAPACITY];
size_t __cynta_sequence_pool_index = 0;

static cynta_parser_error_t sequence_apply(cynta_parser_t *base, cynta_stream_t *stream, void *out)
{
    cynta_sequence_t *self = (cynta_sequence_t *)base;
    if (self == NULL ||
        stream == NULL ||
        out == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    size_t total_size = 0;
    cynta_parser_error_t err;

    for (size_t i = 0; i < 2; i++)
    {
        err = cynta_parser_apply(self->parsers[i], stream, (void *)&(self->incoming_buffer));
        if (err != CYNTA_PARSER_SUCCESS)
        {
            return err;
        }
        if (total_size + self->incoming_buffer.size >= CYNTA_UINT8_ARRAY_CAPACITY)
        {
            return CYNTA_PARSER_ERROR_OUT_OF_CAPACITY;
        }
        memcpy((*(cynta_uint8_array_t *)out).data + total_size, self->incoming_buffer.data, self->incoming_buffer.size);
        total_size += self->incoming_buffer.size;
    }

    (*(cynta_uint8_array_t *)out).size = total_size;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_error_t cynta_sequence_init(cynta_sequence_t *self, cynta_parser_t *p0, cynta_parser_t *p1)
{
    if (self == NULL ||
        p0 == NULL ||
        p1 == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    self->base.apply = sequence_apply;
    self->parsers[0] = p0;
    self->parsers[1] = p1;

    return CYNTA_PARSER_SUCCESS;
}

#endif