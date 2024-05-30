#ifndef CYNTA_H
#define CYNTA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef CYNTA_STREAM_BACKTRACK_HISTORY_CAPACITY
#define CYNTA_STREAM_BACKTRACK_HISTORY_CAPACITY ((size_t)256)
#endif

#ifndef CYNTA_STREAM_BACKTRACK_CHECKPOINTS_CAPACITY
#define CYNTA_STREAM_BACKTRACK_CHECKPOINTS_CAPACITY ((size_t)256)
#endif

#ifndef CYNTA_UINT8_ARRAY_CAPACITY
#define CYNTA_UINT8_ARRAY_CAPACITY ((size_t)256)
#endif

#ifndef CYNTA_SATISFY_POOL_CAPACITY
#define CYNTA_SATISFY_POOL_CAPACITY 256
#endif

#ifndef CYNTA_ANY_POOL_CAPACITY
#define CYNTA_ANY_POOL_CAPACITY 256
#endif

#ifndef CYNTA_VALUE_POOL_CAPACITY
#define CYNTA_VALUE_POOL_CAPACITY 256
#endif

#ifndef CYNTA_SEQUENCE_POOL_CAPACITY
#define CYNTA_SEQUENCE_POOL_CAPACITY 256
#endif

#ifndef CYNTA_REPEAT_POOL_CAPACITY
#define CYNTA_REPEAT_POOL_CAPACITY 256
#endif

#ifndef CYNTA_MANY_POOL_CAPACITY
#define CYNTA_MANY_POOL_CAPACITY 256
#endif

    /****************************************************************************************************/
    /* stream                                                                                           */
    /****************************************************************************************************/

    typedef enum cynta_stream_error_t
    {
        CYNTA_STREAM_SUCCESS,
        CYNTA_STREAM_ERROR_NULL_POINTER,
        CYNTA_STREAM_ERROR_OUT_OF_CAPACITY,
        CYNTA_STREAM_ERROR_NO_CHECKPOINT,
        CYNTA_STREAM_ERROR_END_OF_STREAM,
        CYNTA_STREAM_ERROR_INTERNAL
    } cynta_stream_error_t;

#define CYNTA_STREAM_ERROR(err) ((err) == CYNTA_STREAM_SUCCESS                 ? "CYNTA_STREAM_SUCCESS"               \
                                 : (err) == CYNTA_STREAM_ERROR_NULL_POINTER    ? "CYNTA_STREAM_ERROR_NULL_POINTER"    \
                                 : (err) == CYNTA_STREAM_ERROR_OUT_OF_CAPACITY ? "CYNTA_STREAM_ERROR_OUT_OF_CAPACITY" \
                                 : (err) == CYNTA_STREAM_ERROR_NO_CHECKPOINT   ? "CYNTA_STREAM_ERROR_NO_CHECKPOINT"   \
                                 : (err) == CYNTA_STREAM_ERROR_END_OF_STREAM   ? "CYNTA_STREAM_ERROR_END_OF_STREAM"   \
                                 : (err) == CYNTA_STREAM_ERROR_INTERNAL        ? "CYNTA_STREAM_ERROR_INTERNAL"        \
                                                                               : (assert(0), "CYNTA_STREAM_ERROR_UNKNOWN"))

    typedef struct cynta_stream_t
    {
        cynta_stream_error_t (*next)(struct cynta_stream_t *, uint8_t *);

        uint8_t history[CYNTA_STREAM_BACKTRACK_HISTORY_CAPACITY];
        size_t history_size;
        size_t history_index;

        size_t checkpoints[CYNTA_STREAM_BACKTRACK_CHECKPOINTS_CAPACITY];
        size_t checkpoints_size;
    } cynta_stream_t;

    cynta_stream_error_t cynta_stream_push_checkpoint(cynta_stream_t *);
    cynta_stream_error_t cynta_stream_rewind(cynta_stream_t *);
    cynta_stream_error_t cynta_stream_discard_checkpoint(cynta_stream_t *);
    cynta_stream_error_t cynta_stream_next(cynta_stream_t *, uint8_t *);
    cynta_stream_error_t cynta_stream_init(cynta_stream_t *);

    /****************************************************************************************************/
    /* uint8_array                                                                                      */
    /****************************************************************************************************/

    typedef struct cynta_uint8_array_t
    {
        uint8_t data[CYNTA_UINT8_ARRAY_CAPACITY];
        size_t size;
    } cynta_uint8_array_t;

    /****************************************************************************************************/
    /* parser                                                                                           */
    /****************************************************************************************************/

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

#define cynta_parser_apply(parser, stream, out) ((parser) == NULL                      \
                                                     ? CYNTA_PARSER_ERROR_NULL_POINTER \
                                                     : (parser)->apply((parser), (stream), (out)))

    /****************************************************************************************************/
    /* parser/satisfy                                                                                   */
    /****************************************************************************************************/

#if 0 < CYNTA_SATISFY_POOL_CAPACITY + CYNTA_ANY_POOL_CAPACITY + CYNTA_VALUE_POOL_CAPACITY

    typedef struct cynta_satisfy_t
    {
        cynta_parser_t base;

        bool (*condition)(struct cynta_satisfy_t *, uint8_t);
    } cynta_satisfy_t;

    cynta_parser_error_t cynta_satisfy_init(cynta_satisfy_t *, bool (*)(cynta_satisfy_t *, uint8_t));

    extern cynta_satisfy_t __cynta_satisfy_pool[CYNTA_SATISFY_POOL_CAPACITY];
    extern size_t __cynta_satisfy_pool_index;

#define cynta_satisfy(cond) (__cynta_satisfy_pool_index >= CYNTA_SATISFY_POOL_CAPACITY                                               \
                                 ? NULL                                                                                              \
                             : cynta_satisfy_init(&__cynta_satisfy_pool[__cynta_satisfy_pool_index], (cond)) != CYNTA_PARSER_SUCCESS \
                                 ? NULL                                                                                              \
                                 : (cynta_parser_t *)&__cynta_satisfy_pool[__cynta_satisfy_pool_index++])

#endif

    /****************************************************************************************************/
    /* parser/any                                                                                       */
    /****************************************************************************************************/

#if 0 < CYNTA_ANY_POOL_CAPACITY

    typedef struct cynta_any_t
    {
        cynta_satisfy_t base;
    } cynta_any_t;

    cynta_parser_error_t cynta_any_init(cynta_any_t *);

    extern cynta_any_t __cynta_any_pool[CYNTA_ANY_POOL_CAPACITY];
    extern size_t __cynta_any_pool_index;

#define cynta_any() (__cynta_any_pool_index >= CYNTA_ANY_POOL_CAPACITY                                   \
                         ? NULL                                                                          \
                     : cynta_any_init(&__cynta_any_pool[__cynta_any_pool_index]) != CYNTA_PARSER_SUCCESS \
                         ? NULL                                                                          \
                         : (cynta_parser_t *)&__cynta_any_pool[__cynta_any_pool_index++])

#endif

    /****************************************************************************************************/
    /* parser/value                                                                                     */
    /****************************************************************************************************/

#if 0 < CYNTA_VALUE_POOL_CAPACITY

    typedef struct cynta_value_t
    {
        cynta_satisfy_t base;

        uint8_t value;
    } cynta_value_t;

    cynta_parser_error_t cynta_value_init(cynta_value_t *, uint8_t);

    extern cynta_value_t __cynta_value_pool[CYNTA_VALUE_POOL_CAPACITY];
    extern size_t __cynta_value_pool_index;

#define cynta_value(value) (__cynta_value_pool_index >= CYNTA_VALUE_POOL_CAPACITY                                              \
                                ? NULL                                                                                         \
                            : cynta_value_init(&__cynta_value_pool[__cynta_value_pool_index], (value)) != CYNTA_PARSER_SUCCESS \
                                ? NULL                                                                                         \
                                : (cynta_parser_t *)&__cynta_value_pool[__cynta_value_pool_index++])

#endif

    /****************************************************************************************************/
    /* parser/sequence                                                                                  */
    /****************************************************************************************************/

#if 0 < CYNTA_SEQUENCE_POOL_CAPACITY

    typedef struct cynta_sequence_t
    {
        cynta_parser_t base;

        cynta_parser_t *parsers[2];
        cynta_uint8_array_t incoming_buffer;
    } cynta_sequence_t;

    cynta_parser_error_t cynta_sequence_init(cynta_sequence_t *, cynta_parser_t *, cynta_parser_t *);

    extern cynta_sequence_t __cynta_sequence_pool[CYNTA_SEQUENCE_POOL_CAPACITY];
    extern size_t __cynta_sequence_pool_index;

#define cynta_sequence(p0, p1) (__cynta_sequence_pool_index >= CYNTA_SEQUENCE_POOL_CAPACITY                                                    \
                                    ? NULL                                                                                                     \
                                : cynta_sequence_init(&__cynta_sequence_pool[__cynta_sequence_pool_index], (p0), (p1)) != CYNTA_PARSER_SUCCESS \
                                    ? NULL                                                                                                     \
                                    : (cynta_parser_t *)&__cynta_sequence_pool[__cynta_sequence_pool_index++])

#endif

    /****************************************************************************************************/
    /* parser/repeat                                                                                    */
    /****************************************************************************************************/

#if 0 < CYNTA_REPEAT_POOL_CAPACITY

    typedef struct cynta_repeat_t
    {
        cynta_parser_t base;

        cynta_parser_t *parser;
        size_t count;
        cynta_uint8_array_t incoming_buffer;
    } cynta_repeat_t;

    cynta_parser_error_t cynta_repeat_init(cynta_repeat_t *, cynta_parser_t *, size_t);

    extern cynta_repeat_t __cynta_repeat_pool[CYNTA_REPEAT_POOL_CAPACITY];
    extern size_t __cynta_repeat_pool_index;

#define cynta_repeat(p, n) (__cynta_repeat_pool_index >= CYNTA_REPEAT_POOL_CAPACITY                                                \
                                ? NULL                                                                                             \
                            : cynta_repeat_init(&__cynta_repeat_pool[__cynta_repeat_pool_index], (p), (n)) != CYNTA_PARSER_SUCCESS \
                                ? NULL                                                                                             \
                                : (cynta_parser_t *)&__cynta_repeat_pool[__cynta_repeat_pool_index++])

#endif

    /****************************************************************************************************/
    /* parser/many                                                                                      */
    /****************************************************************************************************/

#if 0 < CYNTA_MANY_POOL_CAPACITY

    typedef struct cynta_many_t
    {
        cynta_parser_t base;

        cynta_parser_t *parser;
        cynta_uint8_array_t incoming_buffer;
    } cynta_many_t;

    cynta_parser_error_t cynta_many_init(cynta_many_t *, cynta_parser_t *);

    extern cynta_many_t __cynta_many_pool[CYNTA_MANY_POOL_CAPACITY];
    extern size_t __cynta_many_pool_index;

#define cynta_many(p) (__cynta_many_pool_index >= CYNTA_MANY_POOL_CAPACITY                                         \
                           ? NULL                                                                                  \
                       : cynta_many_init(&__cynta_many_pool[__cynta_many_pool_index], (p)) != CYNTA_PARSER_SUCCESS \
                           ? NULL                                                                                  \
                           : (cynta_parser_t *)&__cynta_many_pool[__cynta_many_pool_index++])

#endif

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_H */