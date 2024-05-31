#include <cynta.h>

#if (0 < CYNTA_GLOBAL_POOL_TRY_CAPACITY)

cynta_try_t __cynta_global_pool_try[CYNTA_GLOBAL_POOL_TRY_CAPACITY];
size_t __cynta_global_pool_try_index = 0;

static cynta_parser_error_t try_apply(cynta_parser_t *base,
                                      cynta_stream_t *stream, void *out) {
    cynta_try_t *self = (cynta_try_t *)base;
    if (self == NULL || stream == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    cynta_stream_push_checkpoint(stream);
    cynta_parser_error_t err = cynta_parser_apply(self->parser, stream, out);
    if (err != CYNTA_PARSER_SUCCESS) {
        cynta_stream_rewind(stream);
    } else {
        cynta_stream_discard_checkpoint(stream);
    }

    return err;
}

cynta_parser_error_t cynta_try_init(cynta_try_t *self, cynta_parser_t *parser) {
    if (self == NULL || parser == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    self->base.apply = try_apply;
    self->parser = parser;

    return CYNTA_PARSER_SUCCESS;
}

#endif