#include <cynta.h>

static cynta_parser_error_t satisfy_apply(cynta_parser_t *base, cynta_stream_t *stream, void **out, size_t *out_size)
{
    cynta_satisfy_parser_t *parser = (cynta_satisfy_parser_t *)base;
    if (parser == NULL ||
        stream == NULL ||
        out == NULL ||
        out_size == NULL)
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
    else
    {
        uint8_t *temp_out = (uint8_t *)malloc(sizeof(uint8_t));
        if (temp_out == NULL)
        {
            return CYNTA_PARSER_ERROR_MEMORY_ALLOCATION;
        }

        *temp_out = incoming;
        *out = temp_out;
        *out_size = 1;

        return CYNTA_PARSER_SUCCESS;
    }
}

static void satisfy_delete(cynta_parser_t *base)
{
}

cynta_parser_error_t cynta_satisfy_init(cynta_satisfy_parser_t *parser, bool (*condition)(cynta_satisfy_parser_t *, uint8_t))
{
    if (parser == NULL ||
        condition == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    parser->base.apply = satisfy_apply;
    parser->base.delete = satisfy_delete;
    parser->condition = condition;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_t *cynta_satisfy(bool (*condition)(cynta_satisfy_parser_t *, uint8_t))
{
    cynta_satisfy_parser_t *parser = (cynta_satisfy_parser_t *)malloc(sizeof(cynta_satisfy_parser_t));
    if (parser == NULL)
    {
        return NULL;
    }

    cynta_parser_error_t err = cynta_satisfy_init(parser, condition);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        free(parser);
        return NULL;
    }

    return (cynta_parser_t *)parser;
}