#include <cynta.h>

#if (0 < CYNTA_MANY_POOL_CAPACITY)

#include <string.h>

typedef enum cynta_many_transaction_post_action_t {
    CYNTA_MANY_TRANSACTION_BREAK,
    CYNTA_MANY_TRANSACTION_GOTO_COMMIT,
    CYNTA_MANY_TRANSACTION_RETURN_ERROR
} cynta_many_transaction_post_action_t;

typedef struct cynta_many_transaction_data_t {
    cynta_many_t *self;
    cynta_many_transaction_post_action_t out_post_action;
    cynta_parser_error_t out_err;
} cynta_many_transaction_data_t;

static cynta_transaction_status_t many_transaction(cynta_stream_t *stream,
                                                   void *other_data) {
    assert(stream != NULL);
    assert(other_data != NULL);

    cynta_many_transaction_data_t *data =
        (cynta_many_transaction_data_t *)other_data;

    cynta_parser_error_t err = cynta_parser_apply(
        data->self->parser, stream, (void *)&(data->self->incoming_buffer));
    switch (err) {
    case CYNTA_PARSER_SUCCESS:
        data->out_post_action = CYNTA_MANY_TRANSACTION_BREAK;
        return CYNTA_TRANSACTION_COMMIT;

    case CYNTA_PARSER_ERROR_END_OF_STREAM:
        data->out_post_action = CYNTA_MANY_TRANSACTION_GOTO_COMMIT;
        return CYNTA_TRANSACTION_COMMIT;

    case CYNTA_PARSER_ERROR_UNEXPECTED_VALUE:
        data->out_post_action = CYNTA_MANY_TRANSACTION_GOTO_COMMIT;
        return CYNTA_TRANSACTION_ROLLBACK;

    default:
        data->out_post_action = CYNTA_MANY_TRANSACTION_RETURN_ERROR;
        data->out_err = err;
        return CYNTA_TRANSACTION_COMMIT;
    }
}

static cynta_parser_error_t many_apply(cynta_parser_t *base,
                                       cynta_stream_t *stream, void *out) {
    cynta_many_t *self = (cynta_many_t *)base;
    if (self == NULL || stream == NULL || out == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    size_t total_size = 0;
    cynta_parser_error_t err;

    while (1) {
        cynta_many_transaction_data_t data = {.self = self};
        cynta_stream_error_t stream_err =
            cynta_stream_begin_transaction(stream, many_transaction, &data);
        if (stream_err != CYNTA_STREAM_SUCCESS) {
            return CYNTA_PARSER_ERROR_INTERNAL;
        }

        switch (data.out_post_action) {
        case CYNTA_MANY_TRANSACTION_BREAK:
            break;

        case CYNTA_MANY_TRANSACTION_GOTO_COMMIT:
            goto commit;

        case CYNTA_MANY_TRANSACTION_RETURN_ERROR:
        default:
            return data.out_err;
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

cynta_parser_error_t cynta_many_init(cynta_many_t *self,
                                     cynta_parser_t *parser) {
    static cynta_parser_vtable_t vtbl = {.apply = many_apply};

    if (self == NULL || parser == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    self->base.vtbl = &vtbl;
    self->parser = parser;

    return CYNTA_PARSER_SUCCESS;
}

#endif