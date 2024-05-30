#ifndef CYNTA_PARSER_H
#define CYNTA_PARSER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stream.h"

#include <stdbool.h>
#include <stdlib.h>

    typedef enum cynta_parser_error_t
    {
        CYNTA_PARSER_SUCCESS,

        CYNTA_PARSER_ERROR_NULL_POINTER,
        CYNTA_PARSER_ERROR_OUT_OF_CAPACITY,
        CYNTA_PARSER_ERROR_END_OF_STREAM,
        CYNTA_PARSER_ERROR_UNEXPECTED_VALUE,

        CYNTA_PARSER_ERROR_INTERNAL
    } cynta_parser_error_t;

#define CYNTA_PARSER_ERROR(err) ((err) == CYNTA_PARSER_SUCCESS                  ? "CYNTA_PARSER_SUCCESS"                \
                                 : (err) == CYNTA_PARSER_ERROR_NULL_POINTER     ? "CYNTA_PARSER_ERROR_NULL_POINTER"     \
                                 : (err) == CYNTA_PARSER_ERROR_OUT_OF_CAPACITY  ? "CYNTA_PARSER_ERROR_OUT_OF_CAPACITY"  \
                                 : (err) == CYNTA_PARSER_ERROR_END_OF_STREAM    ? "CYNTA_PARSER_ERROR_END_OF_STREAM"    \
                                 : (err) == CYNTA_PARSER_ERROR_UNEXPECTED_VALUE ? "CYNTA_PARSER_ERROR_UNEXPECTED_VALUE" \
                                 : (err) == CYNTA_PARSER_ERROR_INTERNAL         ? "CYNTA_PARSER_ERROR_INTERNAL"         \
                                                                                : (assert(0), "CYNTA_PARSER_ERROR_UNKNOWN"))

    typedef struct cynta_parser_t
    {
        cynta_parser_error_t (*apply)(struct cynta_parser_t *, cynta_stream_t *, void *);
        void (*delete)(struct cynta_parser_t *);
    } cynta_parser_t;

    static inline cynta_parser_error_t cynta_parser_apply(cynta_parser_t *parser, cynta_stream_t *stream, void *out)
    {
        return parser == NULL
                   ? CYNTA_PARSER_ERROR_NULL_POINTER
                   : parser->apply(parser, stream, out);
    }

    static inline void cynta_parser_delete(cynta_parser_t *parser)
    {
        if (parser == NULL)
        {
            return;
        }

        parser->delete (parser);
        free(parser);
    }

#ifndef CYNTA_UINT8_ARRAY_CAPACITY
#define CYNTA_UINT8_ARRAY_CAPACITY 256
#endif

    typedef struct cynta_uint8_array_t
    {
        uint8_t data[CYNTA_UINT8_ARRAY_CAPACITY];
        size_t size;
    } cynta_uint8_array_t;

    typedef struct cynta_satisfy_parser_t
    {
        cynta_parser_t base;

        bool (*condition)(struct cynta_satisfy_parser_t *, uint8_t);
    } cynta_satisfy_parser_t;

    cynta_parser_error_t cynta_satisfy_init(cynta_satisfy_parser_t *, bool (*)(cynta_satisfy_parser_t *, uint8_t));
    cynta_parser_t *cynta_satisfy(bool (*condition)(cynta_satisfy_parser_t *, uint8_t));

    typedef struct cynta_any_parser_t
    {
        cynta_satisfy_parser_t base;
    } cynta_any_parser_t;

    cynta_parser_error_t cynta_any_init(cynta_any_parser_t *);
    cynta_parser_t *cynta_any(void);

    typedef struct cynta_value_parser_t
    {
        cynta_satisfy_parser_t base;

        uint8_t value;
    } cynta_value_parser_t;

    cynta_parser_error_t cynta_value_init(cynta_value_parser_t *, uint8_t);
    cynta_parser_t *cynta_value(uint8_t);

    typedef struct cynta_sequence_parser_t
    {
        cynta_parser_t base;

        cynta_parser_t *parsers[2];
    } cynta_sequence_parser_t;

    cynta_parser_error_t cynta_sequence_init(cynta_sequence_parser_t *, cynta_parser_t *, cynta_parser_t *);
    cynta_parser_t *cynta_sequence(cynta_parser_t *, cynta_parser_t *);

    typedef struct cynta_repeat_parser_t
    {
        cynta_parser_t base;

        cynta_parser_t *parser;
        size_t count;
    } cynta_repeat_parser_t;

    cynta_parser_error_t cynta_repeat_init(cynta_repeat_parser_t *, cynta_parser_t *, size_t);
    cynta_parser_t *cynta_repeat(cynta_parser_t *, size_t);

    typedef struct cynta_many_parser_t
    {
        cynta_parser_t base;

        cynta_parser_t *parser;
    } cynta_many_parser_t;

    cynta_parser_error_t cynta_many_init(cynta_many_parser_t *, cynta_parser_t *);
    cynta_parser_t *cynta_many(cynta_parser_t *);

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_PARSER_H */