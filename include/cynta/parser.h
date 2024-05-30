#ifndef CYNTA_PARSER_H
#define CYNTA_PARSER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stream.h"

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
    } cynta_parser_t;

    static inline cynta_parser_error_t cynta_parser_apply(cynta_parser_t *parser, cynta_stream_t *stream, void *out)
    {
        return parser == NULL
                   ? CYNTA_PARSER_ERROR_NULL_POINTER
                   : parser->apply(parser, stream, out);
    }

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_PARSER_H */