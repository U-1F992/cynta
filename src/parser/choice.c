#include <cynta.h>

#if 0 < CYNTA_GLOBAL_POOL_CHOICE_CAPACITY

#include <stdarg.h>

cynta_choice_t __cynta_global_pool_choice[CYNTA_GLOBAL_POOL_CHOICE_CAPACITY];
size_t __cynta_global_pool_choice_index = 0;

static cynta_parser_error_t choice_apply(cynta_parser_t *base, cynta_stream_t *stream, void *out)
{
    cynta_choice_t *self = (cynta_choice_t *)base;
    if (self == NULL ||
        stream == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    cynta_parser_error_t err = CYNTA_PARSER_ERROR_UNEXPECTED_VALUE; // Default error if no parser succeeds

    for (size_t i = 0; i < self->parsers_size; i++)
    {
        cynta_stream_push_checkpoint(stream);
        err = cynta_parser_apply(self->parsers[i], stream, out);
        if (err == CYNTA_PARSER_SUCCESS)
        {
            cynta_stream_discard_checkpoint(stream);
            return CYNTA_PARSER_SUCCESS;
        }
        else
        {
            cynta_stream_rewind(stream);
        }
    }

    return err; // Return the last error encountered
}

cynta_parser_error_t cynta_choice_init(cynta_choice_t *self, size_t size, ...)
{
    if (self == NULL)
    {
        return CYNTA_PARSER_ERROR_NULL_POINTER;
    }

    if (size < 1 || CYNTA_CHOICE_VA_ARGS_CAPACITY < size)
    {
        return CYNTA_PARSER_ERROR_OUT_OF_CAPACITY;
    }

    self->base.apply = choice_apply;
    va_list ap;
    va_start(ap, size);
    for (size_t i = 0; i < size; i++)
    {
        if ((self->parsers[i] = va_arg(ap, cynta_parser_t *)) == NULL)
        {
            va_end(ap);
            return CYNTA_PARSER_ERROR_NULL_POINTER;
        }
    }
    va_end(ap);
    self->parsers_size = size;

    return CYNTA_PARSER_SUCCESS;
}

#endif