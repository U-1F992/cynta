#ifndef TEST_STREAM_H
#define TEST_STREAM_T

#include <cynta.h>

typedef struct array_stream_t
{
    cynta_stream_t base;

    uint8_t *origin;
    size_t origin_size;
    size_t origin_index;
} array_stream_t;

cynta_stream_error_t array_stream_next(cynta_stream_t *base, uint8_t *out)
{
    array_stream_t *stream = (array_stream_t *)base;
    if (stream == NULL)
    {
        return CYNTA_STREAM_ERROR_NULL_POINTER;
    }
    if (stream->origin_index >= stream->origin_size)
    {
        return CYNTA_STREAM_ERROR_END_OF_STREAM;
    }
    *out = stream->origin[stream->origin_index++];
    return CYNTA_STREAM_SUCCESS;
}

cynta_stream_error_t array_stream_init(array_stream_t *stream, uint8_t origin[], size_t origin_size)
{
    if (stream == NULL)
    {
        return CYNTA_STREAM_ERROR_NULL_POINTER;
    }
    stream->base.next = array_stream_next;
    stream->origin = origin;
    stream->origin_size = origin_size;
    stream->origin_index = 0;
    return cynta_stream_init(&(stream->base));
}

#endif /* TEST_STREAM_T */