#ifndef CYNTA_PARSER_REPEAT_H
#define CYNTA_PARSER_REPEAT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../parser.h"

    typedef struct cynta_repeat_parser_t
    {
        cynta_parser_t base;

        cynta_parser_t *parser;
        size_t count;
    } cynta_repeat_parser_t;

    cynta_parser_error_t cynta_repeat_init(cynta_repeat_parser_t *, cynta_parser_t *, size_t);

#ifndef CYNTA_REPEAT_POOL_CAPACITY
#define CYNTA_REPEAT_POOL_CAPACITY 256
#endif
    extern cynta_repeat_parser_t __cynta_repeat_pool[CYNTA_REPEAT_POOL_CAPACITY];
    extern size_t __cynta_repeat_pool_index;
#define cynta_repeat(p, n) (__cynta_repeat_pool_index >= CYNTA_REPEAT_POOL_CAPACITY                                                \
                                ? NULL                                                                                             \
                            : cynta_repeat_init(&__cynta_repeat_pool[__cynta_repeat_pool_index], (p), (n)) != CYNTA_PARSER_SUCCESS \
                                ? NULL                                                                                             \
                                : (cynta_parser_t *)&__cynta_repeat_pool[__cynta_repeat_pool_index++])

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_PARSER_REPEAT_H */