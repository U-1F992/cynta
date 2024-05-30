# Cynta（心太）

A parser combinator library.

This library has achieved the elimination of dynamic memory allocation (e.g., `malloc`, `realloc`, `free`). It aims to enhance performance, prevent memory leaks, and increase the predictability of memory usage, particularly in resource-constrained environments such as microcontrollers.

We use macros to adjust memory usage settings to sizes that are optimal for the application. This approach requires careful consideration of the number of parsers in use simultaneously and the potential inputs and outputs for each parser. In microcontroller applications, although parsers may handle dynamic data, the parsers themselves are typically not dynamically generated during runtime but are instead statically integrated during application compilation, aligning well with this design.

## `void *` handling

We introduce a generics-like notation for explanation. `cynta_parser_t<T>` is assumed to internally cast the `void *out` argument of `cynta_parser_apply` to `T *` and set the value. The caller declares a variable of type `T` and supplies its reference to `cynta_parser_apply`.

Furthermore, we prepare a helper structure. By `typedef struct cynta_uint8_array_t { uint8_t data[UINT8_ARRAY_CAPACITY]; size_t size; } cynta_uint8_array_t;`, an array and its size are shared through a single reference, `void *out`.

`cynta_satisfy` and its derivatives `cynta_any` and `cynta_value` are `cynta_parser_t<cynta_uint8_array_t>` with `size` being fixed to 1. Meanwhile, `cynta_sequence`, `cynta_repeat`, and `cynta_many` are `cynta_parser_t<cynta_uint8_array_t>` that take `cynta_parser_t<cynta_uint8_array_t>` at initialization, where `size` varies but will result in an error if it exceeds `CYNTA_UINT8_ARRAY_CAPACITY`.

On the other hand, `cynta_choice` and `cynta_try` are `cynta_parser_t<T>` that accept `cynta_parser_t<T>` at initialization. Although the utility of being polymorphic is limited in this context as nearly all bundled parsers are `cynta_parser_t<cynta_uint8_array_t>`, the capability to directly pass custom parsers to `cynta_choice` and `cynta_try` is crucial.
