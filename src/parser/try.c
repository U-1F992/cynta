#include <cynta.h>

#if (0 < CYNTA_TRY_POOL_CAPACITY)

typedef struct cynta_try_transaction_data_t {
    cynta_try_t *self;
    void *out;
    cynta_parser_error_t out_err;
} cynta_try_transaction_data_t;

static cynta_transaction_status_t try_transaction(cynta_stream_t *stream,
                                                  void *other_data) {
    assert(stream != NULL);
    assert(other_data != NULL);

    cynta_try_transaction_data_t *data = (cynta_try_transaction_data_t *)data;

    data->out_err = cynta_parser_apply(data->self->parser, stream, data->out);
    if (data->out_err != CYNTA_PARSER_SUCCESS) {
        return CYNTA_TRANSACTION_ROLLBACK;
    } else {
        return CYNTA_TRANSACTION_COMMIT;
    }
}

static cynta_parser_error_t try_apply(cynta_parser_t *base,
                                      cynta_stream_t *stream, void *out) {
    cynta_try_t *self = (cynta_try_t *)base;
    if (self == NULL || stream == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    cynta_try_transaction_data_t data = {.self = self, .out = out};
    cynta_stream_error_t stream_err =
        cynta_stream_begin_transaction(stream, try_transaction, &data);
    if (stream_err != CYNTA_STREAM_SUCCESS) {
        return CYNTA_PARSER_ERROR_INTERNAL;
    }

    return data.out_err;
}

cynta_parser_error_t cynta_try_init(cynta_try_t *self, cynta_parser_t *parser) {
    static cynta_parser_vtable_t vtbl = {.apply = try_apply};

    if (self == NULL || parser == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    self->base.vtbl = &vtbl;
    self->parser = parser;

    return CYNTA_PARSER_SUCCESS;
}

#endif