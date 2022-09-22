#include <munit/munit.h>

#include <access_points_data.h>

#define SETUP_SSID "some_ssid"

static void* create_map_with_entry(const MunitParameter /*params*/[], void* /*user_data*/)
{
    access_point_map* new_map = create_access_point_map();

    access_point ap = {.ssid = SETUP_SSID,
                       .channel = 1u,
                       .SNR = 10u};

    add_to_map(new_map, &ap);

    return new_map;
}

static void delete_map(void* map)
{
    delete_access_point_map(map);
}

MunitResult create_delete_map_test(const MunitParameter /*params*/[], void* /*user_data_or_fixture*/)
{
    access_point_map* new_map = create_access_point_map();

    munit_assert_ptr_not_null(new_map);

    delete_access_point_map(new_map);

    return MUNIT_OK;
}

MunitResult set_and_get_map_test(const MunitParameter /*params*/[], void* /*user_data_or_fixture*/)
{
    access_point_map* new_map = create_access_point_map();

    set_current_access_points(new_map);

    // recently set access point map is returned
    munit_assert_ptr_equal(get_current_access_points(), new_map);

    delete_access_point_map(new_map);

    return MUNIT_OK;
}

MunitResult add_to_map_test(const MunitParameter /*params*/[], void* /*user_data_or_fixture*/)
{

    access_point_map* new_map = create_access_point_map();

    // newly created map is emtpy
    munit_assert_size(map_size(new_map), ==, 0u);

    access_point ap = {.ssid = "some_ssid",
                       .channel = 1u,
                       .SNR = 10u};

    add_to_map(new_map, &ap);
    // one item added
    munit_assert_size(map_size(new_map), ==, 1u);

    access_point ap2 = {.ssid = "other_ssid",
                        .channel = 1u,
                        .SNR = 10u};

    add_to_map(new_map, &ap2);
    // two items added
    munit_assert_size(map_size(new_map), ==, 2u);

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

MunitResult detect_changed_ssid_test(const MunitParameter /*params*/[], void* fixture)
{
    access_point_map* map = fixture;

    int old_ap_index = find_ssid_in_map(map, SETUP_SSID);
    access_point* old_ap = &(map[old_ap_index].value);
    access_point channel_changed_ap = *old_ap;

    // no change
    list_change change = detect_changed_ssid(map, old_ap);
    munit_assert(change.change_type == NO_CHANGE);

    // channel change
    ++channel_changed_ap.channel;
    change = detect_changed_ssid(map, &channel_changed_ap);

    munit_assert(change.change_type == PARAM_CHANGE);
    munit_assert_int(change.changed_ssid_index, ==, old_ap_index);

    access_point snr_changed_ap = *old_ap;

    // SNR change
    ++snr_changed_ap.SNR;
    change = detect_changed_ssid(map, &snr_changed_ap);

    munit_assert(change.change_type == PARAM_CHANGE);
    munit_assert_int(change.changed_ssid_index, ==, old_ap_index);

    // New ssid
    access_point new_ssid = *old_ap;
    ++new_ssid.ssid[0];

    change = detect_changed_ssid(map, &new_ssid);
    munit_assert(change.change_type == NEW_SSID);

    return MUNIT_OK;
}

MunitResult detect_param_changes_test(const MunitParameter /*params*/[], void* /*fixture*/)
{
    access_point lhs = {.ssid = "some_ssid",
                        .channel = 1u,
                        .SNR = 10u};

    access_point rhs = {.ssid = "some_ssid",
                        .channel = 1u,
                        .SNR = 10u};

    param_changes changes = detect_param_changes(&lhs, &rhs);

    // no difference
    munit_assert_false(changes.channel_change);
    munit_assert_false(changes.SNR_change);

    //  difference in SNR only
    ++rhs.SNR;
    changes = detect_param_changes(&lhs, &rhs);
    munit_assert_false(changes.channel_change);
    munit_assert_true(changes.SNR_change);

    //  difference in channel & SNR
    ++rhs.channel;
    changes = detect_param_changes(&lhs, &rhs);
    munit_assert_true(changes.channel_change);
    munit_assert_true(changes.SNR_change);

    // difference in channel only
    lhs.SNR = rhs.SNR;
    changes = detect_param_changes(&lhs, &rhs);
    munit_assert_true(changes.channel_change);
    munit_assert_false(changes.SNR_change);

    return MUNIT_OK;
}

MunitTest access_points_data_tests[] =
    {
        {.name = "/create_delete_map_test",
         .test = create_delete_map_test,
         .setup = NULL,
         .tear_down = NULL,
         .options = MUNIT_TEST_OPTION_NONE,
         .parameters = NULL},
        {.name = "/set_and_get_map_test",
         .test = set_and_get_map_test,
         .setup = NULL,
         .tear_down = NULL,
         .options = MUNIT_TEST_OPTION_NONE,
         .parameters = NULL},
        {.name = "/add_to_map_test",
         .test = add_to_map_test,
         .setup = NULL,
         .tear_down = NULL,
         .options = MUNIT_TEST_OPTION_NONE,
         .parameters = NULL},
        {.name = "/find_in_map_test",
         .test = find_in_map_test,
         .setup = NULL,
         .tear_down = NULL,
         .options = MUNIT_TEST_OPTION_NONE,
         .parameters = NULL},
        {.name = "/detect_changed_ssid_test",
         .test = detect_changed_ssid_test,
         .setup = create_map_with_entry,
         .tear_down = delete_map,
         .options = MUNIT_TEST_OPTION_NONE,
         .parameters = NULL},
        {.name = "/detect_param_changes_test",
         .test = detect_param_changes_test,
         .setup = NULL,
         .tear_down = NULL,
         .options = MUNIT_TEST_OPTION_NONE,
         .parameters = NULL},

        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}}; // test array terminator

static const MunitSuite suite = {
    .prefix = "",
    .tests = access_points_data_tests,
    .suites = NULL,
    .iterations = 1,
    .options = MUNIT_SUITE_OPTION_NONE};

int main(int argc, char** argv)
{
    return munit_suite_main(&suite, NULL, argc, argv);
}
