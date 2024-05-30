#ifndef CYNTA_PARSER_SATISFY_H
#define CYNTA_PARSER_SATISFY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../parser.h"

#include <stdbool.h>

    typedef struct cynta_satisfy_parser_t
    {
        cynta_parser_t base;

        bool (*condition)(struct cynta_satisfy_parser_t *, uint8_t);
    } cynta_satisfy_parser_t;

    cynta_parser_error_t cynta_satisfy_init(cynta_satisfy_parser_t *, bool (*)(cynta_satisfy_parser_t *, uint8_t));

#ifndef CYNTA_SATISFY_POOL_CAPACITY
#define CYNTA_SATISFY_POOL_CAPACITY 256
#endif
    extern cynta_satisfy_parser_t __cynta_satisfy_pool[CYNTA_SATISFY_POOL_CAPACITY];
    extern size_t __cynta_satisfy_pool_index;
#define cynta_satisfy(cond) (__cynta_satisfy_pool_index >= CYNTA_SATISFY_POOL_CAPACITY                                               \
                                 ? NULL                                                                                              \
                             : cynta_satisfy_init(&__cynta_satisfy_pool[__cynta_satisfy_pool_index], (cond)) != CYNTA_PARSER_SUCCESS \
                                 ? NULL                                                                                              \
                                 : (cynta_parser_t *)&__cynta_satisfy_pool[__cynta_satisfy_pool_index++])

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_PARSER_SATISFY_H */