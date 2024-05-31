#ifndef CYNTA_H
#define CYNTA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdarg.h>
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

/***************/
/* uint8_array */
/***************/

typedef struct cynta_uint8_array_t {
    uint8_t data[CYNTA_UINT8_ARRAY_CAPACITY];
    size_t size;
} cynta_uint8_array_t;

/**********/
/* parser */
/**********/

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

typedef struct cynta_parser_t cynta_parser_t;

typedef struct cynta_parser_vtable_t {
    cynta_parser_error_t (*apply)(cynta_parser_t *, cynta_stream_t *, void *);
} cynta_parser_vtable_t;

typedef struct cynta_parser_t {
    cynta_parser_vtable_t *vtbl;
} cynta_parser_t;

static inline cynta_parser_error_t
cynta_parser_apply(cynta_parser_t *self, cynta_stream_t *stream, void *out) {
    if (self == NULL) {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }
    return self->vtbl->apply(self, stream, out);
}

/******************/
/* parser/satisfy */
/******************/

#if (0 < CYNTA_GLOBAL_POOL_SATISFY_CAPACITY + CYNTA_GLOBAL_POOL_ANY_CAPACITY + \
             CYNTA_GLOBAL_POOL_VALUE_CAPACITY)

typedef struct cynta_satisfy_t {
    cynta_parser_t base;

    bool (*condition)(struct cynta_satisfy_t *, uint8_t);
} cynta_satisfy_t;

cynta_parser_error_t cynta_satisfy_init(cynta_satisfy_t *,
                                        bool (*)(cynta_satisfy_t *, uint8_t));

static inline cynta_parser_t *cynta_satisfy(bool (*condition)(cynta_satisfy_t *,
                                                              uint8_t)) {
    static cynta_satisfy_t pool[CYNTA_GLOBAL_POOL_SATISFY_CAPACITY];
    static size_t pool_index = 0;

    if (pool_index >= CYNTA_GLOBAL_POOL_SATISFY_CAPACITY) {
        return NULL;
    }
    if (cynta_satisfy_init(&pool[pool_index], condition) !=
        CYNTA_PARSER_SUCCESS) {
        return NULL;
    }
    return (cynta_parser_t *)&pool[pool_index++];
}

#endif

/**************/
/* parser/any */
/**************/

#if (0 < CYNTA_GLOBAL_POOL_ANY_CAPACITY)

typedef struct cynta_any_t {
    cynta_satisfy_t base;
} cynta_any_t;

cynta_parser_error_t cynta_any_init(cynta_any_t *);

static inline cynta_parser_t *cynta_any() {
    static cynta_any_t pool[CYNTA_GLOBAL_POOL_ANY_CAPACITY];
    static size_t pool_index = 0;

    if (pool_index >= CYNTA_GLOBAL_POOL_ANY_CAPACITY) {
        return NULL;
    }
    if (cynta_any_init(&pool[pool_index]) != CYNTA_PARSER_SUCCESS) {
        return NULL;
    }
    return (cynta_parser_t *)&pool[pool_index++];
}

#endif

/****************/
/* parser/value */
/****************/

#if (0 < CYNTA_GLOBAL_POOL_VALUE_CAPACITY)

typedef struct cynta_value_t {
    cynta_satisfy_t base;

    uint8_t value;
} cynta_value_t;

cynta_parser_error_t cynta_value_init(cynta_value_t *, uint8_t);

static inline cynta_parser_t *cynta_value(uint8_t value) {
    static cynta_value_t pool[CYNTA_GLOBAL_POOL_VALUE_CAPACITY];
    static size_t pool_index = 0;

    if (pool_index >= CYNTA_GLOBAL_POOL_VALUE_CAPACITY) {
        return NULL;
    }
    if (cynta_value_init(&pool[pool_index], value) != CYNTA_PARSER_SUCCESS) {
        return NULL;
    }
    return (cynta_parser_t *)&pool[pool_index++];
}

#endif

/*******************/
/* parser/sequence */
/*******************/

#if (0 < CYNTA_GLOBAL_POOL_SEQUENCE_CAPACITY)

typedef struct cynta_sequence_t {
    cynta_parser_t base;

    cynta_parser_t *parsers[CYNTA_SEQUENCE_VA_ARGS_CAPACITY];
    size_t parsers_size;
    cynta_uint8_array_t incoming_buffer;
} cynta_sequence_t;

cynta_parser_error_t cynta_internal_sequence_init(cynta_sequence_t *, size_t,
                                                  va_list);
cynta_parser_error_t cynta_sequence_init(cynta_sequence_t *, size_t, ...);

static inline cynta_parser_t *cynta_sequence(size_t size, ...) {
    static cynta_sequence_t pool[CYNTA_GLOBAL_POOL_SEQUENCE_CAPACITY];
    static size_t pool_index = 0;

    if (pool_index >= CYNTA_GLOBAL_POOL_SEQUENCE_CAPACITY) {
        return NULL;
    }
    va_list args;
    va_start(args, size);
    if (cynta_internal_sequence_init(&pool[pool_index], size, args) !=
        CYNTA_PARSER_SUCCESS) {
        return NULL;
    }
    va_end(args);
    return (cynta_parser_t *)&pool[pool_index++];
}

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

static inline cynta_parser_t *cynta_repeat(cynta_parser_t *parser,
                                           size_t count) {
    static cynta_repeat_t pool[CYNTA_GLOBAL_POOL_REPEAT_CAPACITY];
    static size_t pool_index = 0;

    if (pool_index >= CYNTA_GLOBAL_POOL_REPEAT_CAPACITY) {
        return NULL;
    }
    if (cynta_repeat_init(&pool[pool_index], parser, count) !=
        CYNTA_PARSER_SUCCESS) {
        return NULL;
    }
    return (cynta_parser_t *)&pool[pool_index++];
}

#endif

/***************/
/* parser/many */
/***************/

#if (0 < CYNTA_GLOBAL_POOL_MANY_CAPACITY)

typedef struct cynta_many_t {
    cynta_parser_t base;

    cynta_parser_t *parser;
    cynta_uint8_array_t incoming_buffer;
} cynta_many_t;

cynta_parser_error_t cynta_many_init(cynta_many_t *, cynta_parser_t *);

static inline cynta_parser_t *cynta_many(cynta_parser_t *parser) {
    static cynta_many_t pool[CYNTA_GLOBAL_POOL_MANY_CAPACITY];
    static size_t pool_index = 0;

    if (pool_index >= CYNTA_GLOBAL_POOL_MANY_CAPACITY) {
        return NULL;
    }
    if (cynta_many_init(&pool[pool_index], parser) != CYNTA_PARSER_SUCCESS) {
        return NULL;
    }
    return (cynta_parser_t *)&pool[pool_index++];
}

#endif

/**************/
/* parser/try */
/**************/

#if (0 < CYNTA_GLOBAL_POOL_TRY_CAPACITY)

typedef struct cynta_try_t {
    cynta_parser_t base;

    cynta_parser_t *parser;
} cynta_try_t;

cynta_parser_error_t cynta_try_init(cynta_try_t *, cynta_parser_t *);

static inline cynta_parser_t *cynta_try(cynta_parser_t *parser) {
    static cynta_try_t pool[CYNTA_GLOBAL_POOL_TRY_CAPACITY];
    static size_t pool_index = 0;

    if (pool_index >= CYNTA_GLOBAL_POOL_TRY_CAPACITY) {
        return NULL;
    }
    if (cynta_try_init(&pool[pool_index], parser) != CYNTA_PARSER_SUCCESS) {
        return NULL;
    }
    return (cynta_parser_t *)&pool[pool_index++];
}

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

cynta_parser_error_t cynta_internal_choice_init(cynta_choice_t *, size_t,
                                                va_list);
cynta_parser_error_t cynta_choice_init(cynta_choice_t *, size_t, ...);

static inline cynta_parser_t *cynta_choice(size_t size, ...) {
    static cynta_choice_t pool[CYNTA_GLOBAL_POOL_CHOICE_CAPACITY];
    static size_t pool_index = 0;

    if (pool_index >= CYNTA_GLOBAL_POOL_CHOICE_CAPACITY) {
        return NULL;
    }
    va_list args;
    va_start(args, size);
    if (cynta_internal_choice_init(&pool[pool_index], size, args) !=
        CYNTA_PARSER_SUCCESS) {
        return NULL;
    }
    va_end(args);
    return (cynta_parser_t *)&pool[pool_index++];
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_H */