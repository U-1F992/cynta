#include <cynta/parser/sequence.h>
#include <cynta/uint8_array.h>

#include <string.h>

cynta_sequence_parser_t __cynta_sequence_pool[CYNTA_SEQUENCE_POOL_CAPACITY];
size_t __cynta_sequence_pool_index = 0;

static cynta_parser_error_t sequence_apply(cynta_parser_t *base, cynta_stream_t *stream, void *out)
{
    cynta_sequence_parser_t *parser = (cynta_sequence_parser_t *)base;
    if (parser == NULL ||
        stream == NULL ||
        out == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    cynta_uint8_array_t out0;
    cynta_parser_error_t err = cynta_parser_apply(parser->parsers[0], stream, (void *)&out0);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        return err;
    }

    cynta_uint8_array_t out1;
    err = cynta_parser_apply(parser->parsers[1], stream, (void *)&out1);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        return err;
    }

    memcpy((*(cynta_uint8_array_t *)out).data, out0.data, out0.size);
    memcpy((*(cynta_uint8_array_t *)out).data + out0.size, out1.data, out1.size);
    (*(cynta_uint8_array_t *)out).size = out0.size + out1.size;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_error_t cynta_sequence_init(cynta_sequence_parser_t *parser, cynta_parser_t *p0, cynta_parser_t *p1)
{
    if (parser == NULL ||
        p0 == NULL ||
        p1 == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    parser->base.apply = sequence_apply;
    parser->parsers[0] = p0;
    parser->parsers[1] = p1;

    return CYNTA_PARSER_SUCCESS;
}