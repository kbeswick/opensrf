COMMON = testsuite.c
OSRF_INC = $(top_srcdir)/include/opensrf
AM_LDFLAGS = $(DEF_LDFLAGS) -R $(libdir)

TESTS = check_osrf_message check_osrf_json_object
check_PROGRAMS = check_osrf_message check_osrf_json_object

check_osrf_message_SOURCES = $(COMMON) $(OSRF_INC)/osrf_message.h check_osrf_message.c
check_osrf_message_CFLAGS = @CHECK_CFLAGS@ $(DEF_CFLAGS)
check_osrf_message_LDADD = @CHECK_LIBS@ $(top_builddir)/src/libopensrf/libopensrf.la

check_osrf_json_object_SOURCES = $(COMMON) $(OSRF_INC)/osrf_json_object.h check_osrf_json_object.c
check_osrf_json_object_CFLAGS = @CHECK_CFLAGS@ $(DEF_CFLAGS)
check_osrf_json_object_LDADD = @CHECK_LIBS@ $(top_builddir)/src/libopensrf/libopensrf.la
