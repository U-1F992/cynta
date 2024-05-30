#ifndef CYNTA_PARSER_ANY_H
#define CYNTA_PARSER_ANY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "satisfy.h"

    typedef struct cynta_any_parser_t
    {
        cynta_satisfy_parser_t base;
    } cynta_any_parser_t;

    cynta_parser_error_t cynta_any_init(cynta_any_parser_t *);

#ifndef CYNTA_ANY_POOL_CAPACITY
#define CYNTA_ANY_POOL_CAPACITY 256
#endif
    extern cynta_any_parser_t __cynta_any_pool[CYNTA_ANY_POOL_CAPACITY];
    extern size_t __cynta_any_pool_index;
#define cynta_any() (__cynta_any_pool_index >= CYNTA_ANY_POOL_CAPACITY                                   \
                         ? NULL                                                                          \
                     : cynta_any_init(&__cynta_any_pool[__cynta_any_pool_index]) != CYNTA_PARSER_SUCCESS \
                         ? NULL                                                                          \
                         : (cynta_parser_t *)&__cynta_any_pool[__cynta_any_pool_index++])

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_PARSER_ANY_H */