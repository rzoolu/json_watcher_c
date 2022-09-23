#include <munit/munit.h>

extern const MunitSuite access_points_data_test_suite;
extern const MunitSuite json_parser_test_suite;

// returns EXIT_FAILURE(1) if any test fails, EXIT_SUCCESS(0) otherwise
int main(int argc, char* argv[])
{
    return munit_suite_main(&access_points_data_test_suite, NULL, argc, argv) ||
           munit_suite_main(&json_parser_test_suite, NULL, argc, argv);
}
