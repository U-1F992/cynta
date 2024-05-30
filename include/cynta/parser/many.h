#ifndef CYNTA_PARSER_MANY_H
#define CYNTA_PARSER_MANY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../parser.h"

    typedef struct cynta_many_parser_t
    {
        cynta_parser_t base;

        cynta_parser_t *parser;
    } cynta_many_parser_t;

    cynta_parser_error_t cynta_many_init(cynta_many_parser_t *, cynta_parser_t *);

#ifndef CYNTA_MANY_POOL_CAPACITY
#define CYNTA_MANY_POOL_CAPACITY 256
#endif
    extern cynta_many_parser_t __cynta_many_pool[CYNTA_MANY_POOL_CAPACITY];
    extern size_t __cynta_many_pool_index;
#define cynta_many(p) (__cynta_many_pool_index >= CYNTA_MANY_POOL_CAPACITY                                         \
                           ? NULL                                                                                  \
                       : cynta_many_init(&__cynta_many_pool[__cynta_many_pool_index], (p)) != CYNTA_PARSER_SUCCESS \
                           ? NULL                                                                                  \
                           : (cynta_parser_t *)&__cynta_many_pool[__cynta_many_pool_index++])

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_PARSER_MANY_H */