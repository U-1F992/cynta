#include <cynta.h>

#if (0 < CYNTA_GLOBAL_POOL_SATISFY_CAPACITY + CYNTA_GLOBAL_POOL_ANY_CAPACITY + \
             CYNTA_GLOBAL_POOL_VALUE_CAPACITY)

static cynta_parser_error_t satisfy_apply(cynta_parser_t *base,
                                          cynta_stream_t *stream, void *out) {
    cynta_satisfy_t *self = (cynta_satisfy_t *)base;
    if (self == NULL || stream == NULL || out == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    uint8_t incoming;
    cynta_stream_error_t err = cynta_stream_next(stream, &incoming);
    switch (err) {
    case CYNTA_STREAM_SUCCESS:
        break;

    case CYNTA_STREAM_ERROR_END_OF_STREAM:
        return CYNTA_PARSER_ERROR_END_OF_STREAM;

    default:
        return CYNTA_PARSER_ERROR_INTERNAL;
    }

    if (!self->condition(self, incoming)) {
        return CYNTA_PARSER_ERROR_UNEXPECTED_VALUE;
    }

    ((cynta_uint8_array_t *)out)->data[0] = incoming;
    ((cynta_uint8_array_t *)out)->size = 1;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_error_t cynta_satisfy_init(cynta_satisfy_t *self,
                                        bool (*condition)(cynta_satisfy_t *,
                                                          uint8_t)) {
    static cynta_parser_vtable_t vtbl = {.apply = satisfy_apply};

    if (self == NULL || condition == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    self->base.vtbl = &vtbl;
    self->condition = condition;

    return CYNTA_PARSER_SUCCESS;
}

#endif