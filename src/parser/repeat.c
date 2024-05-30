#include <cynta.h>

#include <memory.h>

static cynta_parser_error_t repeat_apply(cynta_parser_t *base, cynta_stream_t *stream, void **out, size_t *out_size)
{
    cynta_repeat_parser_t *parser = (cynta_repeat_parser_t *)base;
    if (parser == NULL ||
        stream == NULL ||
        out == NULL ||
        out_size == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    size_t total_size = 0;
    uint8_t *buffer = NULL;

    for (size_t i = 0; i < parser->count; i++)
    {
        void *part_out;
        size_t part_out_size;
        cynta_parser_error_t err = cynta_parser_apply(parser->parser, stream, &part_out, &part_out_size);
        if (err != CYNTA_PARSER_SUCCESS)
        {
            // Cleanup any previously allocated memory
            free(buffer);
            return err;
        }

        buffer = realloc(buffer, total_size + part_out_size);
        if (buffer == NULL)
        {
            free(part_out);
            return CYNTA_PARSER_ERROR_MEMORY_ALLOCATION;
        }

        memcpy(buffer + total_size, part_out, part_out_size);
        total_size += part_out_size;
        free(part_out);
    }

    *out = buffer;
    *out_size = total_size;

    return CYNTA_PARSER_SUCCESS;
}

static void repeat_delete(cynta_parser_t *base)
{
    cynta_repeat_parser_t *parser = (cynta_repeat_parser_t *)base;
    if (parser != NULL)
    {
        cynta_parser_delete(parser->parser);
    }
}

cynta_parser_error_t cynta_repeat_init(cynta_repeat_parser_t *parser, cynta_parser_t *p, size_t n)
{
    if (parser == NULL ||
        p == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    parser->base.apply = repeat_apply;
    parser->base.delete = repeat_delete;
    parser->parser = p;
    parser->count = n;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_t *cynta_repeat(cynta_parser_t *p, size_t n)
{
    cynta_repeat_parser_t *parser = (cynta_repeat_parser_t *)malloc(sizeof(cynta_repeat_parser_t));
    if (parser == NULL)
    {
        return NULL;
    }

    cynta_parser_error_t err = cynta_repeat_init(parser, p, n);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        free(parser);
        return NULL;
    }

    return (cynta_parser_t *)parser;
}