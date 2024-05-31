#include <cynta.h>

#if (0 < CYNTA_VALUE_POOL_CAPACITY)

static bool value_condition(cynta_satisfy_t *base, uint8_t incoming) {
    cynta_value_t *self = (cynta_value_t *)base;
    return self != NULL && incoming == self->value;
}

cynta_parser_error_t cynta_value_init(cynta_value_t *self, uint8_t value) {
    if (self == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    self->value = value;

    return cynta_satisfy_init(&self->base, value_condition);
}

#endif