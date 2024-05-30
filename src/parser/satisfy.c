#include <cynta/parser/satisfy.h>
#include <cynta/uint8_array.h>

cynta_satisfy_parser_t __cynta_satisfy_pool[CYNTA_SATISFY_POOL_CAPACITY];
size_t __cynta_satisfy_pool_index = 0;

static cynta_parser_error_t satisfy_apply(cynta_parser_t *base, cynta_stream_t *stream, void *out)
{
    cynta_satisfy_parser_t *parser = (cynta_satisfy_parser_t *)base;
    if (parser == NULL ||
        stream == NULL ||
        out == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    uint8_t incoming;
    cynta_stream_error_t err = cynta_stream_next(stream, &incoming);
    switch (err)
    {
    case CYNTA_STREAM_SUCCESS:
        break;

    case CYNTA_STREAM_ERROR_END_OF_STREAM:
        return CYNTA_PARSER_ERROR_END_OF_STREAM;

    default:
        return CYNTA_PARSER_ERROR_INTERNAL;
    }

    if (!parser->condition(parser, incoming))
    {
        return CYNTA_PARSER_ERROR_UNEXPECTED_VALUE;
    }

    ((cynta_uint8_array_t *)out)->data[0] = incoming;
    ((cynta_uint8_array_t *)out)->size = 1;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_error_t cynta_satisfy_init(cynta_satisfy_parser_t *parser, bool (*cond)(cynta_satisfy_parser_t *, uint8_t))
{
    if (parser == NULL ||
        cond == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    parser->base.apply = satisfy_apply;
    parser->condition = cond;

    return CYNTA_PARSER_SUCCESS;
}