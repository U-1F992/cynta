#include <cynta.h>

#if (0 < CYNTA_CHOICE_POOL_CAPACITY)

typedef struct cynta_choice_transaction_data_t {
    cynta_parser_t *parser;
    void *out;
    cynta_parser_error_t out_err;
} cynta_choice_transaction_data_t;

static cynta_transaction_status_t choice_transaction(cynta_stream_t *stream,
                                                     void *other_data) {
    assert(stream != NULL);
    assert(other_data != NULL);

    cynta_choice_transaction_data_t *data =
        (cynta_choice_transaction_data_t *)other_data;

    data->out_err = cynta_parser_apply(data->parser, stream, data->out);
    if (data->out_err == CYNTA_PARSER_SUCCESS) {
        return CYNTA_TRANSACTION_COMMIT;
    } else {
        return CYNTA_TRANSACTION_ROLLBACK;
    }
}

static cynta_parser_error_t choice_apply(cynta_parser_t *base,
                                         cynta_stream_t *stream, void *out) {
    cynta_choice_t *self = (cynta_choice_t *)base;
    if (self == NULL || stream == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    cynta_parser_error_t err =
        CYNTA_PARSER_ERROR_UNEXPECTED_VALUE; // Default error if no parser
                                             // succeeds

    for (size_t i = 0; i < self->parsers_size; i++) {
        cynta_choice_transaction_data_t data = {.parser = self->parsers[i],
                                                .out = out};
        cynta_stream_error_t stream_err =
            cynta_stream_begin_transaction(stream, choice_transaction, &data);
        if (stream_err != CYNTA_STREAM_SUCCESS) {
            return CYNTA_PARSER_ERROR_INTERNAL;
        }

        if (data.out_err == CYNTA_PARSER_SUCCESS) {
            return CYNTA_PARSER_SUCCESS;
        } else {
            err = data.out_err;
        }
    }

    return err; // Return the last error encountered
}

cynta_parser_error_t cynta_internal_choice_init(cynta_choice_t *self,
                                                size_t size, va_list args) {
    static cynta_parser_vtable_t vtbl = {.apply = choice_apply};

    if (self == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    if (size < 1 || CYNTA_CHOICE_ARGS_CAPACITY < size) {
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

cynta_parser_error_t cynta_choice_init(cynta_choice_t *self, size_t size, ...) {
    va_list args;
    va_start(args, size);
    cynta_parser_error_t ret = cynta_internal_choice_init(self, size, args);
    va_end(args);
    return ret;
}

#endif