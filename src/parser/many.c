#include <cynta.h>

#include <memory.h>

static cynta_parser_error_t many_apply(cynta_parser_t *base, cynta_stream_t *stream, void *out)
{
    cynta_many_parser_t *parser = (cynta_many_parser_t *)base;
    if (parser == NULL ||
        stream == NULL ||
        out == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    size_t total_size = 0;

    while (true)
    {
        cynta_uint8_array_t part_out;

        cynta_stream_push_checkpoint(stream);
        cynta_parser_error_t err = cynta_parser_apply(parser->parser, stream, (void *)&part_out);
        if (err != CYNTA_PARSER_SUCCESS)
        {
            if (err == CYNTA_PARSER_ERROR_END_OF_STREAM)
            {
                cynta_stream_discard_checkpoint(stream);
                break;
            }
            else if (err == CYNTA_PARSER_ERROR_UNEXPECTED_VALUE)
            {
                cynta_stream_rewind(stream);
                break;
            }
            else
            {
                cynta_stream_discard_checkpoint(stream);
                return err;
            }
        }
        cynta_stream_discard_checkpoint(stream);

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

static void many_delete(cynta_parser_t *base)
{
    cynta_many_parser_t *parser = (cynta_many_parser_t *)base;
    if (parser != NULL)
    {
        cynta_parser_delete(parser->parser);
    }
}

cynta_parser_error_t cynta_many_init(cynta_many_parser_t *parser, cynta_parser_t *p)
{
    if (parser == NULL ||
        p == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    parser->base.apply = many_apply;
    parser->base.delete = many_delete;
    parser->parser = p;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_t *cynta_many(cynta_parser_t *p)
{
    cynta_many_parser_t *parser = (cynta_many_parser_t *)malloc(sizeof(cynta_many_parser_t));
    if (parser == NULL)
    {
        return NULL;
    }

    cynta_parser_error_t err = cynta_many_init(parser, p);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        free(parser);
        return NULL;
    }

    return (cynta_parser_t *)parser;
}