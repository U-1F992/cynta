#ifndef CYNTA_PARSER_VALUE_H
#define CYNTA_PARSER_VALUE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "satisfy.h"

    typedef struct cynta_value_parser_t
    {
        cynta_satisfy_parser_t base;

        uint8_t value;
    } cynta_value_parser_t;

    cynta_parser_error_t cynta_value_init(cynta_value_parser_t *, uint8_t);

#ifndef CYNTA_VALUE_POOL_CAPACITY
#define CYNTA_VALUE_POOL_CAPACITY 256
#endif
    extern cynta_value_parser_t __cynta_value_pool[CYNTA_VALUE_POOL_CAPACITY];
    extern size_t __cynta_value_pool_index;
#define cynta_value(value) (__cynta_value_pool_index >= CYNTA_VALUE_POOL_CAPACITY                                              \
                                ? NULL                                                                                         \
                            : cynta_value_init(&__cynta_value_pool[__cynta_value_pool_index], (value)) != CYNTA_PARSER_SUCCESS \
                                ? NULL                                                                                         \
                                : (cynta_parser_t *)&__cynta_value_pool[__cynta_value_pool_index++])

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_PARSER_VALUE_H */