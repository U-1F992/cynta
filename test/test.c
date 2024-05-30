#include <cynta.h>

#include <stdio.h>

#include "test_stream.h"

#define TEST_TEXT_RED "\e[31m"
#define TEST_TEXT_GREEN "\e[32m"
#define TEST_TEXT_RESET "\e[0m"
#define SIZE(array) (sizeof((array)) / sizeof((array)[0]))

void test_stream(void)
{
    uint8_t origin[] = {0, 1, 2, 3, 4, 5, 6, 7};
    size_t origin_size = sizeof(origin) / sizeof(origin[0]);

    array_stream_t stream;
    assert(CYNTA_STREAM_SUCCESS == array_stream_init(&stream, origin, origin_size));

    uint8_t out;

    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 0);

    cynta_stream_push_checkpoint((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 1);
    cynta_stream_rewind((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 1);

    cynta_stream_push_checkpoint((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 2);
    cynta_stream_push_checkpoint((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 3);
    cynta_stream_rewind((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 3);
    cynta_stream_rewind((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 2);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 3);

    cynta_stream_push_checkpoint((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 4);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 5);
    cynta_stream_push_checkpoint((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 6);
    cynta_stream_discard_checkpoint((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 7);
    cynta_stream_rewind((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 4);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 5);
    cynta_stream_push_checkpoint((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 6);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 7);
    cynta_stream_rewind((cynta_stream_t *)&stream);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 6);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 7);

    assert(CYNTA_STREAM_ERROR_END_OF_STREAM == cynta_stream_next((cynta_stream_t *)&stream, &out));
}

void test_parser_uint8(cynta_parser_t *parser,
                       uint8_t test_case[], size_t test_case_size,
                       cynta_parser_error_t expected,
                       uint8_t expected_out[], size_t expected_out_size)
{
    printf("* %s\n", __func__);
    printf(" uint8_t test_case[%d] = {", test_case_size);
    for (size_t i = 0; i < test_case_size; i++)
    {
        printf("%d", test_case[i]);
        if (i < test_case_size - 1)
        {
            printf(", ");
        }
    }
    printf("};\n");
    printf(" cynta_parser_error_t expected = %s;\n", CYNTA_PARSER_ERROR(expected));
    printf(" uint8_t expected_out[%d] = {", expected_out_size);
    for (size_t i = 0; i < expected_out_size; i++)
    {
        printf("%d", expected_out[i]);
        if (i < expected_out_size - 1)
        {
            printf(", ");
        }
    }
    printf("};\n");

    int test_failure = 0;

    array_stream_t stream;
    assert(CYNTA_STREAM_SUCCESS == array_stream_init(&stream, test_case, test_case_size));

    void *actual_out;
    size_t actual_out_size;

    cynta_parser_error_t actual = cynta_parser_apply(parser, (cynta_stream_t *)&stream, &actual_out, &actual_out_size);
    if (expected != actual)
    {
        fprintf(stderr,
                "%sexpected: %s, actual: %s%s\n",
                TEST_TEXT_RED, CYNTA_PARSER_ERROR(expected), CYNTA_PARSER_ERROR(actual), TEST_TEXT_RESET);
        test_failure++;
        goto cleanup_parser_stream;
    }
    if (actual != CYNTA_PARSER_SUCCESS)
    {
        goto cleanup_parser_stream;
    }

    if (expected_out_size != actual_out_size)
    {
        fprintf(stderr,
                "%sexpected_out_size: %d, actual_out_size: %d%s\n",
                TEST_TEXT_RED, expected_out_size, actual_out_size, TEST_TEXT_RESET);
        test_failure++;
        goto cleanup_out;
    }

    uint8_t *actual_out_arr = (uint8_t *)actual_out;
    for (size_t i = 0; i < actual_out_size; i++)
    {
        if (expected_out[i] != actual_out_arr[i])
        {
            fprintf(stderr,
                    "%sexpected_out[%d]: %d, actual_out[%d]: %d%s\n",
                    TEST_TEXT_RED, i, expected_out[i], i, actual_out_arr[i], TEST_TEXT_RESET);
            test_failure++;
            goto cleanup_out;
        }
    }

cleanup_out:
    free(actual_out);

cleanup_parser_stream:
    cynta_parser_delete(parser);

    if (!test_failure)
    {
        printf("%sOK%s\n", TEST_TEXT_GREEN, TEST_TEXT_RESET);
    }
    printf("\n");
}

int main(void)
{
    test_stream();

    uint8_t test_case_0[] = {128};
    uint8_t expected_out_0[] = {128};
    test_parser_uint8(cynta_any(),
                      test_case_0, SIZE(test_case_0),
                      CYNTA_PARSER_SUCCESS,
                      expected_out_0, SIZE(expected_out_0));

    uint8_t test_case_1[] = {127, 128};
    uint8_t expected_out_1[] = {127};
    test_parser_uint8(cynta_any(),
                      test_case_1, SIZE(test_case_1),
                      CYNTA_PARSER_SUCCESS,
                      expected_out_1, SIZE(expected_out_1));

    uint8_t test_case_2[] = {128};
    uint8_t expected_out_2[] = {128};
    test_parser_uint8(cynta_value(128),
                      test_case_2, SIZE(test_case_2),
                      CYNTA_PARSER_SUCCESS,
                      expected_out_2, SIZE(expected_out_2));

    uint8_t test_case_3[] = {127};
    test_parser_uint8(cynta_value(128),
                      test_case_3, SIZE(test_case_3),
                      CYNTA_PARSER_ERROR_UNEXPECTED_VALUE,
                      NULL, 0);

    uint8_t test_case_4[] = {127, 128};
    uint8_t expected_out_4[] = {127, 128};
    test_parser_uint8(cynta_sequence(cynta_value(127),
                                     cynta_value(128)),
                      test_case_4, SIZE(test_case_4),
                      CYNTA_PARSER_SUCCESS,
                      expected_out_4, SIZE(expected_out_4));

    test_parser_uint8(cynta_sequence(cynta_any(),
                                     cynta_value(128)),
                      test_case_4, SIZE(test_case_4),
                      CYNTA_PARSER_SUCCESS,
                      expected_out_4, SIZE(expected_out_4));

    uint8_t test_case_5[] = {127, 129};
    test_parser_uint8(cynta_sequence(cynta_any(),
                                     cynta_value(128)),
                      test_case_5, SIZE(test_case_5),
                      CYNTA_PARSER_ERROR_UNEXPECTED_VALUE,
                      NULL, 0);

    uint8_t test_case_6[] = {128, 128, 128, 128};
    uint8_t expected_out_6[] = {128, 128, 128, 128};
    test_parser_uint8(cynta_repeat(cynta_value(128), 4),
                      test_case_6, SIZE(test_case_6),
                      CYNTA_PARSER_SUCCESS,
                      expected_out_6, SIZE(expected_out_6));

    uint8_t test_case_7[] = {128, 128, 129, 128};
    test_parser_uint8(cynta_repeat(cynta_value(128), 4),
                      test_case_7, SIZE(test_case_7),
                      CYNTA_PARSER_ERROR_UNEXPECTED_VALUE,
                      NULL, 0);

    uint8_t test_case_8[] = {128, 128, 128, 128};
    uint8_t expected_out_8[] = {128, 128, 128, 128};
    test_parser_uint8(cynta_many(cynta_value(128)),
                      test_case_8, SIZE(test_case_8),
                      CYNTA_PARSER_SUCCESS,
                      expected_out_8, SIZE(expected_out_8));

    uint8_t test_case_9[] = {129, 128, 128, 128};
    uint8_t expected_out_9[1]; // To suppress error
    test_parser_uint8(cynta_many(cynta_value(128)),
                      test_case_9, SIZE(test_case_9),
                      CYNTA_PARSER_SUCCESS,
                      expected_out_9, 0);

    uint8_t test_case_10[] = {0xAB, 128, 128, 128, 129};
    uint8_t expected_out_10[] = {0xAB, 128, 128, 128, 129};
    test_parser_uint8(cynta_sequence(cynta_value(0xAB),
                                     cynta_sequence(cynta_many(cynta_value(128)),
                                                    cynta_value(129))),
                      test_case_10, SIZE(test_case_10),
                      CYNTA_PARSER_SUCCESS,
                      expected_out_10, SIZE(expected_out_10));

    return 0;
}