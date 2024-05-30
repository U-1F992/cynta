#include <cynta.h>

#if 0 < CYNTA_SATISFY_POOL_CAPACITY + CYNTA_ANY_POOL_CAPACITY + CYNTA_VALUE_POOL_CAPACITY

cynta_satisfy_t __cynta_satisfy_pool[CYNTA_SATISFY_POOL_CAPACITY];
size_t __cynta_satisfy_pool_index = 0;

static cynta_parser_error_t satisfy_apply(cynta_parser_t *base, cynta_stream_t *stream, void *out)
{
    cynta_satisfy_t *self = (cynta_satisfy_t *)base;
    if (self == NULL ||
        stream == NULL ||
        out == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    uint8_t incoming;
    cynta_stream_error_t err = cynta_stream_next(stream, &incoming);
    switch (err)
    {
    case CYNTA_STREAM_SUCCESS:
        break;

    case CYNTA_STREAM_ERROR_END_OF_STREAM:
        return CYNTA_PARSER_ERROR_END_OF_STREAM;

    default:
        return CYNTA_PARSER_ERROR_INTERNAL;
    }

    if (!self->condition(self, incoming))
    {
        return CYNTA_PARSER_ERROR_UNEXPECTED_VALUE;
    }

    ((cynta_uint8_array_t *)out)->data[0] = incoming;
    ((cynta_uint8_array_t *)out)->size = 1;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_error_t cynta_satisfy_init(cynta_satisfy_t *self, bool (*cond)(cynta_satisfy_t *, uint8_t))
{
    if (self == NULL ||
        cond == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    self->base.apply = satisfy_apply;
    self->condition = cond;

    return CYNTA_PARSER_SUCCESS;
}

#endif