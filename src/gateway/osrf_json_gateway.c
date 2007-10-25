#include "apachetools.h"
#include "opensrf/osrf_app_session.h"
#include "opensrf/osrf_system.h"
#include "opensrf/osrfConfig.h"
#include <opensrf/osrf_json.h>
#include <opensrf/osrf_json_xml.h>
#include <opensrf/osrf_legacy_json.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <strings.h>


#define MODULE_NAME "osrf_json_gateway_module"
#define GATEWAY_CONFIG "OSRFGatewayConfig"
#define DEFAULT_LOCALE "OSRFDefaultLocale"
#define CONFIG_CONTEXT "gateway"
#define JSON_PROTOCOL "OSRFGatewayLegacyJSON"
#define GATEWAY_USE_LEGACY_JSON 1

#define GATEWAY_DEFAULT_CONFIG "/openils/conf/opensrf_core.xml"


/* our config structure */
typedef struct { 
	char* configfile;  /* our bootstrap config file */
} osrf_json_gateway_config;

typedef struct { 
	int legacyJSON;
} osrf_json_gateway_dir_config;


module AP_MODULE_DECLARE_DATA osrf_json_gateway_module;

char* osrf_json_default_locale = "en-US";
char* osrf_json_gateway_config_file = NULL;
int bootstrapped = 0;
int numserved = 0;
osrfStringArray* allowedServices = NULL;

static const char* osrf_json_gateway_set_default_locale(cmd_parms *parms, void *config, const char *arg) {
	if (arg)
		osrf_json_default_locale = (char*) arg;
	return NULL;
}

static const char* osrf_json_gateway_set_config(cmd_parms *parms, void *config, const char *arg) {
	osrf_json_gateway_config  *cfg;
	cfg = ap_get_module_config(parms->server->module_config, &osrf_json_gateway_module);
	cfg->configfile = (char*) arg;
	osrf_json_gateway_config_file = (char*) arg;
	return NULL;
}

static const char* osrf_json_gateway_set_json_proto(cmd_parms *parms, void *config, const char *arg) {
	osrf_json_gateway_dir_config* cfg = (osrf_json_gateway_dir_config*) config;
	cfg->legacyJSON = (!strcasecmp((char*) arg, "false")) ? 0 : 1;
	return NULL;
}

/* tell apache about our commands */
static const command_rec osrf_json_gateway_cmds[] = {
	AP_INIT_TAKE1( GATEWAY_CONFIG, osrf_json_gateway_set_config, 
			NULL, RSRC_CONF, "osrf json gateway config file"),
	AP_INIT_TAKE1( DEFAULT_LOCALE, osrf_json_gateway_set_default_locale, 
			NULL, RSRC_CONF, "osrf json gateway default locale"),
	AP_INIT_TAKE1( JSON_PROTOCOL, osrf_json_gateway_set_json_proto,
			NULL, ACCESS_CONF, "osrf json gateway config file"),
	{NULL}
};

/* build the config object */
static void* osrf_json_gateway_create_config( apr_pool_t* p, server_rec* s) {
	osrf_json_gateway_config* cfg = (osrf_json_gateway_config*) 
			apr_palloc(p, sizeof(osrf_json_gateway_config));
	cfg->configfile = GATEWAY_DEFAULT_CONFIG;
	return (void*) cfg;
}

static void* osrf_json_gateway_create_dir_config( apr_pool_t* p, char* dir) {
	osrf_json_gateway_dir_config* cfg = (osrf_json_gateway_dir_config*) 
			apr_palloc(p, sizeof(osrf_json_gateway_dir_config));
	cfg->legacyJSON = GATEWAY_USE_LEGACY_JSON;
	return (void*) cfg;
}


static void osrf_json_gateway_child_init(apr_pool_t *p, server_rec *s) {

	char* cfg = osrf_json_gateway_config_file;
	char buf[32];
	int t = time(NULL);
	snprintf(buf, sizeof(buf), "%d", t);

	if( ! osrfSystemBootstrapClientResc( cfg, CONFIG_CONTEXT, buf ) ) {
		ap_log_error( APLOG_MARK, APLOG_ERR, 0, s, 
			"Unable to Bootstrap OpenSRF Client with config %s..", cfg);
		return;
	}

	bootstrapped = 1;
	allowedServices = osrfNewStringArray(8);
	osrfLogInfo(OSRF_LOG_MARK, "Bootstrapping gateway child for requests");
	osrfConfigGetValueList( NULL, allowedServices, "/services/service" );

	int i;
	for( i = 0; i < allowedServices->size; i++ ) {
		ap_log_error( APLOG_MARK, APLOG_DEBUG, 0, s, 
			"allowed service: %s\n", osrfStringArrayGetString(allowedServices, i));
	}
}

static int osrf_json_gateway_method_handler (request_rec *r) {

	/* make sure we're needed first thing*/
	if (strcmp(r->handler, MODULE_NAME )) return DECLINED;


	osrf_json_gateway_dir_config* dir_conf =  
		ap_get_module_config(r->per_dir_config, &osrf_json_gateway_module);


	/* provide 2 different JSON parsers and serializers to support legacy JSON */
	jsonObject* (*parseJSONFunc) (const char*) = legacy_jsonParseString;
	char* (*jsonToStringFunc) (const jsonObject*) = legacy_jsonObjectToJSON;

	if(dir_conf->legacyJSON) {
		ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "Using legacy JSON");

	} else {
		ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, "Not using legacy JSON");
		parseJSONFunc = jsonParseString;
		jsonToStringFunc = jsonObjectToJSON;
	}


	osrfLogDebug(OSRF_LOG_MARK, "osrf gateway: entered request handler");

	/* verify we are connected */
	if( !bootstrapped || !osrf_system_get_transport_client()) {
		ap_log_rerror( APLOG_MARK, APLOG_ERR, 0, r, "Cannot process request "
				"because the OpenSRF JSON gateway has not been bootstrapped...");
		usleep( 100000 ); /* 100 milliseconds */
		exit(1);
	}

	osrfLogSetAppname("osrf_json_gw");

	char* osrf_locale	= NULL;
	char* param_locale	= NULL;	/* locale for this call */
	char* service		= NULL;	/* service to connect to */
	char* method		= NULL;	/* method to perform */
	char* format		= NULL;	/* method to perform */
	char* a_l		= NULL;	/* request api level */
	char* input_format	= NULL; /* POST data format, defaults to 'format' */
	int   isXML		= 0;
	int   api_level		= 1;

	r->allowed |= (AP_METHOD_BIT << M_GET);
	r->allowed |= (AP_METHOD_BIT << M_POST);

	osrfLogDebug(OSRF_LOG_MARK, "osrf gateway: parsing URL params");
	string_array* mparams	= NULL;
	string_array* params	= apacheParseParms(r); /* free me */
	param_locale		= apacheGetFirstParamValue( params, "locale" );
	service			= apacheGetFirstParamValue( params, "service" );
	method			= apacheGetFirstParamValue( params, "method" ); 
	format			= apacheGetFirstParamValue( params, "format" ); 
	input_format		= apacheGetFirstParamValue( params, "input_format" ); 
	a_l			= apacheGetFirstParamValue( params, "api_level" ); 
	mparams			= apacheGetParamValues( params, "param" ); /* free me */

	if(format == NULL)
		format = "json";
	if(input_format == NULL)
		input_format = format;

	/* set the user defined timeout value */
	int timeout = 60;
	char* tout = apacheGetFirstParamValue( params, "timeout" ); /* request timeout in seconds */
	if( tout ) {
		timeout = atoi(tout);
		osrfLogDebug(OSRF_LOG_MARK, "Client supplied timeout of %d", timeout);
	}

	if (a_l)
		api_level = atoi(a_l);

	if (!strcasecmp(format, "xml")) {
		isXML = 1;
		ap_set_content_type(r, "application/xml");
	} else {
		ap_set_content_type(r, "text/plain");
	}

	int ret = OK;

	/* ----------------------------------------------------------------- */
	/* Grab the requested locale using the Accept-Language header*/


	if ( !param_locale ) {
		if ( apr_table_get(r->headers_in, "X-OpenSRF-Language") ) {
			param_locale = strdup( apr_table_get(r->headers_in, "X-OpenSRF-Language") );
		} else if ( apr_table_get(r->headers_in, "Accept-Language") ) {
			param_locale = strdup( apr_table_get(r->headers_in, "Accept-Language") );
		}
	}


	if (param_locale) {
		growing_buffer* osrf_locale_buf = buffer_init(16);	
		if (index(param_locale, ',')) {
			int ind = index(param_locale, ',') - param_locale;
			int i;
			for ( i = 0; i < ind && i < 128; i++ )
				buffer_add_char( osrf_locale_buf, param_locale[i] );
		} else {
			buffer_add( osrf_locale_buf, param_locale );
		}

		free(param_locale);
		osrf_locale = buffer_release( osrf_locale_buf );
	} else {
		osrf_locale = strdup( osrf_json_default_locale );
	}
	/* ----------------------------------------------------------------- */


	if(!(service && method) || 
		!osrfStringArrayContains(allowedServices, service)) {

		osrfLogError(OSRF_LOG_MARK, 
			"Service [%s] not found or not allowed", service);
		ret = HTTP_NOT_FOUND;

	} else {

		/* This will log all heaers to the apache error log 
		const apr_array_header_t* arr = apr_table_elts(r->headers_in);
		const void* ptr;

		while( (ptr = apr_array_pop(arr)) ) {
			apr_table_entry_t* e = (apr_table_entry_t*) ptr;
			fprintf(stderr, "Table entry: %s : %s\n", e->key, e->val );
		}
		fflush(stderr);
		*/

		osrfAppSession* session = osrf_app_client_session_init(service);
		osrf_app_session_set_locale(session, osrf_locale);

		double starttime = get_timestamp_millis();
		int req_id = -1;

		if(!strcasecmp(input_format, "json")) {
			jsonObject * arr = jsonNewObject(NULL);

			char* str;
			int i = 0;

			while( (str = osrfStringArrayGetString(mparams, i++)) ) 
				jsonObjectPush(arr, parseJSONFunc(str));

			req_id = osrf_app_session_make_req( session, arr, method, api_level, NULL );
		} else {

			/**
			* If we receive XML method params, convert each param to a JSON object
			* and pass the array of JSON object params to the method */
			if(!strcasecmp(input_format, "xml")) {
				jsonObject* jsonParams = jsonNewObject(NULL);

				char* str;
				int i = 0;
				while( (str = osrfStringArrayGetString(mparams, i++)) ) {
					jsonObjectPush(jsonParams, jsonXMLToJSONObject(str));
				}

				req_id = osrf_app_session_make_req( session, jsonParams, method, api_level, NULL );
				jsonObjectFree(jsonParams);
			}
		}


		if( req_id == -1 ) {
			osrfLogError(OSRF_LOG_MARK, "I am unable to communcate with opensrf..going away...");
			/* we don't want to spawn an intense re-forking storm 
			 * if there is no jabber server.. so give it some time before we die */
			usleep( 100000 ); /* 100 milliseconds */
			exit(1);
		}


		/* ----------------------------------------------------------------- */
		/* log all requests to the activity log */
		const char* authtoken = apr_table_get(r->headers_in, "X-OILS-Authtoken");
		if(!authtoken) authtoken = "";
		growing_buffer* act = buffer_init(128);	
		buffer_fadd(act, "[%s] [%s] %s %s", r->connection->remote_ip, authtoken, service, method );
		char* str; int i = 0;
		while( (str = osrfStringArrayGetString(mparams, i++)) ) {
			if( i == 1 ) {
				OSRF_BUFFER_ADD(act, " ");
				OSRF_BUFFER_ADD(act, str);
			} else {
				OSRF_BUFFER_ADD(act, ", ");
				OSRF_BUFFER_ADD(act, str);
			}
		}

		osrfLogActivity( OSRF_LOG_MARK, act->buf );
		buffer_free(act);
		/* ----------------------------------------------------------------- */


		osrf_message* omsg = NULL;

		int statuscode = 200;

		/* kick off the object */
		if (isXML)
			ap_rputs("<response xmlns=\"http://opensrf.org/-/namespaces/gateway/v1\"><payload>", r);
		else
			ap_rputs("{\"payload\":[", r);

		int morethan1		= 0;
		char* statusname	= NULL;
		char* statustext	= NULL;
		char* output		= NULL;

		while((omsg = osrfAppSessionRequestRecv( session, req_id, timeout ))) {
	
			statuscode = omsg->status_code;
			jsonObject* res;	

			if( ( res = osrfMessageGetResult(omsg)) ) {

				if (isXML) {
					output = jsonObjectToXML( res );
				} else {
					output = jsonToStringFunc( res );
					if( morethan1 ) ap_rputs(",", r); /* comma between JSON array items */
				}
				ap_rputs(output, r);
				free(output);
				morethan1 = 1;
		
			} else {
	
				if( statuscode > 299 ) { /* the request returned a low level error */
					statusname = omsg->status_name ? strdup(omsg->status_name) : strdup("Unknown Error");
					statustext = omsg->status_text ? strdup(omsg->status_text) : strdup("No Error Message");
					osrfLogError( OSRF_LOG_MARK,  "Gateway received error: %s", statustext );
				}
			}
	
			osrf_message_free(omsg);
			if(statusname) break;
		}

		double duration = get_timestamp_millis() - starttime;
		osrfLogDebug(OSRF_LOG_MARK, "gateway request took %f seconds", duration);


		if (isXML)
			ap_rputs("</payload>", r);
		else
			ap_rputs("]",r); /* finish off the payload array */

		if(statusname) {

			/* add a debug field if the request died */
			ap_log_rerror( APLOG_MARK, APLOG_INFO, 0, r, 
					"OpenSRF JSON Request returned error: %s -> %s", statusname, statustext );
			int l = strlen(statusname) + strlen(statustext) + 32;
			char buf[l];

			if (isXML)
				snprintf( buf, sizeof(buf), "<debug>\"%s : %s\"</debug>", statusname, statustext );

			else {
				char bb[l];
				snprintf(bb, sizeof(bb),  "%s : %s", statusname, statustext);
				jsonObject* tmp = jsonNewObject(bb);
				char* j = jsonToStringFunc(tmp);
				snprintf( buf, sizeof(buf), ",\"debug\": %s", j);
				free(j);
				jsonObjectFree(tmp);
			}

			ap_rputs(buf, r);

			free(statusname);
			free(statustext);
		}

		/* insert the status code */
		char buf[32];

		if (isXML)
			snprintf(buf, sizeof(buf), "<status>%d</status>", statuscode );
		else
			snprintf(buf, sizeof(buf), ",\"status\":%d", statuscode );

		ap_rputs( buf, r );

		if (isXML)
			ap_rputs("</response>", r);
		else
			ap_rputs( "}", r ); /* finish off the object */

		osrf_app_session_destroy(session);
	}

	osrfLogInfo(OSRF_LOG_MARK, "Completed processing service=%s, method=%s", service, method);
	string_array_destroy(params);
	string_array_destroy(mparams);

	osrfLogDebug(OSRF_LOG_MARK, "Gateway served %d requests", ++numserved);
	osrfLogClearXid();

	return ret;
}



static void osrf_json_gateway_register_hooks (apr_pool_t *p) {
	ap_hook_handler(osrf_json_gateway_method_handler, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_child_init(osrf_json_gateway_child_init,NULL,NULL,APR_HOOK_MIDDLE);
}


module AP_MODULE_DECLARE_DATA osrf_json_gateway_module = {
	STANDARD20_MODULE_STUFF,
	osrf_json_gateway_create_dir_config,
	NULL,
	osrf_json_gateway_create_config,
	NULL,
	osrf_json_gateway_cmds,
	osrf_json_gateway_register_hooks,
};




