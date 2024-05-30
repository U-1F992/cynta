#include <cynta.h>

#include <stdio.h>

#include "test.h"
#include "test_array_stream.h"

static void test_stream(void)
{
    printf("* %s\n", __func__);

    uint8_t origin[] = {0, 1, 2, 3, 4, 5, 6, 7};
    size_t origin_size = sizeof(origin) / sizeof(origin[0]);

    test_array_stream_t stream;
    assert(CYNTA_STREAM_SUCCESS == test_array_stream_init(&stream, origin, origin_size));

    uint8_t out;

    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 0);

    assert(CYNTA_STREAM_SUCCESS == cynta_stream_push_checkpoint((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 1);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_rewind((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 1);

    assert(CYNTA_STREAM_SUCCESS == cynta_stream_push_checkpoint((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 2);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_push_checkpoint((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 3);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_rewind((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 3);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_rewind((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 2);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 3);

    assert(CYNTA_STREAM_SUCCESS == cynta_stream_push_checkpoint((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 4);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 5);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_push_checkpoint((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 6);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_discard_checkpoint((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 7);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_rewind((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 4);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 5);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_push_checkpoint((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 6);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 7);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_rewind((cynta_stream_t *)&stream));
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 6);
    assert(CYNTA_STREAM_SUCCESS == cynta_stream_next((cynta_stream_t *)&stream, &out) && out == 7);

    assert(CYNTA_STREAM_ERROR_END_OF_STREAM == cynta_stream_next((cynta_stream_t *)&stream, &out));
}

static bool test_parser_helper(size_t index,
                               cynta_parser_t *parser,
                               uint8_t stream[], size_t stream_size,
                               cynta_parser_error_t expected,
                               uint8_t expected_out[], size_t expected_out_size)
{
    test_array_stream_t test_stream;
    assert(CYNTA_STREAM_SUCCESS == test_array_stream_init(&test_stream, stream, stream_size));

    cynta_uint8_array_t actual_out;
    cynta_parser_error_t actual = cynta_parser_apply(parser, (cynta_stream_t *)&test_stream, (void *)&actual_out);
    if (expected != actual)
    {
        fprintf(stderr,
                "%sindex: %d, expected: %s, actual: %s%s\n",
                TEST_TEXT_RED, index, CYNTA_PARSER_ERROR(expected), CYNTA_PARSER_ERROR(actual), TEST_TEXT_RESET);
        return false;
    }
    if (actual != CYNTA_PARSER_SUCCESS)
    {
        return true;
    }

    if (expected_out_size != actual_out.size)
    {
        fprintf(stderr,
                "%sindex: %d, expected_out_size: %d, actual_out_size: %d%s\n",
                TEST_TEXT_RED, index, expected_out_size, actual_out.size, TEST_TEXT_RESET);
        return false;
    }

    bool failure = false;
    for (size_t i = 0; i < actual_out.size; i++)
    {
        if (expected_out[i] != actual_out.data[i])
        {
            fprintf(stderr,
                    "%sindex: %d, expected_out[%d]: %d, actual_out[%d]: %d%s\n",
                    TEST_TEXT_RED, index, i, expected_out[i], i, actual_out.data[i], TEST_TEXT_RESET);
            failure = true;
        }
        if (failure)
        {
            return false;
        }
    }

    return true;
}

static size_t test_any(void)
{
    printf("* %s\n", __func__);

    typedef struct test_case_t
    {
        cynta_parser_t *parser;
        uint8_t *stream;
        size_t stream_size;

        cynta_parser_error_t expected;
        uint8_t *expected_out;
        size_t expected_out_size;
    } test_case_t;

    test_case_t test_cases[] = {{.parser = cynta_any(),
                                 .stream = (uint8_t[]){128},
                                 .stream_size = 1,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){128},
                                 .expected_out_size = 1},
                                {.parser = cynta_any(),
                                 .stream = (uint8_t[]){127, 128},
                                 .stream_size = 2,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){127},
                                 .expected_out_size = 1}};
    size_t test_cases_size = TEST_SIZEOF(test_cases);
    size_t test_failure_count = 0;

    for (size_t i = 0; i < test_cases_size; i++)
    {
        test_case_t *test_case = &test_cases[i];
        if (!test_parser_helper(i,
                                test_case->parser,
                                test_case->stream,
                                test_case->stream_size,
                                test_case->expected,
                                test_case->expected_out,
                                test_case->expected_out_size))
        {
            test_failure_count++;
        }
    }

    return test_failure_count;
}

static size_t test_value(void)
{
    printf("* %s\n", __func__);

    typedef struct test_case_t
    {
        cynta_parser_t *parser;
        uint8_t *stream;
        size_t stream_size;

        cynta_parser_error_t expected;
        uint8_t *expected_out;
        size_t expected_out_size;
    } test_case_t;

    test_case_t test_cases[] = {{.parser = cynta_value(128),
                                 .stream = (uint8_t[]){128},
                                 .stream_size = 1,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){128},
                                 .expected_out_size = 1},
                                {.parser = cynta_value(128),
                                 .stream = (uint8_t[]){0},
                                 .stream_size = 1,
                                 .expected = CYNTA_PARSER_ERROR_UNEXPECTED_VALUE,
                                 .expected_out = NULL,
                                 .expected_out_size = 0}};
    size_t test_cases_size = TEST_SIZEOF(test_cases);
    size_t test_failure_count = 0;

    for (size_t i = 0; i < test_cases_size; i++)
    {
        test_case_t *test_case = &test_cases[i];
        if (!test_parser_helper(i,
                                test_case->parser,
                                test_case->stream,
                                test_case->stream_size,
                                test_case->expected,
                                test_case->expected_out,
                                test_case->expected_out_size))
        {
            test_failure_count++;
        }
    }

    return test_failure_count;
}

static size_t test_sequence(void)
{
    printf("* %s\n", __func__);

    typedef struct test_case_t
    {
        cynta_parser_t *parser;
        uint8_t *stream;
        size_t stream_size;

        cynta_parser_error_t expected;
        uint8_t *expected_out;
        size_t expected_out_size;
    } test_case_t;

    test_case_t test_cases[] = {{.parser = cynta_sequence(2,
                                                          cynta_value(127),
                                                          cynta_value(128)),
                                 .stream = (uint8_t[]){127, 128},
                                 .stream_size = 2,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){127, 128},
                                 .expected_out_size = 2},
                                {.parser = cynta_sequence(2,
                                                          cynta_any(),
                                                          cynta_value(128)),
                                 .stream = (uint8_t[]){127, 128},
                                 .stream_size = 2,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){127, 128},
                                 .expected_out_size = 2},
                                {.parser = cynta_sequence(2,
                                                          cynta_any(),
                                                          cynta_value(128)),
                                 .stream = (uint8_t[]){127, 129},
                                 .stream_size = 2,
                                 .expected = CYNTA_PARSER_ERROR_UNEXPECTED_VALUE,
                                 .expected_out = NULL,
                                 .expected_out_size = 0}};
    size_t test_cases_size = TEST_SIZEOF(test_cases);
    size_t test_failure_count = 0;

    for (size_t i = 0; i < test_cases_size; i++)
    {
        test_case_t *test_case = &test_cases[i];
        if (!test_parser_helper(i,
                                test_case->parser,
                                test_case->stream,
                                test_case->stream_size,
                                test_case->expected,
                                test_case->expected_out,
                                test_case->expected_out_size))
        {
            test_failure_count++;
        }
    }

    return test_failure_count;
}

static size_t test_repeat(void)
{
    printf("* %s\n", __func__);

    typedef struct test_case_t
    {
        cynta_parser_t *parser;
        uint8_t *stream;
        size_t stream_size;

        cynta_parser_error_t expected;
        uint8_t *expected_out;
        size_t expected_out_size;
    } test_case_t;

    test_case_t test_cases[] = {{.parser = cynta_repeat(cynta_value(128), 4),
                                 .stream = (uint8_t[]){128, 128, 128, 128},
                                 .stream_size = 4,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){128, 128, 128, 128},
                                 .expected_out_size = 4},
                                {.parser = cynta_repeat(cynta_value(128), 4),
                                 .stream = (uint8_t[]){128, 128, 129, 128},
                                 .stream_size = 4,
                                 .expected = CYNTA_PARSER_ERROR_UNEXPECTED_VALUE,
                                 .expected_out = NULL,
                                 .expected_out_size = 0}};
    size_t test_cases_size = TEST_SIZEOF(test_cases);
    size_t test_failure_count = 0;

    for (size_t i = 0; i < test_cases_size; i++)
    {
        test_case_t *test_case = &test_cases[i];
        if (!test_parser_helper(i,
                                test_case->parser,
                                test_case->stream,
                                test_case->stream_size,
                                test_case->expected,
                                test_case->expected_out,
                                test_case->expected_out_size))
        {
            test_failure_count++;
        }
    }

    return test_failure_count;
}

static size_t test_many(void)
{
    printf("* %s\n", __func__);

    typedef struct test_case_t
    {
        cynta_parser_t *parser;
        uint8_t *stream;
        size_t stream_size;

        cynta_parser_error_t expected;
        uint8_t *expected_out;
        size_t expected_out_size;
    } test_case_t;

    test_case_t test_cases[] = {{.parser = cynta_many(cynta_value(128)),
                                 .stream = (uint8_t[]){128, 128, 128, 128},
                                 .stream_size = 4,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){128, 128, 128, 128},
                                 .expected_out_size = 4},
                                {.parser = cynta_many(cynta_value(128)),
                                 .stream = (uint8_t[]){128, 128, 129, 128},
                                 .stream_size = 4,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){128, 128},
                                 .expected_out_size = 2},
                                {.parser = cynta_many(cynta_value(128)),
                                 .stream = (uint8_t[]){129, 128, 128, 128},
                                 .stream_size = 4,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){0xFF}, // dummy
                                 .expected_out_size = 0}};
    size_t test_cases_size = TEST_SIZEOF(test_cases);
    size_t test_failure_count = 0;

    for (size_t i = 0; i < test_cases_size; i++)
    {
        test_case_t *test_case = &test_cases[i];
        if (!test_parser_helper(i,
                                test_case->parser,
                                test_case->stream,
                                test_case->stream_size,
                                test_case->expected,
                                test_case->expected_out,
                                test_case->expected_out_size))
        {
            test_failure_count++;
        }
    }

    return test_failure_count;
}

static size_t test_choice(void)
{
    printf("* %s\n", __func__);

    typedef struct test_case_t
    {
        cynta_parser_t *parser;
        uint8_t *stream;
        size_t stream_size;

        cynta_parser_error_t expected;
        uint8_t *expected_out;
        size_t expected_out_size;
    } test_case_t;

    test_case_t test_cases[] = {{.parser = cynta_choice(3,
                                                        cynta_value(0xAB),
                                                        cynta_value(0xCD),
                                                        cynta_value(0xEF)),
                                 .stream = (uint8_t[]){0xAB},
                                 .stream_size = 1,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){0xAB},
                                 .expected_out_size = 1},
                                {.parser = cynta_choice(3,
                                                        cynta_value(0xAB),
                                                        cynta_value(0xCD),
                                                        cynta_value(0xEF)),
                                 .stream = (uint8_t[]){0xCD},
                                 .stream_size = 1,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){0xCD},
                                 .expected_out_size = 1},
                                {.parser = cynta_choice(3,
                                                        cynta_value(0xAB),
                                                        cynta_value(0xCD),
                                                        cynta_value(0xEF)),
                                 .stream = (uint8_t[]){0xEF},
                                 .stream_size = 1,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){0xEF},
                                 .expected_out_size = 1},
                                {.parser = cynta_choice(3,
                                                        cynta_value(0xAB),
                                                        cynta_value(0xCD),
                                                        cynta_value(0xEF)),
                                 .stream = (uint8_t[]){0xAA}, // no match
                                 .stream_size = 1,
                                 .expected = CYNTA_PARSER_ERROR_UNEXPECTED_VALUE,
                                 .expected_out = NULL,
                                 .expected_out_size = 0}};
    size_t test_cases_size = TEST_SIZEOF(test_cases);
    size_t test_failure_count = 0;

    for (size_t i = 0; i < test_cases_size; i++)
    {
        test_case_t *test_case = &test_cases[i];
        if (!test_parser_helper(i,
                                test_case->parser,
                                test_case->stream,
                                test_case->stream_size,
                                test_case->expected,
                                test_case->expected_out,
                                test_case->expected_out_size))
        {
            test_failure_count++;
        }
    }

    return test_failure_count;
}

static size_t test_complex_examples(void)
{
    printf("* %s\n", __func__);

    typedef struct test_case_t
    {
        cynta_parser_t *parser;
        uint8_t *stream;
        size_t stream_size;

        cynta_parser_error_t expected;
        uint8_t *expected_out;
        size_t expected_out_size;
    } test_case_t;

    test_case_t test_cases[] = {{.parser = cynta_sequence(3,
                                                          cynta_value(0xAB),
                                                          cynta_many(cynta_value(128)),
                                                          cynta_value(129)),
                                 .stream = (uint8_t[]){0xAB, 128, 128, 128, 129},
                                 .stream_size = 5,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){0xAB, 128, 128, 128, 129},
                                 .expected_out_size = 5},
                                {.parser = cynta_choice(2, // backtracing
                                                        cynta_sequence(2, cynta_value(0xAB), cynta_value(0xCD)),
                                                        cynta_sequence(2, cynta_value(0xAB), cynta_value(0xEF))),
                                 .stream = (uint8_t[]){0xAB, 0xEF},
                                 .stream_size = 2,
                                 .expected = CYNTA_PARSER_SUCCESS,
                                 .expected_out = (uint8_t[]){0xAB, 0xEF},
                                 .expected_out_size = 2}};
    size_t test_cases_size = TEST_SIZEOF(test_cases);
    size_t test_failure_count = 0;

    for (size_t i = 0; i < test_cases_size; i++)
    {
        test_case_t *test_case = &test_cases[i];
        if (!test_parser_helper(i,
                                test_case->parser,
                                test_case->stream,
                                test_case->stream_size,
                                test_case->expected,
                                test_case->expected_out,
                                test_case->expected_out_size))
        {
            test_failure_count++;
        }
    }

    return test_failure_count;
}

int main(void)
{
    size_t test_failure_count = 0;

    test_stream();

    test_failure_count += test_any();
    test_failure_count += test_value();
    test_failure_count += test_sequence();
    test_failure_count += test_repeat();
    test_failure_count += test_many();
    test_failure_count += test_choice();
    test_failure_count += test_complex_examples();

    if (test_failure_count == 0)
    {
        printf("%sOK%s\n", TEST_TEXT_GREEN, TEST_TEXT_RESET);
        return 0;
    }
    else
    {
        fprintf(stderr, "%s%d errors found%s\n", TEST_TEXT_RED, test_failure_count, TEST_TEXT_RESET);
        return 1;
    }
}