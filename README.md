# Cynta（心太）

A parser combinator library.

This library has achieved the elimination of dynamic memory allocation (e.g., `malloc`, `realloc`, `free`). It aims to enhance performance, prevent memory leaks, and increase the predictability of memory usage, particularly in resource-constrained environments such as microcontrollers.

We use macros to adjust memory usage settings to sizes that are optimal for the application. This approach requires careful consideration of the number of parsers in use simultaneously and the potential inputs and outputs for each parser. In microcontroller applications, although parsers may handle dynamic data, the parsers themselves are typically not dynamically generated during runtime but are instead statically integrated during application compilation, aligning well with this design.

## `void *` handling

We introduce a generics-like notation for explanation. `parser_t<T>` is assumed to internally cast the `void *out` argument of `parser_apply` to `T *out` and set the value. The caller declares a variable of type `T` and supplies its reference to `parser_apply`.

Furthermore, we prepare a helper structure. By `typedef struct uint8_array_t { uint8_t data[UINT8_ARRAY_CAPACITY]; size_t size; } uint8_array_t;`, an array and its size are shared through a single reference, `void *out`.

`satisfy` and its derivatives `any` and `value` are `parser_t<uint8_array_t>` with `size` being fixed to 1. Meanwhile, `sequence`, `repeat`, and `many` are `parser_t<uint8_array_t>` that take `parser_t<uint8_array_t>` at initialization, where `size` varies but will result in an error if it exceeds `UINT8_ARRAY_CAPACITY`.

On the other hand, `choice` and `try` are `parser_t<T>` that accept `parser_t<T>` at initialization. Although the utility of being polymorphic is limited in this context as nearly all bundled parsers are `parser_t<uint8_array_t>`, the capability to directly pass custom parsers to `choice` and `try` when implemented is crucial.
