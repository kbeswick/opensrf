/**
	@file osrf_router_main.c
	@brief top level of OSRF Router

	This top level loads a configuration file and forks into one or more child 
	processes.  Each child process configures itself, daemonizes itself, and then (via a
	call to osrfRouterRun()) goes into an infinite loop to route messages among clients
	and servers.
*/

#include "osrf_router.h"
#include <opensrf/osrfConfig.h>
#include <opensrf/utils.h>
#include <opensrf/log.h>
#include <opensrf/osrf_json.h>
#include <signal.h>

/**
	An osrfRouter contains various bits and scraps that the router uses for networking.
*/
static osrfRouter* router = NULL;

/**
	@brief Respond to signal by exiting immediately.
	@param signo The signal number.
*/
void routerSignalHandler( int signo ) {
	osrfLogWarning( OSRF_LOG_MARK, "Received signal [%d], cleaning up...", signo );

    /* for now, just forcibly exit.  This is not a friendly way to clean up, but
     * there is a bug in osrfRouterFree() (in particular with cleaning up sockets),
     * that can cause the router process to stick around.  If we do this, we 
     * are guaranteed to exit.  
     */
    _exit(0);

	osrfConfigCleanup();
	osrfRouterFree(router);
	router = NULL;

	// Exit by re-raising the signal so that the parent
	// process can detect it
	
	signal( signo, SIG_DFL );
	raise( signo );
}

static void setupRouter(jsonObject* configChunk);


/**
	@brief The top-level function of the router program.
	@param argc Number of items in command line.
	@param argv Pointer to array of items on command line.
	@return System return code.

	Load configuration file, spawn one or more child processes, and exit.
*/
int main( int argc, char* argv[] ) {

	if( argc < 3 ) {
		osrfLogError( OSRF_LOG_MARK,
			"Usage: %s <path_to_config_file> <config_context>", argv[0] );
		exit( EXIT_FAILURE );
	}

	const char* config_file = argv[1];
	const char* context = argv[2];

	/* Get a set of router definitions from a config file */
	
	osrfConfig* cfg = osrfConfigInit(config_file, context);
	if( NULL == cfg ) {
		osrfLogError( OSRF_LOG_MARK, "Router can't load config file %s", config_file );
		exit( EXIT_FAILURE );
	}
	
	osrfConfigSetDefaultConfig(cfg);
    jsonObject* configInfo = osrfConfigGetValueObject(NULL, "/router");
	
	if( configInfo->size < 1 || NULL == jsonObjectGetIndex( configInfo, 1 ) ) {
		osrfLogError( OSRF_LOG_MARK, "No routers defined in config file %s, context \"%s\"",
			config_file, context );
		exit( EXIT_FAILURE );
	}
	
	/* We're done with the command line now, */
	/* so we can safely overlay it */
	
	init_proc_title( argc, argv );
	set_proc_title( "OpenSRF Router" );

	/* Spawn child process(es) */
	
    int i;
    for(i = 0; i < configInfo->size; i++) {
        jsonObject* configChunk = jsonObjectGetIndex(configInfo, i);
        if(fork() == 0) { /* create a new child to run this router instance */
            setupRouter(configChunk);
			break;  /* We're a child; don't spawn any more children here */
		}
    }

	return EXIT_SUCCESS;
}

/**
	@brief Configure and run a child process.
	@param configChunk Pointer to a subset of the loaded configuration.

	Configure oneself, daemonize, and then call osrfRouterRun() to go into an infinite
	loop.  Do not return unless something goes wrong.
*/
static void setupRouter(jsonObject* configChunk) {

	jsonObject* transport_cfg = jsonObjectGetKey( configChunk, "transport" );
	if( ! transport_cfg ) {
		//fprintf( stderr, "Child router process %ld has no transport configuration\n", (long) getpid() );
		return; /* these are not the configs you're looking for */
	}

	const char* server   = jsonObjectGetString( jsonObjectGetKey( transport_cfg, "server" ) );
	const char* port     = jsonObjectGetString( jsonObjectGetKey( transport_cfg, "port" ) );
	const char* username = jsonObjectGetString( jsonObjectGetKey( transport_cfg, "username" ) );
	const char* password = jsonObjectGetString( jsonObjectGetKey( transport_cfg, "password" ) );
	const char* resource = jsonObjectGetString( jsonObjectGetKey( transport_cfg, "resource" ) );

	const char* level    = jsonObjectGetString( jsonObjectGetKey( configChunk, "loglevel" ) );
	const char* log_file = jsonObjectGetString( jsonObjectGetKey( configChunk, "logfile" ) );
	const char* facility = jsonObjectGetString( jsonObjectGetKey( configChunk, "syslog" ) );

	int llevel = 1;
	if(level) llevel = atoi(level);

	if(!log_file)
	{ 
		fprintf(stderr, "Log file name not specified for router\n");
		return;
	}

	if(!strcmp(log_file, "syslog")) {
		osrfLogInit( OSRF_LOG_TYPE_SYSLOG, "router", llevel );
		osrfLogSetSyslogFacility(osrfLogFacilityToInt(facility));

	} else {
		osrfLogInit( OSRF_LOG_TYPE_FILE, "router", llevel );
		osrfLogSetFile( log_file );
	}

	osrfLogInfo( OSRF_LOG_MARK, "Router connecting as: server: %s port: %s "
		"user: %s resource: %s", server, port, username, resource );

	int iport = 0;
	if(port)	iport = atoi( port );

	osrfStringArray* tclients = osrfNewStringArray(4);
	osrfStringArray* tservers = osrfNewStringArray(4);

    jsonObject* tclientsList = jsonObjectFindPath(configChunk, "/trusted_domains/client");
    jsonObject* tserversList = jsonObjectFindPath(configChunk, "/trusted_domains/server");

	int i;

    if(tserversList->type == JSON_ARRAY) {
	    for( i = 0; i != tserversList->size; i++ ) {
            const char* serverDomain = jsonObjectGetString(jsonObjectGetIndex(tserversList, i));
		    osrfLogInfo( OSRF_LOG_MARK,  "Router adding trusted server: %s", serverDomain);
            osrfStringArrayAdd(tservers, serverDomain);
        }
    } else {
        const char* serverDomain = jsonObjectGetString(tserversList);
        osrfLogInfo( OSRF_LOG_MARK,  "Router adding trusted server: %s", serverDomain);
        osrfStringArrayAdd(tservers, serverDomain);
    }

    if(tclientsList->type == JSON_ARRAY) {
	    for( i = 0; i != tclientsList->size; i++ ) {
            const char* clientDomain = jsonObjectGetString(jsonObjectGetIndex(tclientsList, i));
		    osrfLogInfo( OSRF_LOG_MARK,  "Router adding trusted client: %s", clientDomain);
            osrfStringArrayAdd(tclients, clientDomain);
        }
    } else {
        const char* clientDomain = jsonObjectGetString(tclientsList);
        osrfLogInfo( OSRF_LOG_MARK,  "Router adding trusted client: %s", clientDomain);
        osrfStringArrayAdd(tclients, clientDomain);
    }


	if( tclients->size == 0 || tservers->size == 0 ) {
		osrfLogError( OSRF_LOG_MARK, "We need trusted servers and trusted client to run the router...");
		osrfStringArrayFree( tservers );
		osrfStringArrayFree( tclients );
		return;
	}

	router = osrfNewRouter( server,
			username, resource, password, iport, tclients, tservers );
	
	signal(SIGHUP,routerSignalHandler);
	signal(SIGINT,routerSignalHandler);
	signal(SIGTERM,routerSignalHandler);

	if( (osrfRouterConnect(router)) != 0 ) {
		fprintf(stderr, "Unable to connect router to jabber server %s... exiting\n", server );
		osrfRouterFree(router);
		return;
	}

	daemonize();
	osrfRouterRun( router );

	// Shouldn't get here, since osrfRouterRun()
	// should go into an infinite loop

	osrfRouterFree(router);
	router = NULL;

	return;
}


