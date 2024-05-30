#include <cynta/parser/repeat.h>
#include <cynta/uint8_array.h>

#include <string.h>

cynta_repeat_parser_t __cynta_repeat_pool[CYNTA_REPEAT_POOL_CAPACITY];
size_t __cynta_repeat_pool_index = 0;

static cynta_parser_error_t repeat_apply(cynta_parser_t *base, cynta_stream_t *stream, void *out)
{
    cynta_repeat_parser_t *parser = (cynta_repeat_parser_t *)base;
    if (parser == NULL ||
        stream == NULL ||
        out == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    size_t total_size = 0;

    for (size_t i = 0; i < parser->count; i++)
    {
        cynta_uint8_array_t part_out;
        cynta_parser_error_t err = cynta_parser_apply(parser->parser, stream, (void *)&part_out);
        if (err != CYNTA_PARSER_SUCCESS)
        {
            return err;
        }

        if (total_size + part_out.size >= CYNTA_UINT8_ARRAY_CAPACITY)
        {
            return CYNTA_PARSER_ERROR_OUT_OF_CAPACITY;
        }

        memcpy((*(cynta_uint8_array_t *)out).data + total_size, part_out.data, part_out.size);
        total_size += part_out.size;
    }
    (*(cynta_uint8_array_t *)out).size = total_size;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_error_t cynta_repeat_init(cynta_repeat_parser_t *parser, cynta_parser_t *p, size_t n)
{
    if (parser == NULL ||
        p == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    parser->base.apply = repeat_apply;
    parser->parser = p;
    parser->count = n;

    return CYNTA_PARSER_SUCCESS;
}