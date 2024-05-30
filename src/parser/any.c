#include <cynta.h>

static bool any_condition(cynta_satisfy_parser_t *base, uint8_t incoming)
{
    return true;
}

cynta_parser_error_t cynta_any_init(cynta_any_parser_t *parser)
{
    if (parser == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    return cynta_satisfy_init(&parser->base, any_condition);
}

cynta_parser_t *cynta_any(void)
{
    cynta_any_parser_t *parser = (cynta_any_parser_t *)malloc(sizeof(cynta_any_parser_t));
    if (parser == NULL)
    {
        return NULL;
    }

    cynta_parser_error_t err = cynta_any_init(parser);
    if (err != CYNTA_PARSER_SUCCESS)
    {
        free(parser);
        return NULL;
    }

    return (cynta_parser_t *)parser;
}