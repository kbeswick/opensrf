#include <check.h>
#include "opensrf/transport_session.h"

#define JABBER_BODY_BUFSIZE    4096  /**< buffer size for message body */
#define JABBER_SUBJECT_BUFSIZE   64  /**< buffer size for message subject */
#define JABBER_THREAD_BUFSIZE    64  /**< buffer size for message thread */
#define JABBER_JID_BUFSIZE       64  /**< buffer size for various ids */
#define JABBER_STATUS_BUFSIZE    16  /**< buffer size for status code */


void setup(void) {
}

void teardown(void) {
}

int socket_wait(socket_manager* sock_mgr, int timeout, int sock_fd) {
  if (timeout == 1)
    return -1;
  else
    return 0;
}

int socket_send(int sock_fd, const char* data) {
  if (sock_fd == 1)
    return 0;
  else
    return -1;
}

int socket_open_unix_client(socket_manager* mgr, const char* unix_path) {
  if (strcmp(unix_path, "invalid_path") == 0)
    return -1;
  else
    return 1;
}

int socket_open_tcp_client(socket_manager* mgr, int port, const char* server) {
  if (port == 9999)
    return -1;
  else
    return 2;
}

//BEGIN TESTS

START_TEST(test_transport_session_init_transport_empty)
  transport_session* empty_tsession = init_transport(NULL, NULL, NULL, NULL, NULL);
  fail_if(empty_tsession == NULL, "init_transport should create a transport_session object");
  fail_unless(empty_tsession->user_data == NULL,
      "should populate transport_session->user_data with value of user_data arg");
  fail_unless(empty_tsession->component == NULL,
      "should populate transport_session->component with value of component arg");

  //data buffers
  fail_unless(empty_tsession->body_buffer->size == JABBER_BODY_BUFSIZE,
      "transport_init should initialize a body_buffer of size JABBER_BODY_BUFSIZE");
  fail_unless(empty_tsession->subject_buffer->size == JABBER_SUBJECT_BUFSIZE,
      "transport_init should initialize a subject_buffer of size JABBER_SUBJECT_BUFSIZE");
  fail_unless(empty_tsession->thread_buffer->size == JABBER_THREAD_BUFSIZE,
      "transport_init should initialize a thread_buffer of size JABBER_THREAD_BUFSIZE");
  fail_unless(empty_tsession->from_buffer->size == JABBER_JID_BUFSIZE,
      "transport_init should initialize a from_buffer of size JABBER_JID_BUFSIZE");
  fail_unless(empty_tsession->status_buffer->size == JABBER_STATUS_BUFSIZE,
      "transport_init should initialize a status_buffer of size JABBER_STATUS_BUFSIZE");
  fail_unless(empty_tsession->recipient_buffer->size == JABBER_JID_BUFSIZE,
      "transport_init should initialize a recipient_buffer of size JABBER_JID_BUFSIZE");
  fail_unless(empty_tsession->message_error_type->size == JABBER_JID_BUFSIZE,
      "transport_init should initialize a message_error_type buffer of size JABBER_JID_BUFSIZE");
  fail_unless(empty_tsession->session_id->size == 64,
      "transport_init should initialize a session_id buffer of size 64");

  fail_unless(empty_tsession->message_error_code == 0,
      "default value for transport_session->message_error_code is 0");

  fail_unless(empty_tsession->router_to_buffer->size == JABBER_JID_BUFSIZE,
      "transport_init should initialize a router_to_buffer of size JABBER_JID_BUFSIZE");
  fail_unless(empty_tsession->router_from_buffer->size == JABBER_JID_BUFSIZE,
      "transport_init should initialize a router_from_buffer of size JABBER_JID_BUFSIZE");
  fail_unless(empty_tsession->osrf_xid_buffer->size == JABBER_JID_BUFSIZE,
      "transport_init should initialize a osrf_xid_buffer of size JABBER_JID_BUFSIZE");
  fail_unless(empty_tsession->router_class_buffer->size == JABBER_JID_BUFSIZE,
      "transport_init should initialize a router_class_buffer of size JABBER_JID_BUFSIZE");
  fail_unless(empty_tsession->router_command_buffer->size == JABBER_JID_BUFSIZE,
      "transport_init should initialize a router_command_buffer of size JABBER_JID_BUFSIZE");

  fail_unless(empty_tsession->router_broadcast == 0,
      "default value for transport_session->router_broadcast is 0");
  //jabber state machine
  fail_if(empty_tsession->state_machine == NULL,
      "init_transport should create a state_machine in the transport_session");
  fail_unless(empty_tsession->state_machine->connected == 0,
      "default value for transport_session->state_machine->connected is 0");
  fail_unless(empty_tsession->state_machine->connecting == 0,
      "default value for transport_session->state_machine->connecting is 0");
  fail_unless(empty_tsession->state_machine->in_message == 0,
      "default value for transport_session->state_machine->in_message is 0");
  fail_unless(empty_tsession->state_machine->in_message_body == 0,
      "default value for transport_session->state_machine->in_message_body is 0");
  fail_unless(empty_tsession->state_machine->in_thread == 0,
      "default value for transport_session->state_machine->in_thread is 0");
  fail_unless(empty_tsession->state_machine->in_subject == 0,
      "default value for transport_session->state_machine->in_subject is 0");
  fail_unless(empty_tsession->state_machine->in_error == 0,
      "default value for transport_session->state_machine->in_error is 0");
  fail_unless(empty_tsession->state_machine->in_message_error == 0,
      "default value for transport_session->state_machine->in_message_error is 0");
  fail_unless(empty_tsession->state_machine->in_iq == 0,
      "default value for transport_session->state_machine->in_iq is 0");
  fail_unless(empty_tsession->state_machine->in_presence == 0,
      "default value for transport_session->state_machine->in_presence is 0");
  fail_unless(empty_tsession->state_machine->in_status == 0,
      "default value for transport_session->state_machine->in_status is 0");

  fail_if(empty_tsession->parser_ctxt == NULL,
      "init_transport should initialize a sax push parser in transport_session->parser_ctxt");
  fail_if(empty_tsession->sock_mgr == NULL,
      "init_transport should initialize a socket manager in transport_session->sock_mgr");
  fail_unless(empty_tsession->sock_mgr->on_socket_closed == NULL,
      "default value for transport_session->sock_mgr->on_socket_closed is NULL");
  fail_unless(empty_tsession->sock_mgr->socket == NULL,
      "default value for transport_session->sock_mgr->socket is NULL");
  fail_unless(empty_tsession->sock_mgr->blob == empty_tsession,
      "default value for transport_session->sock_mgr->blob is current session");
  fail_unless(empty_tsession->port == NULL,
      "init_transport should set transport_session->port to the value of the port arg");
  fail_unless(empty_tsession->server == "",
      "init_transport should set transport_session->server to the value of the server arg, or empty string if NULL");
  fail_unless(empty_tsession->unix_path == NULL,
      "init_transport should set transport_session->unix_path to the value of the unix_path arg");
  fail_unless(empty_tsession->sock_id == 0,
      "default value for transport_session->sock_id is 0");
  fail_unless(empty_tsession->message_callback == NULL,
      "default value for transport_session->message_callback is NULL");
END_TEST

START_TEST(test_transport_session_init_transport_populated)
  transport_session *tsession = init_transport("server", 1234, "some_path", "user", 1);
  fail_if(tsession == NULL, "init_transport should return a new transport_session object");
  fail_unless(strcmp(tsession->user_data, "user") == 0,
      "transport_session->user_data should contain user_data arg contents");
  fail_unless(tsession->component, 1,
      "transport_session->component should contain component arg contents");
  fail_unless(tsession->port == 1234, 
      "transport_session->port should contain port arg contents");
  fail_unless(strcmp(tsession->server, "server") == 0,
      "transport_session->server should contain server arg contents");
  fail_unless(strcmp(tsession->unix_path, "some_path") == 0, 
      "transport_session->unix_path should contain unix_path arg contents");
END_TEST

START_TEST(test_transport_session_free)
  transport_session *tsession = init_transport("server", 1234, "some_path", "user", 1);
  fail_unless(session_free(NULL) == 0,
      "session_free should return 0 on failure");
  fail_unless(session_free(tsession) == 1,
      "session_free should return 1 on success");
END_TEST

START_TEST(test_transport_session_discard)
  transport_session *tsession = init_transport("server", 1234, "some_path", "user", 1);
  fail_unless(session_discard(NULL) == 0,
      "session_discard should return 0 on failure");
  fail_unless(session_discard(tsession) == 1,
      "session_discard should return 1 on success");
END_TEST

START_TEST(test_transport_session_connected)
  transport_session *tsession = init_transport("server", 1234, "some_path", "user", 1);
  fail_unless(session_connected(NULL) == 0,
      "session_connected should return 0 if no session is passed");
  fail_unless(session_connected(tsession) == 0,
      "session_connected should return 0 if session is not connected");
  tsession->state_machine->connected = 1;
  fail_unless(session_connected(tsession) == 1,
      "session_connected should return 1 if session is connected");
END_TEST

START_TEST(test_transport_session_wait)
  fail_unless(session_wait(NULL, 1) == 0,
      "session_wait should return 0 if no session is passed");
  transport_session *tsession = init_transport("server", 1234, "some_path", "user", 1);
  fail_unless(session_wait(tsession, 1) == -1,
      "if the call to socket_wait fails, return -1");
  fail_unless(tsession->state_machine->connected == 0,
      "a failure on session_wait should set transport_session->state_machine->connected to 0");
  fail_unless(session_wait(tsession, 0) == 0,
      "session_wait should return 0 if successful");
END_TEST

START_TEST(test_transport_session_send_msg)
  transport_message *msg = message_init("body", "subject", "thread", "recipient", "sender");
  transport_session *ses = init_transport("server", 1234, "some_path", "user", 1);

  fail_unless(session_send_msg(NULL, NULL) == -1,
      "session_send_msg should return -1 when no session arg is passed");
  fail_unless(session_send_msg(ses, msg) == -1,
      "session_send_msg should return -1 when jabber state machine is not connected");
  ses->state_machine->connected = 1;
  fail_unless(session_send_msg(ses, msg) == -1,
      "session_send_msg should return -1 on failure");
  ses->sock_id = 1;
  fail_unless(session_send_msg(ses, msg) == 0,
      "session_send_msg should return 0 on success");
END_TEST

START_TEST(test_transport_session_connect)
  transport_session *ses = init_transport("server", 0, NULL, "user", 1);
  ses->sock_id = 1;
  fail_unless(session_connect(NULL, NULL, NULL, NULL, NULL, AUTH_PLAIN) == 0,
      "session_connect should return 0 if no session is given");
  fail_unless(session_connect(ses, NULL, NULL, NULL, NULL, AUTH_PLAIN) == 0,
      "session_connect should return 0 if ses->sock_id is non 0");
  ses->sock_id = 0;
  fail_unless(session_connect(ses, NULL, NULL, NULL, NULL, AUTH_PLAIN) == 0,
      "session_connect should return 0 if no port or unix path are given");
  ses->unix_path = "invalid_path";
  fail_unless(session_connect(ses, NULL, NULL, NULL, NULL, AUTH_PLAIN) == 0,
      "session_connect should return 0 if the call to socket_open_unix_client fails");
  ses->port = 9999;
  fail_unless(session_connect(ses, NULL, NULL, NULL, NULL, AUTH_PLAIN) == 0,
      "session_connect should return 0 if the call to socket_open_tcp_client fails");
END_TEST

//END TESTS

Suite *transport_session_suite(void) {
  Suite *s = suite_create("transport_session");
  TCase *tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  //Add tests to test case
  tcase_add_test(tc_core, test_transport_session_init_transport_empty);
  tcase_add_test(tc_core, test_transport_session_init_transport_populated);
  tcase_add_test(tc_core, test_transport_session_free);
  tcase_add_test(tc_core, test_transport_session_discard);
  tcase_add_test(tc_core, test_transport_session_connected);
  tcase_add_test(tc_core, test_transport_session_wait);
  tcase_add_test(tc_core, test_transport_session_send_msg);
  tcase_add_test(tc_core, test_transport_session_connect);


  suite_add_tcase(s, tc_core);

  return s;
}

void run_tests(SRunner *sr) {
  srunner_add_suite(sr, transport_session_suite());
}
