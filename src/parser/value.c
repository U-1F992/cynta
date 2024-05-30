#include <cynta.h>

static bool value_condition(cynta_satisfy_parser_t *base, uint8_t incoming)
{
    cynta_value_parser_t *parser = (cynta_value_parser_t *)base;
    return parser != NULL && incoming == parser->value;
}

cynta_parser_error_t cynta_value_init(cynta_value_parser_t *parser, uint8_t value)
{
    if (parser == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    parser->value = value;

    return cynta_satisfy_init(&parser->base, value_condition);
}

cynta_parser_t *cynta_value(uint8_t value)
{
    cynta_value_parser_t *parser = (cynta_value_parser_t *)malloc(sizeof(cynta_value_parser_t));
    if (parser == NULL)
    {
        return NULL;
    }

    cynta_parser_error_t err = cynta_value_init(parser, value);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        free(parser);
        return NULL;
    }

    return (cynta_parser_t *)parser;
}