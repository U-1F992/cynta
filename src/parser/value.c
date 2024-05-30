#include <cynta/parser/value.h>

cynta_value_parser_t __cynta_value_pool[CYNTA_VALUE_POOL_CAPACITY];
size_t __cynta_value_pool_index = 0;

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