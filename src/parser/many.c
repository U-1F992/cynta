#include <cynta.h>

#include <memory.h>

static cynta_parser_error_t many_apply(cynta_parser_t *base, cynta_stream_t *stream, void **out, size_t *out_size)
{
    cynta_many_parser_t *parser = (cynta_many_parser_t *)base;
    if (parser == NULL ||
        stream == NULL ||
        out == NULL ||
        out_size == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    size_t total_size = 0;
    uint8_t *buffer = NULL;

    while (true)
    {
        uint8_t *part_out;
        size_t part_out_size;

        cynta_stream_push_checkpoint(stream);
        cynta_parser_error_t err = cynta_parser_apply(parser->parser, stream, (void *)&part_out, &part_out_size);
        if (err != CYNTA_PARSER_SUCCESS)
        {
            if (err == CYNTA_PARSER_ERROR_END_OF_STREAM)
            {
                cynta_stream_discard_checkpoint(stream);
                break;
            }
            if (err == CYNTA_PARSER_ERROR_UNEXPECTED_VALUE)
            {
                cynta_stream_rewind(stream);
                break;
            }
            else
            {
                // Cleanup any previously allocated memory
                cynta_stream_discard_checkpoint(stream);
                free(buffer);
                return err;
            }
        }
        cynta_stream_discard_checkpoint(stream);

        buffer = (uint8_t *)realloc(buffer, total_size + part_out_size);
        if (buffer == NULL)
        {
            free(part_out);
            return CYNTA_PARSER_ERROR_MEMORY_ALLOCATION;
        }
        
        for (size_t i = 0; i < part_out_size; i++)
        {
            buffer[total_size + i] = part_out[i];
        }
        
        total_size += part_out_size;
        free(part_out);
    }

    *out = buffer;
    *out_size = total_size;

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