#include <munit/munit.h>

#include <access_points_data.h>

MunitResult create_delete_map_test(const MunitParameter /*params*/[], void* /*user_data_or_fixture*/)
{
    access_point_map* new_map = create_access_point_map();

    munit_assert_ptr_not_null(new_map);

    delete_access_point_map(new_map);

    return MUNIT_OK;
}

MunitResult add_to_map_test(const MunitParameter /*params*/[], void* /*user_data_or_fixture*/)
{
    access_point ap = {.ssid = "some_ssid",
                       .channel = 1u,
                       .SNR = 10u};

    access_point_map* new_map = create_access_point_map();

    // newly created map is emtpy
    munit_assert_size(map_size(new_map), ==, 0u);

    add_to_map(new_map, &ap);

    munit_assert_size(map_size(new_map), ==, 1u);

    delete_access_point_map(new_map);

    return MUNIT_OK;
}

MunitResult find_in_map_test(const MunitParameter /*params*/[], void* /*user_data_or_fixture*/)
{
    access_point ap = {.ssid = "some_ssid",
                       .channel = 1u,
                       .SNR = 10u};

    access_point_map* new_map = create_access_point_map();

    // entry is not found before addition
    munit_assert_int(find_ssid_in_map(new_map, ap.ssid), ==, -1);

    add_to_map(new_map, &ap);

    // entry is found after addition
    munit_assert_int(find_ssid_in_map(new_map, ap.ssid), !=, -1);

    delete_access_point_map(new_map);

    return MUNIT_OK;
}

MunitTest access_points_data_tests[] =
    {
        {
            "/create_delete_map_test", // name
            create_delete_map_test,    // test
            NULL,                      //  setup
            NULL,                      //  tear_down
            MUNIT_TEST_OPTION_NONE,    // options
            NULL                       // parameters
        },
        {"/add_to_map_test",
         add_to_map_test,
         NULL,
         NULL,
         MUNIT_TEST_OPTION_NONE,
         NULL},
        {"/find_in_map_test",
         find_in_map_test,
         NULL,
         NULL,
         MUNIT_TEST_OPTION_NONE,
         NULL},

        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}}; // test array terminator

static const MunitSuite suite = {
    "/access_points_data_tests", // name
    access_points_data_tests,    // tests
    NULL,                        // suites
    1,                           // iterations
    MUNIT_SUITE_OPTION_NONE      // options
};

int main(int argc, char** argv)
{
    return munit_suite_main(&suite, NULL, argc, argv);
}
