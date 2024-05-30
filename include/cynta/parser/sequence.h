#ifndef CYNTA_PARSER_SEQUENCE_H
#define CYNTA_PARSER_SEQUENCE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../parser.h"

    typedef struct cynta_sequence_parser_t
    {
        cynta_parser_t base;

        cynta_parser_t *parsers[2];
    } cynta_sequence_parser_t;

    cynta_parser_error_t cynta_sequence_init(cynta_sequence_parser_t *, cynta_parser_t *, cynta_parser_t *);

#ifndef CYNTA_SEQUENCE_POOL_CAPACITY
#define CYNTA_SEQUENCE_POOL_CAPACITY 256
#endif
    extern cynta_sequence_parser_t __cynta_sequence_pool[CYNTA_SEQUENCE_POOL_CAPACITY];
    extern size_t __cynta_sequence_pool_index;
#define cynta_sequence(p0, p1) (__cynta_sequence_pool_index >= CYNTA_SEQUENCE_POOL_CAPACITY                                                    \
                                    ? NULL                                                                                                     \
                                : cynta_sequence_init(&__cynta_sequence_pool[__cynta_sequence_pool_index], (p0), (p1)) != CYNTA_PARSER_SUCCESS \
                                    ? NULL                                                                                                     \
                                    : (cynta_parser_t *)&__cynta_sequence_pool[__cynta_sequence_pool_index++])

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_PARSER_SEQUENCE_H */