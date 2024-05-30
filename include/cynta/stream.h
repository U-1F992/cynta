#ifndef CYNTA_STREAM_H
#define CYNTA_STREAM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

    typedef enum cynta_stream_error_t
    {
        CYNTA_STREAM_SUCCESS,

        CYNTA_STREAM_ERROR_NULL_POINTER,
        CYNTA_STREAM_ERROR_MEMORY_ALLOCATION,
        CYNTA_STREAM_ERROR_END_OF_STREAM,

        CYNTA_STREAM_ERROR_INTERNAL
    } cynta_stream_error_t;

#define CYNTA_STREAM_ERROR(err) ((err) == CYNTA_STREAM_SUCCESS                   ? "CYNTA_STREAM_SUCCESS"                 \
                                 : (err) == CYNTA_STREAM_ERROR_NULL_POINTER      ? "CYNTA_STREAM_ERROR_NULL_POINTER"      \
                                 : (err) == CYNTA_STREAM_ERROR_MEMORY_ALLOCATION ? "CYNTA_STREAM_ERROR_MEMORY_ALLOCATION" \
                                 : (err) == CYNTA_STREAM_ERROR_END_OF_STREAM     ? "CYNTA_STREAM_ERROR_END_OF_STREAM"     \
                                 : (err) == CYNTA_STREAM_ERROR_INTERNAL          ? "CYNTA_STREAM_ERROR_INTERNAL"          \
                                                                                 : (assert(0), "CYNTA_STREAM_ERROR_UNKNOWN"))

    typedef struct cynta_stream_t
    {
        cynta_stream_error_t (*next)(struct cynta_stream_t *, uint8_t *);

        uint8_t *history;
        size_t history_size;
        size_t history_index;
        size_t *checkpoints;
        size_t checkpoints_size;
    } cynta_stream_t;

    void cynta_stream_push_checkpoint(cynta_stream_t *);
    void cynta_stream_rewind(cynta_stream_t *);
    void cynta_stream_discard_checkpoint(cynta_stream_t *);
    cynta_stream_error_t cynta_stream_next(cynta_stream_t *, uint8_t *);
    cynta_stream_error_t cynta_stream_init(cynta_stream_t *);
    cynta_stream_error_t cynta_stream_deinit(cynta_stream_t *);

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_STREAM_H */