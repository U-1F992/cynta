#ifndef CYNTA_H
#define CYNTA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef CYNTA_STREAM_BACKTRACK_HISTORY_CAPACITY
#define CYNTA_STREAM_BACKTRACK_HISTORY_CAPACITY 256
#endif

#ifndef CYNTA_STREAM_BACKTRACK_CHECKPOINTS_CAPACITY
#define CYNTA_STREAM_BACKTRACK_CHECKPOINTS_CAPACITY 256
#endif

#ifndef CYNTA_UINT8_ARRAY_CAPACITY
#define CYNTA_UINT8_ARRAY_CAPACITY 256
#endif

#ifndef CYNTA_GLOBAL_POOL_SATISFY_CAPACITY
#define CYNTA_GLOBAL_POOL_SATISFY_CAPACITY 256
#endif

#ifndef CYNTA_GLOBAL_POOL_ANY_CAPACITY
#define CYNTA_GLOBAL_POOL_ANY_CAPACITY 256
#endif

#ifndef CYNTA_GLOBAL_POOL_VALUE_CAPACITY
#define CYNTA_GLOBAL_POOL_VALUE_CAPACITY 256
#endif

#ifndef CYNTA_GLOBAL_POOL_SEQUENCE_CAPACITY
#define CYNTA_GLOBAL_POOL_SEQUENCE_CAPACITY 256
#endif

#ifndef CYNTA_GLOBAL_POOL_REPEAT_CAPACITY
#define CYNTA_GLOBAL_POOL_REPEAT_CAPACITY 256
#endif

#ifndef CYNTA_GLOBAL_POOL_MANY_CAPACITY
#define CYNTA_GLOBAL_POOL_MANY_CAPACITY 256
#endif

#ifndef CYNTA_GLOBAL_POOL_TRY_CAPACITY
#define CYNTA_GLOBAL_POOL_TRY_CAPACITY 256
#endif

#ifndef CYNTA_GLOBAL_POOL_CHOICE_CAPACITY
#define CYNTA_GLOBAL_POOL_CHOICE_CAPACITY 256
#endif

#ifndef CYNTA_SEQUENCE_VA_ARGS_CAPACITY
#define CYNTA_SEQUENCE_VA_ARGS_CAPACITY 256
#endif

#ifndef CYNTA_CHOICE_VA_ARGS_CAPACITY
#define CYNTA_CHOICE_VA_ARGS_CAPACITY 256
#endif

/**********/
/* stream */
/**********/

typedef enum cynta_stream_error_t {
    CYNTA_STREAM_SUCCESS,
    CYNTA_STREAM_ERROR_NULL_POINTER,
    CYNTA_STREAM_ERROR_OUT_OF_CAPACITY,
    CYNTA_STREAM_ERROR_NO_CHECKPOINT,
    CYNTA_STREAM_ERROR_END_OF_STREAM,
    CYNTA_STREAM_ERROR_INTERNAL
} cynta_stream_error_t;

#define CYNTA_STREAM_ERROR(err)                                                \
    ((err) == CYNTA_STREAM_SUCCESS ? "CYNTA_STREAM_SUCCESS"                    \
     : (err) == CYNTA_STREAM_ERROR_NULL_POINTER                                \
         ? "CYNTA_STREAM_ERROR_NULL_POINTER"                                   \
     : (err) == CYNTA_STREAM_ERROR_OUT_OF_CAPACITY                             \
         ? "CYNTA_STREAM_ERROR_OUT_OF_CAPACITY"                                \
     : (err) == CYNTA_STREAM_ERROR_NO_CHECKPOINT                               \
         ? "CYNTA_STREAM_ERROR_NO_CHECKPOINT"                                  \
     : (err) == CYNTA_STREAM_ERROR_END_OF_STREAM                               \
         ? "CYNTA_STREAM_ERROR_END_OF_STREAM"                                  \
     : (err) == CYNTA_STREAM_ERROR_INTERNAL                                    \
         ? "CYNTA_STREAM_ERROR_INTERNAL"                                       \
         : (assert(0), "CYNTA_STREAM_ERROR_UNKNOWN"))

typedef struct cynta_stream_t {
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

/*****************/
/* uint8_array */
/*****************/

typedef struct cynta_uint8_array_t {
    uint8_t data[CYNTA_UINT8_ARRAY_CAPACITY];
    size_t size;
} cynta_uint8_array_t;

/*****************/
/* parser */
/*****************/

typedef enum cynta_parser_error_t {
    CYNTA_PARSER_SUCCESS,
    CYNTA_PARSER_ERROR_NULL_POINTER,
    CYNTA_PARSER_ERROR_OUT_OF_CAPACITY,
    CYNTA_PARSER_ERROR_END_OF_STREAM,
    CYNTA_PARSER_ERROR_UNEXPECTED_VALUE,
    CYNTA_PARSER_ERROR_INTERNAL
} cynta_parser_error_t;

#define CYNTA_PARSER_ERROR(err)                                                \
    ((err) == CYNTA_PARSER_SUCCESS ? "CYNTA_PARSER_SUCCESS"                    \
     : (err) == CYNTA_PARSER_ERROR_NULL_POINTER                                \
         ? "CYNTA_PARSER_ERROR_NULL_POINTER"                                   \
     : (err) == CYNTA_PARSER_ERROR_OUT_OF_CAPACITY                             \
         ? "CYNTA_PARSER_ERROR_OUT_OF_CAPACITY"                                \
     : (err) == CYNTA_PARSER_ERROR_END_OF_STREAM                               \
         ? "CYNTA_PARSER_ERROR_END_OF_STREAM"                                  \
     : (err) == CYNTA_PARSER_ERROR_UNEXPECTED_VALUE                            \
         ? "CYNTA_PARSER_ERROR_UNEXPECTED_VALUE"                               \
     : (err) == CYNTA_PARSER_ERROR_INTERNAL                                    \
         ? "CYNTA_PARSER_ERROR_INTERNAL"                                       \
         : (assert(0), "CYNTA_PARSER_ERROR_UNKNOWN"))

typedef struct cynta_parser_t {
    cynta_parser_error_t (*apply)(struct cynta_parser_t *, cynta_stream_t *,
                                  void *);
} cynta_parser_t;

#define cynta_parser_apply(parser, stream, out)                                \
    ((parser) == NULL ? CYNTA_PARSER_ERROR_NULL_POINTER                        \
                      : (parser)->apply((parser), (stream), (out)))

/*****************/
/* parser/satisfy */
/*****************/

#if (0 < CYNTA_GLOBAL_POOL_SATISFY_CAPACITY + CYNTA_GLOBAL_POOL_ANY_CAPACITY + \
             CYNTA_GLOBAL_POOL_VALUE_CAPACITY)

typedef struct cynta_satisfy_t {
    cynta_parser_t base;

    bool (*condition)(struct cynta_satisfy_t *, uint8_t);
} cynta_satisfy_t;

cynta_parser_error_t cynta_satisfy_init(cynta_satisfy_t *,
                                        bool (*)(cynta_satisfy_t *, uint8_t));

extern cynta_satisfy_t
    __cynta_global_pool_satisfy[CYNTA_GLOBAL_POOL_SATISFY_CAPACITY];
extern size_t __cynta_global_pool_satisfy_index;

#define cynta_satisfy(cond)                                                    \
    (__cynta_global_pool_satisfy_index >= CYNTA_GLOBAL_POOL_SATISFY_CAPACITY   \
         ? NULL                                                                \
     : cynta_satisfy_init(                                                     \
           &__cynta_global_pool_satisfy[__cynta_global_pool_satisfy_index],    \
           (cond)) != CYNTA_PARSER_SUCCESS                                     \
         ? NULL                                                                \
         : (cynta_parser_t *)&__cynta_global_pool_satisfy                      \
               [__cynta_global_pool_satisfy_index++])

#endif

/*****************/
/* parser/any */
/*****************/

#if (0 < CYNTA_GLOBAL_POOL_ANY_CAPACITY)

typedef struct cynta_any_t {
    cynta_satisfy_t base;
} cynta_any_t;

cynta_parser_error_t cynta_any_init(cynta_any_t *);

extern cynta_any_t __cynta_global_pool_any[CYNTA_GLOBAL_POOL_ANY_CAPACITY];
extern size_t __cynta_global_pool_any_index;

#define cynta_any()                                                            \
    (__cynta_global_pool_any_index >= CYNTA_GLOBAL_POOL_ANY_CAPACITY ? NULL    \
     : cynta_any_init(                                                         \
           &__cynta_global_pool_any[__cynta_global_pool_any_index]) !=         \
             CYNTA_PARSER_SUCCESS                                              \
         ? NULL                                                                \
         : (cynta_parser_t                                                     \
                *)&__cynta_global_pool_any[__cynta_global_pool_any_index++])

#endif

/*****************/
/* parser/value */
/*****************/

#if (0 < CYNTA_GLOBAL_POOL_VALUE_CAPACITY)

typedef struct cynta_value_t {
    cynta_satisfy_t base;

    uint8_t value;
} cynta_value_t;

cynta_parser_error_t cynta_value_init(cynta_value_t *, uint8_t);

extern cynta_value_t
    __cynta_global_pool_value[CYNTA_GLOBAL_POOL_VALUE_CAPACITY];
extern size_t __cynta_global_pool_value_index;

#define cynta_value(value)                                                     \
    (__cynta_global_pool_value_index >= CYNTA_GLOBAL_POOL_VALUE_CAPACITY       \
         ? NULL                                                                \
     : cynta_value_init(                                                       \
           &__cynta_global_pool_value[__cynta_global_pool_value_index],        \
           (value)) != CYNTA_PARSER_SUCCESS                                    \
         ? NULL                                                                \
         : (cynta_parser_t *)&__cynta_global_pool_value                        \
               [__cynta_global_pool_value_index++])

#endif

/*****************/
/* parser/sequence */
/*****************/

#if (0 < CYNTA_GLOBAL_POOL_SEQUENCE_CAPACITY)

typedef struct cynta_sequence_t {
    cynta_parser_t base;

    cynta_parser_t *parsers[CYNTA_SEQUENCE_VA_ARGS_CAPACITY];
    size_t parsers_size;
    cynta_uint8_array_t incoming_buffer;
} cynta_sequence_t;

cynta_parser_error_t cynta_sequence_init(cynta_sequence_t *, size_t, ...);

extern cynta_sequence_t
    __cynta_global_pool_sequence[CYNTA_GLOBAL_POOL_SEQUENCE_CAPACITY];
extern size_t __cynta_global_pool_sequence_index;

#define cynta_sequence(size, ...)                                              \
    (__cynta_global_pool_sequence_index >= CYNTA_GLOBAL_POOL_SEQUENCE_CAPACITY \
         ? NULL                                                                \
     : cynta_sequence_init(                                                    \
           &__cynta_global_pool_sequence[__cynta_global_pool_sequence_index],  \
           size, __VA_ARGS__) != CYNTA_PARSER_SUCCESS                          \
         ? NULL                                                                \
         : (cynta_parser_t *)&__cynta_global_pool_sequence                     \
               [__cynta_global_pool_sequence_index++])

#endif

/*****************/
/* parser/repeat */
/*****************/

#if (0 < CYNTA_GLOBAL_POOL_REPEAT_CAPACITY)

typedef struct cynta_repeat_t {
    cynta_parser_t base;

    cynta_parser_t *parser;
    size_t count;
    cynta_uint8_array_t incoming_buffer;
} cynta_repeat_t;

cynta_parser_error_t cynta_repeat_init(cynta_repeat_t *, cynta_parser_t *,
                                       size_t);

extern cynta_repeat_t
    __cynta_global_pool_repeat[CYNTA_GLOBAL_POOL_REPEAT_CAPACITY];
extern size_t __cynta_global_pool_repeat_index;

#define cynta_repeat(p, n)                                                     \
    (__cynta_global_pool_repeat_index >= CYNTA_GLOBAL_POOL_REPEAT_CAPACITY     \
         ? NULL                                                                \
     : cynta_repeat_init(                                                      \
           &__cynta_global_pool_repeat[__cynta_global_pool_repeat_index], (p), \
           (n)) != CYNTA_PARSER_SUCCESS                                        \
         ? NULL                                                                \
         : (cynta_parser_t *)&__cynta_global_pool_repeat                       \
               [__cynta_global_pool_repeat_index++])

#endif

/*****************/
/* parser/many */
/*****************/

#if (0 < CYNTA_GLOBAL_POOL_MANY_CAPACITY)

typedef struct cynta_many_t {
    cynta_parser_t base;

    cynta_parser_t *parser;
    cynta_uint8_array_t incoming_buffer;
} cynta_many_t;

cynta_parser_error_t cynta_many_init(cynta_many_t *, cynta_parser_t *);

extern cynta_many_t __cynta_global_pool_many[CYNTA_GLOBAL_POOL_MANY_CAPACITY];
extern size_t __cynta_global_pool_many_index;

#define cynta_many(p)                                                          \
    (__cynta_global_pool_many_index >= CYNTA_GLOBAL_POOL_MANY_CAPACITY ? NULL  \
     : cynta_many_init(                                                        \
           &__cynta_global_pool_many[__cynta_global_pool_many_index], (p)) !=  \
             CYNTA_PARSER_SUCCESS                                              \
         ? NULL                                                                \
         : (cynta_parser_t                                                     \
                *)&__cynta_global_pool_many[__cynta_global_pool_many_index++])

/*****************/
/* parser/try */
/*****************/

#if (0 < CYNTA_GLOBAL_POOL_TRY_CAPACITY)

typedef struct cynta_try_t {
    cynta_parser_t base;

    cynta_parser_t *parser;
} cynta_try_t;

cynta_parser_error_t cynta_try_init(cynta_try_t *, cynta_parser_t *);

extern cynta_try_t __cynta_global_pool_try[CYNTA_GLOBAL_POOL_TRY_CAPACITY];
extern size_t __cynta_global_pool_try_index;

#define cynta_try(p)                                                           \
    (__cynta_global_pool_try_index >= CYNTA_GLOBAL_POOL_TRY_CAPACITY ? NULL    \
     : cynta_try_init(&__cynta_global_pool_try[__cynta_global_pool_try_index], \
                      (p)) != CYNTA_PARSER_SUCCESS                             \
         ? NULL                                                                \
         : (cynta_parser_t                                                     \
                *)&__cynta_global_pool_try[__cynta_global_pool_try_index++])

#endif

/*****************/
/* parser/choice */
/*****************/

#if (0 < CYNTA_GLOBAL_POOL_CHOICE_CAPACITY)

typedef struct cynta_choice_t {
    cynta_parser_t base;

    cynta_parser_t *parsers[CYNTA_CHOICE_VA_ARGS_CAPACITY];
    size_t parsers_size;
} cynta_choice_t;

cynta_parser_error_t cynta_choice_init(cynta_choice_t *, size_t, ...);

extern cynta_choice_t
    __cynta_global_pool_choice[CYNTA_GLOBAL_POOL_CHOICE_CAPACITY];
extern size_t __cynta_global_pool_choice_index;

#define cynta_choice(size, ...)                                                \
    (__cynta_global_pool_choice_index >= CYNTA_GLOBAL_POOL_CHOICE_CAPACITY     \
         ? NULL                                                                \
     : cynta_choice_init(                                                      \
           &__cynta_global_pool_choice[__cynta_global_pool_choice_index],      \
           size, __VA_ARGS__) != CYNTA_PARSER_SUCCESS                          \
         ? NULL                                                                \
         : (cynta_parser_t *)&__cynta_global_pool_choice                       \
               [__cynta_global_pool_choice_index++])
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_H */