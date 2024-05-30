#include <cynta.h>

static cynta_parser_error_t sequence_apply(cynta_parser_t *base, cynta_stream_t *stream, void **out, size_t *out_size)
{
    cynta_sequence_parser_t *parser = (cynta_sequence_parser_t *)base;
    if (parser == NULL ||
        stream == NULL ||
        out == NULL ||
        out_size == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    uint8_t *out0;
    size_t out0_size;
    cynta_parser_error_t err = cynta_parser_apply(parser->parsers[0], stream, (void *)&out0, &out0_size);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        return err;
    }

    uint8_t *out1;
    size_t out1_size;
    err = cynta_parser_apply(parser->parsers[1], stream, (void *)&out1, &out1_size);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        free(out0);
        return err;
    }

    size_t temp_out_size = out0_size + out1_size;
    uint8_t *temp_out = (uint8_t *)malloc(sizeof(uint8_t) * temp_out_size);
    if (temp_out == NULL)
    {
        free(out0);
        free(out1);
        return CYNTA_PARSER_ERROR_MEMORY_ALLOCATION;
    }

    for (size_t i = 0; i < out0_size; i++)
    {
        temp_out[i] = out0[i];
    }
    for (size_t i = 0; i < out1_size; i++)
    {
        temp_out[out0_size + i] = out1[i];
    }
    free(out0);
    free(out1);

    *out = temp_out;
    *out_size = temp_out_size;

    return CYNTA_PARSER_SUCCESS;
}

static void sequence_delete(cynta_parser_t *base)
{
    cynta_sequence_parser_t *parser = (cynta_sequence_parser_t *)base;
    if (parser != NULL)
    {
        cynta_parser_delete(parser->parsers[0]);
        cynta_parser_delete(parser->parsers[1]);
    }
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
    parser->base.delete = sequence_delete;
    parser->parsers[0] = p0;
    parser->parsers[1] = p1;

    return CYNTA_PARSER_SUCCESS;
}

cynta_parser_t *cynta_sequence(cynta_parser_t *p0, cynta_parser_t *p1)
{
    cynta_sequence_parser_t *parser = (cynta_sequence_parser_t *)malloc(sizeof(cynta_sequence_parser_t));
    if (parser == NULL)
    {
        return NULL;
    }

    cynta_parser_error_t err = cynta_sequence_init(parser, p0, p1);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        free(parser);
        return NULL;
    }

    return (cynta_parser_t *)parser;
}