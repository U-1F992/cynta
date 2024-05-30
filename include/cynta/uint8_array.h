#ifndef CYNTA_UINT8_ARRAY_H
#define CYNTA_UINT8_ARRAY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>

#ifndef CYNTA_UINT8_ARRAY_CAPACITY
#define CYNTA_UINT8_ARRAY_CAPACITY 256
#endif

    typedef struct cynta_uint8_array_t
    {
        uint8_t data[CYNTA_UINT8_ARRAY_CAPACITY];
        size_t size;
    } cynta_uint8_array_t;

#ifdef __cplusplus
}
#endif

#endif /* CYNTA_UINT8_ARRAY_H */