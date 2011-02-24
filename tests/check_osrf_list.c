#include <check.h>
#include "opensrf/osrf_list.h"

osrfList *testOsrfList;

//Set up the test fixture
void setup(void)
{
  //Set up a list of size 10, add some items to test with
  testOsrfList = osrfNewListSize(10);
  int *item1 = 7;
  int *item3 = 15;

  osrfListPush(testOsrfList, item1);
  osrfListPush(testOsrfList, NULL);
  osrfListPush(testOsrfList, item3);
}

//Clean up the test fixture
void teardown(void)
{
  osrfListFree(testOsrfList);
}

// BEGIN TESTS

START_TEST(test_osrf_list_osrfNewList)
  osrfList *newList = osrfNewList();
  fail_if(newList == NULL, "osrfList object not successfully created");
  fail_unless(newList->arrsize == 48, "the osrfList is not the default size of 48");
END_TEST

START_TEST(test_osrf_list_osrfNewListSize)
  osrfList *smallList = osrfNewListSize(5);
  fail_if(smallList == NULL, "smallList not successfully created");
  fail_unless(smallList->arrsize == 5, "smallList wasn't created with the size 5");
  fail_unless(smallList->freeItem == NULL, "freeItem should be null by default");
  int i;
  for (i = 0 ; i < smallList->arrsize ; i++)
  {
    fail_if(smallList->arrlist[i] != NULL, "Every value in smallList->arrlist should be null");
  }

  //List created with size <= 0
  osrfList *sizelessList = osrfNewListSize(0);
  fail_unless(sizelessList->arrsize == 16, "osrfNewListSize called with a size of 0 or less should have an array size of 16");
END_TEST

START_TEST(test_osrf_list_osrfListPush)
  fail_unless(osrfListPush(NULL, NULL) == -1, "Passing a null list to osrfListPush should return -1");
  int *listItem = 111;
  fail_unless(osrfListPush(testOsrfList, listItem) == 0, "osrfListPush should return 0 if successful");
  fail_unless(testOsrfList->size == 4, "testOsrfList->size did not update correctly, should be 4");
  fail_unless(osrfListGetIndex(testOsrfList, 3) == listItem, "listItem did not add to the end of testOsrfList");
END_TEST

START_TEST(test_osrf_list_osrfListPushFirst)
  fail_unless(osrfListPushFirst(NULL, NULL) == -1, "Passing a null list to osrfListPushFirst should return -1");
  int *listItem = 123;
  fail_unless(osrfListPushFirst(testOsrfList, listItem) == 3, "osrfListPushFirst should return a size of 3");
  fail_unless(osrfListGetIndex(testOsrfList, 1) == listItem, "listItem should be in index 1 because it is the first that is null");
END_TEST

START_TEST(test_osrf_list_osrfListSet)
  //Null argument check
  fail_unless(osrfListSet(NULL, NULL, 1) == NULL, "Given a null list arg, osrfListSet should return null");

  //Adding an item to an existing, NULL position in the list
  int *listItem = 456;
  fail_unless(osrfListSet(testOsrfList, listItem, 4) == NULL, "Calling osrfListSet on an empty index should return NULL");
  fail_unless(osrfListGetIndex(testOsrfList, 4) == listItem, "osrfListSet is not assigning item pointer to the correct position");
  fail_unless(testOsrfList->size == 5, "osrfListSet should update a lists size after adding an item to that list");

  //Adding an item to an exisiting, occupied position in the list
  int *listItem2 = 789;
  fail_unless(osrfListSet(testOsrfList, listItem2, 4) == listItem, "Calling osrfListSet on an index that held a value should return the reference to that value");
  fail_unless(osrfListGetIndex(testOsrfList, 4) == listItem2, "When called on a position that already has a value, osrfListSet should replace that value with the new item");
  fail_unless(testOsrfList->size == 5, "osrfListSet shouldn't update a lists size if the item is not added beyond the current size");

  //Adding an item to a position outside of the current array size

  int *listItem3 = 444;
  fail_unless(osrfListSet(testOsrfList, listItem3, 18) == NULL, "Calling osrfListSet on an empty index should return NULL, even if the index does not exist yet");
  fail_unless(testOsrfList->arrsize == 266, "New arrsize should be 266 since it was 10 before, and grows in increments of 256 when expanded");
  fail_unless(testOsrfList->size == 19, "List should have a size value of 19");
  fail_unless(osrfListGetIndex(testOsrfList, 18) == listItem3, "Value not added to correct index of list");
END_TEST

START_TEST(test_osrf_list_osrfListGetIndex)
  fail_unless(osrfListGetIndex(NULL, 1) == NULL, "Calling osrfListGetIndex with a null list should return null");
  fail_unless(osrfListGetIndex(testOsrfList, 8) == NULL, "Calling osrfListGetIndex with a value outside the range of occupied indexes should return NULL");
  fail_unless(osrfListGetIndex(testOsrfList, 2) == 15, "osrfListGetIndex should return the value of the list at the given index");
END_TEST

START_TEST(test_osrf_list_osrfListFree)
END_TEST
START_TEST(test_osrf_list_osrfListClear)
END_TEST
START_TEST(test_osrf_list_osrfListSwap)
END_TEST
START_TEST(test_osrf_list_osrfListRemove)
END_TEST
START_TEST(test_osrf_list_osrfListExtract)
END_TEST
START_TEST(test_osrf_list_osrfListFind)
END_TEST
START_TEST(test_osrf_list_osrfListGetCount)
END_TEST
START_TEST(test_osrf_list_osrfListPop)
END_TEST
START_TEST(test_osrf_list_osrfNewListIterator)
END_TEST
START_TEST(test_osrf_list_osrfListIteratorNext)
END_TEST
START_TEST(test_osrf_list_osrfListIteratorFree)
END_TEST
START_TEST(test_osrf_list_osrfListIteratorReset)
END_TEST
START_TEST(test_osrf_list_osrfListSetDefaultFree)
END_TEST

//END TESTS

Suite *osrf_list_suite(void)
{
  //Create test suite, test case, initialize fixture
  Suite *s = suite_create("osrf_list");
  TCase *tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  //Add tests to test case
  tcase_add_test(tc_core, test_osrf_list_osrfNewList);
  tcase_add_test(tc_core, test_osrf_list_osrfNewListSize);
  tcase_add_test(tc_core, test_osrf_list_osrfListPush);
  tcase_add_test(tc_core, test_osrf_list_osrfListPushFirst);
  tcase_add_test(tc_core, test_osrf_list_osrfListSet);
  tcase_add_test(tc_core, test_osrf_list_osrfListGetIndex);

  //Add test case to test suite
  suite_add_tcase(s, tc_core);

  return s;
}

void run_tests(SRunner *sr)
{
  srunner_add_suite(sr, osrf_list_suite());
}
