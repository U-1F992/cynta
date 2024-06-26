#include <cynta.h>

#include <stdlib.h>

#define PUSH(stack, value) ((stack)[(stack##_size)++] = (value))
#define POP(stack) ((stack)[--(stack##_size)])

cynta_stream_error_t cynta_stream_begin_transaction(
    cynta_stream_t *stream,
    cynta_transaction_status_t (*transaction)(cynta_stream_t *, void *),
    void *other_data) {
    if (stream == NULL || transaction == NULL) {
        return CYNTA_STREAM_ERROR_NULL_POINTER;
    }

    if (stream->checkpoints_size >=
        CYNTA_STREAM_BACKTRACK_CHECKPOINTS_CAPACITY) {
        return CYNTA_STREAM_ERROR_OUT_OF_CAPACITY;
    }
    PUSH(stream->checkpoints, stream->history_index);

    if (transaction(stream, other_data) /* == CYNTA_TRANSACTION_COMMIT */) {
        // Discard the checkpoint.
        if (stream->checkpoints_size == 0) {
            // assert(0);
            return CYNTA_STREAM_ERROR_NO_CHECKPOINT;
        }
        POP(stream->checkpoints);
    } else /* CYNTA_TRANSACTION_ROLLBACK */ {
        // Rewind to the checkpoint.
        if (stream->checkpoints_size == 0) {
            // assert(0);
            return CYNTA_STREAM_ERROR_NO_CHECKPOINT;
        }
        stream->history_index = POP(stream->checkpoints);
    }

    return CYNTA_STREAM_SUCCESS;
}

cynta_stream_error_t cynta_stream_next(cynta_stream_t *stream, uint8_t *out) {
    if (stream == NULL || out == NULL) {
        return CYNTA_STREAM_ERROR_NULL_POINTER;
    }

    if (stream->history_index < stream->history_size) {
        *out = stream->history[stream->history_index++];
    } else {
        cynta_stream_error_t err = stream->next(stream, out);
        if (err != CYNTA_STREAM_SUCCESS) {
            return err;
        }

        // Still need history
        if (stream->checkpoints_size != 0) {
            if (stream->history_size >=
                CYNTA_STREAM_BACKTRACK_HISTORY_CAPACITY) {
                return CYNTA_STREAM_ERROR_OUT_OF_CAPACITY;
            }

            PUSH(stream->history, *out);
            stream->history_index++;
        }
    }

    return CYNTA_STREAM_SUCCESS;
}

cynta_stream_error_t cynta_stream_init(cynta_stream_t *stream) {
    if (stream == NULL) {
        return CYNTA_STREAM_ERROR_NULL_POINTER;
    }

    stream->history_size = 0;
    stream->history_index = 0;

    stream->checkpoints_size = 0;

    return CYNTA_STREAM_SUCCESS;
}