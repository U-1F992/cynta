#include <cynta/parser/any.h>

cynta_any_parser_t __cynta_any_pool[CYNTA_ANY_POOL_CAPACITY];
size_t __cynta_any_pool_index = 0;

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