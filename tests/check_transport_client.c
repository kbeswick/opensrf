#include <check.h>
#include "opensrf/transport_client.h"

transport_client *a_client;
transport_message *a_message; 

//Set up the test fixture
void setup(void)
{
  a_client = client_init("server", 1234, "unixpath", 123);
  a_message = message_init("body", "subject", "thread", "recipient", "sender");
}

//Clean up the test fixture
void teardown(void)
{
  free(a_client);
  free(a_message);
}

// Stub functions to simulate behavior of transport_session functions used in
// transport_client.c (to isolate system under test)

transport_session* init_transport(const char* server, int port, 
    const char* unix_path, void* user_data, int component) {

  transport_session* session = (transport_session*) safe_malloc(sizeof(transport_session));
  return session;
}

char* va_list_to_string(const char* format, ...) {
  return "user@server/resource";
}

int session_connect(transport_session* session, 
          const char* username, const char* password, 
          const char* resource, int connect_timeout, enum TRANSPORT_AUTH_TYPE auth_type ) {

  return 1;
}

int session_disconnect(transport_session* session) {
  return 1;
}

int session_connected(transport_session* session) {
  return 1;
}

int session_send_msg(transport_session* session, transport_message* message) {
  return 0;
}

int session_wait(transport_session* session, int timeout) {
  if (session == a_client->session && timeout == -1) {
    transport_message* recvd_msg = message_init("body1", "subject1", "thread1", "recipient1", "sender1");
    a_client->msg_q_head = recvd_msg;
    return 0;
  }
  else if (session == a_client->session && timeout > 0) {
    return 0;
  }
  else
    return 1;
}

//End Stubs

// BEGIN TESTS

START_TEST(test_transport_client_init)
  fail_unless(client_init(NULL, 1234, "some_path", 123) == NULL);
  transport_client *test_client = client_init("server", 1234, "unixpath", 123);
  fail_unless(test_client->msg_q_head == NULL);
  fail_unless(test_client->msg_q_tail == NULL);
  fail_if(test_client->session == NULL);
  //fail_unless(test_client->session->message_callback == client_message_handler);
  fail_unless(test_client->error == 0);
  fail_unless(strcmp(test_client->host, "server") == 0);
  fail_unless(test_client->xmpp_id == NULL);
END_TEST

START_TEST(test_transport_client_connect)
  fail_unless(client_connect(NULL, "user", "password", "resource", 10, AUTH_PLAIN) == 0);
  fail_unless(client_connect(a_client, "user", "password", "resource", 10, AUTH_PLAIN) == 1);
  fail_unless(a_client->xmpp_id == "user@server/resource");
END_TEST

START_TEST(test_transport_client_disconnect)
  fail_unless(client_disconnect(NULL) == 0);
  fail_unless(client_disconnect(a_client) == 1);
END_TEST

START_TEST(test_transport_client_connected)
  fail_unless(client_connected(NULL) == 0);
  fail_unless(client_connected(a_client) == 1);
END_TEST

START_TEST(test_transport_client_send_message)
  fail_unless(client_send_message(NULL, a_message) == -1);
  a_client->error = 1;
  //fail_unless(client_send_message(a_client->session, a_message) == -1);
  a_client->error = 0;
  //fail_unless(client_send_message(a_client->session, a_message) == 0);
  //fail_unless(strcmp(a_message->sender, "user") == 0);
END_TEST

START_TEST(test_transport_client_recv)
  //NULL client case
  fail_unless(client_recv(NULL, 10) == NULL);

  //Message at head of queue
  a_client->msg_q_head = a_message; //put a message at the head of the queue
  transport_message *msg = client_recv(a_client, 10);
  fail_if(msg == NULL);
  fail_unless(a_client->msg_q_head == NULL);
  fail_unless(msg->next == NULL);
  fail_unless(a_client->msg_q_tail == NULL);

  //session_wait failure with no timeout
  transport_client* other_client = client_init("server2", 4321, "unixpath2", 321);
  transport_message *msg2 = client_recv(other_client, -1);
  fail_unless(msg2 == NULL);
  fail_unless(other_client->error == 1);

  //message in queue with no timeout
  transport_message *msg3 = client_recv(a_client, -1);
  fail_unless(msg3->next == NULL);
  fail_unless(a_client->msg_q_head == NULL);
  fail_unless(a_client->msg_q_tail == NULL);
  fail_unless(strcmp(msg3->body, "body1") == 0);
  fail_unless(strcmp(msg3->subject, "subject1") == 0);
  fail_unless(strcmp(msg3->thread, "thread1") == 0);
  fail_unless(strcmp(msg3->recipient, "recipient1") == 0);
  fail_unless(strcmp(msg3->sender, "sender1") == 0);

  //No message in queue with timeout
  a_client->error = 0;
  transport_message *msg4 = client_recv(a_client, 1); //only 1 sec timeout so we dont slow down testing
  fail_unless(msg4 == NULL);
  fail_unless(a_client->error == 0);

  //session_wait failure with timeout
  other_client->error = 0;
  transport_message *msg5 = client_recv(other_client, 1); //only 1 sec again...
  fail_unless(msg5 == NULL);
  fail_unless(other_client->error == 1);
END_TEST

START_TEST(test_transport_client_free)
  fail_unless(client_free(NULL) == 0);
  transport_client* client1 = client_init("server", 1234, "unixpath", 123);
  fail_unless(client_free(client1) == 1);
END_TEST

START_TEST(test_transport_client_discard)
  fail_unless(client_discard(NULL) == 0);
  transport_client* client1 = client_init("server", 1234, "unixpath", 123);
  fail_unless(client_discard(client1) == 1);
END_TEST

START_TEST(test_transport_client_sock_fd)
  fail_unless(client_sock_fd(NULL) == 0);
  a_client->session->sock_id = 1;
  fail_unless(client_sock_fd(a_client) == 1);
END_TEST

//END TESTS

Suite *transport_client_suite(void)
{
  //Create test suite, test case, initialize fixture
  Suite *s = suite_create("transport_client");
  TCase *tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  //Add tests to test case
  tcase_add_test(tc_core, test_transport_client_init);
  tcase_add_test(tc_core, test_transport_client_connect);
  tcase_add_test(tc_core, test_transport_client_disconnect);
  tcase_add_test(tc_core, test_transport_client_connected);
  tcase_add_test(tc_core, test_transport_client_send_message);
  tcase_add_test(tc_core, test_transport_client_recv);
  tcase_add_test(tc_core, test_transport_client_free);
  tcase_add_test(tc_core, test_transport_client_discard);
  tcase_add_test(tc_core, test_transport_client_sock_fd);

  //Add test case to test suite
  suite_add_tcase(s, tc_core);

  return s;
}

void run_tests(SRunner *sr)
{
  srunner_add_suite(sr, transport_client_suite());
}
