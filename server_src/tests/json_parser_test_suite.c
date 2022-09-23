#include <munit/munit.h>

#include <utils.h>

static MunitResult json_test(const MunitParameter UNUSED(params[]), void* UNUSED(user_data_or_fixture))
{
    return MUNIT_OK;
}

static MunitTest json_parser_tests[] =
    {
        {.name = "/json_test",
         .test = json_test,
         .setup = NULL,
         .tear_down = NULL,
         .options = MUNIT_TEST_OPTION_NONE,
         .parameters = NULL},

        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}}; // test array terminator

const MunitSuite json_parser_test_suite = {
    .prefix = "",
    .tests = json_parser_tests,
    .suites = NULL,
    .iterations = 1,
    .options = MUNIT_SUITE_OPTION_NONE};
