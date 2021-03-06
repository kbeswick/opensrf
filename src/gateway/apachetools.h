#ifndef APACHE_TOOLS_H
#define APACHE_TOOLS_H

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_protocol.h"
//#include "apr_compat.h"
#include "apr_strings.h"
#include "apr_reslist.h"
#include "http_log.h"


#include "opensrf/string_array.h"
#include "opensrf/utils.h"
#include "opensrf/log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APACHE_TOOLS_MAX_POST_SIZE 10485760 /* 10 MB */


/* parses apache URL params (GET and POST).  
	Returns a osrfStringArray of the form [ key, val, key, val, ...]
	Returns NULL if there are no params */
osrfStringArray* apacheParseParms(request_rec* r);

/* provide the params string array, and this will generate a 
	string of array of param keys 
	the returned osrfStringArray most be freed by the caller
	*/
osrfStringArray* apacheGetParamKeys(osrfStringArray* params);

/* provide the params string array and a key name, and 
	this will provide the value found for that key 
	the returned osrfStringArray most be freed by the caller
	*/
osrfStringArray* apacheGetParamValues(osrfStringArray* params, char* key);

/* returns the first value found for the given param.  
	char* must be freed by the caller */
char* apacheGetFirstParamValue(osrfStringArray* params, char* key);

/* Writes msg to stderr, flushes stderr, and returns 0 */
int apacheDebug( char* msg, ... );

/* Writes to stderr, flushe stderr, and returns HTTP_INTERNAL_SERVER_ERROR; 
 */
int apacheError( char* msg, ... );

/*
 * Creates an apache table* of cookie name / value pairs 
 */
/*
apr_table_t* apacheParseCookies(request_rec *r);
*/

#ifdef __cplusplus
}
#endif

#endif
