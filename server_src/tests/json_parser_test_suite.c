#include <access_points_data.h>
#include <json_parser.h>

#include <utils.h>

#include <munit/munit.h>

static MunitResult json_parse_invalid_buffer_test(const MunitParameter UNUSED(params[]), void* UNUSED(user_data_or_fixture))
{
    json_data_buffer empty = {.data = NULL, .size = 0};
    munit_assert_null(parse_json_from_buffer(empty));
    return MUNIT_OK;
}

static MunitResult json_parse_invalid_json_test(const MunitParameter UNUSED(params[]), void* UNUSED(user_data_or_fixture))
{
    json_data_buffer empty = {.data = "not//valid//json", .size = 10};
    munit_assert_null(parse_json_from_buffer(empty));
    return MUNIT_OK;
}

// todo: more invalid json tests

static MunitResult json_parse_valid_json_test(const MunitParameter UNUSED(params[]), void* UNUSED(user_data_or_fixture))
{
    const char json[100] =
        "{\"access_points\": [{\"ssid\": \"MyAP\",\"snr\": 63,\"channel\": 11}]}";

    json_data_buffer one_entry = {.data = json, .size = 100l};

    access_point_map* map = parse_json_from_buffer(one_entry);

    munit_assert_not_null(map);
    munit_assert_size(map_size(map), ==, 1u);

    munit_assert_string_equal(map[0].key, "MyAP");
    munit_assert_string_equal(map[0].value.ssid, "MyAP");
    munit_assert_uint(map[0].value.SNR, ==, 63u);
    munit_assert_uint(map[0].value.channel, ==, 11u);

    delete_access_point_map(map);

    return MUNIT_OK;
}

// todo: more valid json tests

static MunitTest json_parser_tests[] =
    {
        {.name = "/json_parse_invalid_buffer_test",
         .test = json_parse_invalid_buffer_test,
         .setup = NULL,
         .tear_down = NULL,
         .options = MUNIT_TEST_OPTION_NONE,
         .parameters = NULL},

        {.name = "/json_parse_invalid_json_test",
         .test = json_parse_invalid_json_test,
         .setup = NULL,
         .tear_down = NULL,
         .options = MUNIT_TEST_OPTION_NONE,
         .parameters = NULL},
        {.name = "/json_parse_valid_json_test",
         .test = json_parse_valid_json_test,
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
