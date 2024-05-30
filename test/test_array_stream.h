#ifndef TEST_ARRAY_STREAM_H
#define TEST_ARRAY_STREAM_H

#include <cynta.h>

typedef struct test_array_stream_t
{
    cynta_stream_t base;

    uint8_t *origin;
    size_t origin_size;
    size_t origin_index;
} test_array_stream_t;

cynta_stream_error_t test_array_stream_next(cynta_stream_t *base, uint8_t *out)
{
    test_array_stream_t *stream = (test_array_stream_t *)base;
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

cynta_stream_error_t test_array_stream_init(test_array_stream_t *stream, uint8_t origin[], size_t origin_size)
{
    if (stream == NULL)
    {
        return CYNTA_STREAM_ERROR_NULL_POINTER;
    }
    stream->base.next = test_array_stream_next;
    stream->origin = origin;
    stream->origin_size = origin_size;
    stream->origin_index = 0;
    return cynta_stream_init(&(stream->base));
}

#endif /* TEST_ARRAY_STREAM_H */