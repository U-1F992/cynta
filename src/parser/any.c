#include <cynta.h>

#if (0 < CYNTA_GLOBAL_POOL_ANY_CAPACITY)

cynta_any_t __cynta_global_pool_any[CYNTA_GLOBAL_POOL_ANY_CAPACITY];
size_t __cynta_global_pool_any_index = 0;

static bool any_condition(cynta_satisfy_t *base, uint8_t incoming) {
    return true;
}

cynta_parser_error_t cynta_any_init(cynta_any_t *self) {
    if (self == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    return cynta_satisfy_init(&self->base, any_condition);
}

#endif