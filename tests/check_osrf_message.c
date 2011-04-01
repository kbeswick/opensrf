#include <check.h>
#include "opensrf/osrf_json.h"
#include "opensrf/osrf_message.h"

osrfMessage *o;

//Set up the test fixture
void setup(void)
{
  o = osrf_message_init(CONNECT, 1, 1);
}

//Clean up the test fixture
void teardown(void)
{
  osrfMessageFree(o);
}

//Tests

START_TEST(test_osrf_message_init)
  fail_if (o == NULL, "osrfMessage not created");
END_TEST

START_TEST(test_osrf_message_get_last_locale)
    fail_unless(osrf_message_get_last_locale() == NULL);
END_TEST

START_TEST(test_osrf_message_set_locale)
  const char* new_locale = "en-CA";
  fail_unless (osrf_message_set_locale(o, NULL) == NULL);
  fail_unless (osrf_message_set_locale(NULL, new_locale) == NULL);
  char* l = osrf_message_set_locale(o, new_locale);
  fail_unless (strcmp(l, "en-CA") == 0);
END_TEST

START_TEST(test_osrf_message_set_default_locale)
  fail_unless(osrf_message_set_default_locale(NULL) == NULL);
  fail_unless(osrf_message_set_default_locale("This string is \
        longer than 16 characters for sure") == NULL);
  fail_unless(strcmp(osrf_message_set_default_locale("fr-CA"),
        "fr-CA") == 0);
END_TEST

START_TEST(test_osrf_message_set_method)
  osrf_message_set_method(o, NULL);
  fail_unless(o->method_name == NULL);
  osrf_message_set_method(o, "add");
  fail_unless(strcmp(o->method_name, "add") == 0);
END_TEST

START_TEST(test_osrf_message_set_params)
  osrf_message_set_params(o, NULL);
  fail_unless(o->_params == NULL);
  jsonObject *testJSONObject;
  testJSONObject = jsonNewObject("test");
  osrf_message_set_params(o, testJSONObject);
  fail_unless(strcmp(jsonObjectGetIndex(o->_params, 0)->value.s, "test") == 0);
  jsonObjectFree(testJSONObject);
END_TEST

//END Tests

Suite *osrf_message_suite(void)
{
  //Create test suite, test case, initialize fixture
  Suite *s = suite_create("osrf_message");
  TCase *tc_core = tcase_create("Core");
  tcase_add_checked_fixture (tc_core, setup, teardown);

  //Add tests to test case
  tcase_add_test(tc_core, test_osrf_message_init);
  tcase_add_test(tc_core, test_osrf_message_get_last_locale);
  tcase_add_test(tc_core, test_osrf_message_set_locale);
  tcase_add_test(tc_core, test_osrf_message_set_default_locale);
  tcase_add_test(tc_core, test_osrf_message_set_method);
  tcase_add_test(tc_core, test_osrf_message_set_params);

  //Add test case to test suite
  suite_add_tcase(s, tc_core);

  return s;
}

void run_tests(SRunner *sr)
{
  srunner_add_suite(sr, osrf_message_suite());
}
