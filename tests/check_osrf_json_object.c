#include <check.h>
#include "opensrf/osrf_json.h"

jsonObject *jsonObj;
jsonObject *jsonHash;
jsonObject *jsonNumber;
jsonObject *jsonBool;
jsonObject *jsonArray;

//Set up the test fixture
void setup (void)
{
  jsonObj = jsonNewObject("test");
  jsonHash = jsonNewObject(NULL);
  jsonNumber = jsonNewNumberObject(123.456);
  jsonBool = jsonNewBoolObject(0);
  jsonArray = jsonNewObjectType(JSON_ARRAY);
}

//Clean up the test fixture
void teardown (void)
{
  jsonObjectFree(jsonObj);
  jsonObjectFree(jsonHash);
  jsonObjectFree(jsonNumber);
  jsonObjectFree(jsonBool);
  jsonObjectFree(jsonArray);
}

//Tests

START_TEST(test_osrf_json_object_jsonNewObject)
  fail_if(jsonObj == NULL, "jsonObject not created");
END_TEST

START_TEST(test_osrf_json_object_jsonNewObjectFmt)
  jsonObject *fmtObj;
  jsonObject *nullObj;
  fmtObj = jsonNewObjectFmt("string %d %d", 10, 20);
  nullObj = jsonNewObjectFmt(NULL);

  fail_if(fmtObj == NULL, "jsonObject not created");
  fail_unless(strcmp(fmtObj->value.s, "string 10 20") == 0);
  fail_unless(fmtObj->type == JSON_STRING);
  fail_unless(nullObj->value.s == NULL);
  fail_unless(nullObj->type == JSON_NULL);
END_TEST

START_TEST(test_osrf_json_object_jsonNewNumberObject)
  jsonObject *numObj;
  numObj = jsonNewNumberObject(123);

  fail_if(numObj == NULL, "jsonObject not created");
  fail_unless(strcmp(numObj->value.s, "123") == 0);
  fail_unless(numObj->type == JSON_NUMBER);
END_TEST

START_TEST(test_osrf_json_object_jsonNewBoolObject)
  fail_unless(jsonBool->type == JSON_BOOL);
  fail_unless(jsonBool->value.b == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonSetBool)
  jsonSetBool(jsonBool, -1);
  fail_unless(jsonBool->value.b == -1);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectSetKey)
  fail_unless(jsonObjectSetKey(NULL, "key1", NULL) == -1);
  fail_unless(jsonObjectSetKey(jsonHash, "key1", NULL) == 1);
  fail_unless(jsonObjectSetKey(jsonHash, "key2", jsonNewObject("test2")) == 2);
  fail_unless(jsonObjectGetKey(jsonHash, "key1")->value.s == NULL);
  fail_unless(strcmp(jsonObjectGetKey(jsonHash, "key2")->value.s, "test2") == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectRemoveKey)
  jsonObjectSetKey(jsonHash, "key1", jsonNewObject("value"));
  fail_unless(jsonObjectRemoveKey(jsonHash, NULL) == -1);
  fail_unless(jsonObjectRemoveKey(jsonHash, "key1") == 1);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectGetKey)
  jsonObjectSetKey(jsonHash, "key1", jsonNewObject("value"));
  fail_unless(strcmp(jsonObjectGetKey(jsonHash, "key1")->value.s, "value") == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectSetClass)
  jsonObjectSetClass(jsonObj, NULL);
  fail_unless(jsonObj->classname == NULL);
  jsonObjectSetClass(jsonObj, "aClass");
  fail_unless(strcmp(jsonObj->classname, "aClass") == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectGetClass)
  fail_unless(jsonObjectGetClass(NULL) == NULL);
  jsonObjectSetClass(jsonObj, "aClass");
  fail_unless(strcmp(jsonObjectGetClass(jsonObj), "aClass") == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectSetIndex)
  jsonObject *jsonArrayValue = jsonNewObject("value");
  fail_unless(jsonObjectSetIndex(NULL, 0, jsonArrayValue) == -1);
  fail_unless(jsonObjectSetIndex(jsonArray, 0, NULL) == 1);
  fail_unless(jsonObjectSetIndex(jsonArray, 1, jsonArrayValue) == 2);
  jsonObject *jsonArrayResult = jsonObjectGetIndex(jsonArray, 1);
  fail_unless(strcmp(jsonArrayResult->value.s, "value") == 0);
  fail_unless(jsonArrayResult->parent == jsonArray);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectGetIndex)
  jsonObject *jsonArrayValue = jsonNewObject("value");
  jsonObjectSetIndex(jsonArray, 0, jsonArrayValue);
  fail_unless(jsonObjectGetIndex(NULL, 0) == NULL);
  fail_unless(jsonObjectGetIndex(jsonArray, 2) == NULL);
  fail_unless(jsonObjectGetIndex(jsonNumber, 0) == NULL);
  jsonObject *getIndexValue = jsonObjectGetIndex(jsonArray, 0);
  fail_unless(strcmp(getIndexValue->value.s, "value") == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectToJSONRaw)
  jsonObject *val1 = jsonNewObject("value1");
  jsonObject *val2 = jsonNewObject("value2");
  jsonObjectSetClass(val1, "class1");
  jsonObjectSetClass(val2, "class2");

  jsonObjectSetKey(jsonHash, "key1", val1);
  jsonObjectSetKey(jsonHash, "key2", val2);
  fail_unless(strcmp(jsonObjectToJSONRaw(jsonHash),
        "{\"key1\":\"value1\",\"key2\":\"value2\"}") == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectToJSON)
  jsonObject *val1 = jsonNewObject("value1");
  jsonObject *val2 = jsonNewObject("value2");
  jsonObjectSetClass(val1, "class1");
  jsonObjectSetClass(val2, "class2");

  jsonObjectSetKey(jsonHash, "key1", val1);
  jsonObjectSetKey(jsonHash, "key2", val2);
  fail_unless(strcmp(jsonObjectToJSON(jsonHash),
        "{\"key1\":{\"__c\":\"class1\",\"__p\":\"value1\"},\"key2\":{\"__c\":\"class2\",\"__p\":\"value2\"}}") == 0);
END_TEST

START_TEST(test_osrf_json_object_doubleToString)
  fail_unless(strcmp(doubleToString(123.456),
        "123.456000000000003069544618484") == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectGetString)
  fail_unless(strcmp(jsonObjectGetString(jsonObj), "test") == 0);
  fail_unless(strcmp(jsonObjectGetString(jsonNumber),
        "123.456000000000003069544618484") == 0);
  jsonObject *jsonNullNumber = jsonNewNumberObject(0);
  jsonObjectSetNumberString(jsonNullNumber, "NaN"); //set jsonNullNumber->value to NULL
  fail_unless(strcmp(jsonObjectGetString(jsonNullNumber), "0") == 0);
  fail_unless(jsonObjectGetString(jsonHash) == NULL);
  fail_unless(jsonObjectGetString(NULL) == NULL);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectGetNumber)
  fail_unless(jsonObjectGetNumber(NULL) == 0);
  fail_unless(jsonObjectGetNumber(jsonHash) == 0);
  jsonObject *jsonNullNumber = jsonNewNumberObject(0);
  jsonObjectSetNumberString(jsonNullNumber, "NaN");
  fail_unless(jsonObjectGetNumber(jsonNullNumber) == 0);
  fail_unless(jsonObjectGetNumber(jsonNumber) == 123.456000000000003069544618484);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectSetString)
  jsonObjectSetString(jsonObj, NULL);
  fail_unless(strcmp(jsonObj->value.s, "test") == 0);
  jsonObjectSetString(jsonObj, "changed");
  fail_unless(strcmp(jsonObj->value.s, "changed") == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectSetNumberString)
  fail_unless(jsonObjectSetNumberString(NULL, "asdf") == -1);
  fail_unless(jsonObjectSetNumberString(jsonNumber, NULL) == -1);
  fail_unless(jsonObjectSetNumberString(jsonNumber, "111.111") == 0);
  fail_unless(strcmp(jsonNumber->value.s, "111.111") == 0);
  fail_unless(jsonObjectSetNumberString(jsonNumber, "not a number") == -1);
  fail_unless(jsonNumber->value.s == NULL);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectSetNumber)
  jsonObjectSetNumber(jsonNumber, 999.999);
  fail_unless(strcmp(jsonNumber->value.s, "999.999000000000023646862246096") == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonObjectClone)
  jsonObject *nullClone = jsonObjectClone(NULL);
  fail_unless(nullClone->type == JSON_NULL && nullClone->value.s == NULL);

  jsonObject *anotherNullClone = jsonObjectClone(nullClone);
  fail_unless(anotherNullClone->type == JSON_NULL && anotherNullClone->value.s == NULL);

  jsonObject *stringClone = jsonObjectClone(jsonObj);
  fail_unless(stringClone->type == JSON_STRING && strcmp(stringClone->value.s, "test") == 0);

  jsonObject *numberClone = jsonObjectClone(jsonNumber);
  fail_unless(numberClone->type == JSON_NUMBER);
  fail_unless(strcmp(numberClone->value.s, "123.456000000000003069544618484") == 0);

  jsonObject *boolClone = jsonObjectClone(jsonBool);
  fail_unless(boolClone->type == JSON_BOOL && boolClone->value.b == 0);

  //Array
  jsonObject *arrayVal1 = jsonNewObject("arrayval1");
  jsonObject *arrayVal2 = jsonNewObject("arrayval2");
  jsonObjectSetIndex(jsonArray, 0, arrayVal1);
  jsonObjectSetIndex(jsonArray, 0, arrayVal2);
  jsonObject *arrayClone = jsonObjectClone(jsonArray);
  fail_unless(strcmp(jsonObjectToJSON(arrayClone), jsonObjectToJSON(jsonArray)) == 0);

  //Hash
  jsonObject *val1 = jsonNewObject("value1");
  jsonObject *val2 = jsonNewObject("value2");
  jsonObjectSetClass(val1, "class1");
  jsonObjectSetClass(val2, "class2");
  jsonObjectSetKey(jsonHash, "key1", val1);
  jsonObjectSetKey(jsonHash, "key2", val2);
  jsonObject *hashClone = jsonObjectClone(jsonHash);
  fail_unless(strcmp(jsonObjectToJSON(hashClone), jsonObjectToJSON(jsonHash)) == 0);
END_TEST

START_TEST(test_osrf_json_object_jsonBoolIsTrue)
  fail_unless(jsonBoolIsTrue(NULL) == 0);
  fail_unless(jsonBoolIsTrue(jsonObj) == 0);
  fail_unless(jsonBoolIsTrue(jsonBool) == 0);
  jsonObject *newBool = jsonNewBoolObject(123);
  fail_unless(jsonBoolIsTrue(newBool) == 1);
END_TEST

//END Tests


Suite *osrf_json_object_suite (void)
{
  //Create test suite, test case, initialize fixture
  Suite *s = suite_create("osrf_json_object");
  TCase *tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  //Add tests to test case
  tcase_add_test(tc_core, test_osrf_json_object_jsonNewObject);
  tcase_add_test(tc_core, test_osrf_json_object_jsonNewObjectFmt);
  tcase_add_test(tc_core, test_osrf_json_object_jsonNewBoolObject);
  tcase_add_test(tc_core, test_osrf_json_object_jsonSetBool);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectToJSONRaw);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectToJSON);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectSetKey);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectGetKey);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectSetClass);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectGetClass);
  tcase_add_test(tc_core, test_osrf_json_object_jsonNewNumberObject);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectRemoveKey);
  tcase_add_test(tc_core, test_osrf_json_object_doubleToString);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectGetString);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectGetNumber);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectSetString);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectSetNumberString);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectSetNumber);
  tcase_add_test(tc_core, test_osrf_json_object_jsonBoolIsTrue);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectSetIndex);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectGetIndex);
  tcase_add_test(tc_core, test_osrf_json_object_jsonObjectClone);

  //Add test case to test suite
  suite_add_tcase(s, tc_core);

  return s;
}

void run_tests (SRunner *sr)
{
  srunner_add_suite (sr, osrf_json_object_suite());
}
