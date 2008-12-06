#include <opensrf/string_array.h>

osrfStringArray* osrfNewStringArray(int size) {
	if(size > STRING_ARRAY_MAX_SIZE)
		osrfLogError( OSRF_LOG_MARK, "osrfNewStringArray size is too large");

	osrfStringArray* arr;
	OSRF_MALLOC( arr, sizeof(osrfStringArray));
    arr->list = osrfNewListSize(size);
    osrfListSetDefaultFree(arr->list);
	arr->size = 0;
	return arr;
}


void osrfStringArrayAdd(osrfStringArray* arr, char* str) {
	if(arr == NULL || str == NULL ) return;
	if( arr->size > STRING_ARRAY_MAX_SIZE ) 
		osrfLogError( OSRF_LOG_MARK, "osrfStringArrayAdd size is too large");
    osrfListPush(arr->list, strdup(str));
    arr->size = arr->list->size;
}

char* osrfStringArrayGetString(osrfStringArray* arr, int index) {
    if(!arr) return NULL;
    return OSRF_LIST_GET_INDEX(arr->list, index);
}

void osrfStringArrayFree(osrfStringArray* arr) {
    OSRF_STRING_ARRAY_FREE(arr);
}

int osrfStringArrayContains( osrfStringArray* arr, char* string ) {
	if(!(arr && string)) return 0;
	int i;
	for( i = 0; i < arr->size; i++ ) {
        char* str = OSRF_LIST_GET_INDEX(arr->list, i);
		if(str && !strcmp(str, string)) 
            return 1;
	}

	return 0;
}

void osrfStringArrayRemove( osrfStringArray* arr, char* tstr) {
	if(!(arr && tstr)) return;
	int i;
    char* str;

	for( i = 0; i < arr->size; i++ ) {
        /* find and remove the string */
        str = OSRF_LIST_GET_INDEX(arr->list, i);
		if(str && !strcmp(str, tstr)) {
            osrfListRemove(arr->list, i);
			break;
		}
	}

    /* disable automatic item freeing on delete and shift
     * items up in the array to fill in the gap
     */
    arr->list->freeItem = NULL;
	for( ; i < arr->size - 1; i++ ) 
        osrfListSet(arr->list, OSRF_LIST_GET_INDEX(arr->list, i+1) , i);

    /* remove the last item since it was shifted up */
    osrfListRemove(arr->list, i);

    /* re-enable automatic item freeing in delete */
    osrfListSetDefaultFree(arr->list);
	arr->size--;
}


