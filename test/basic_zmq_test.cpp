#include <chrono> // sleep
#include <thread>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <zmq.h>
#include <string.h>
#include <stdio.h>

//loops copied from: https://zguide.zeromq.org/docs/chapter1/

int server (void)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    CHECK(rc == 0);

    for(int i=0;i<10;i++) {
        char buffer [10];
        zmq_recv (responder, buffer, 10, 0);
        std::cout<<"Received Hello\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        zmq_send (responder, "World", 5, 0);
    }
    return 0;
}

int client (void)
{
    std::cout<<"Connecting to hello world server...\n";
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer [10];
        std::cout<<"Sending Hello " << request_nbr <<"...\n";
        zmq_send (requester, "Hello", 5, 0);
        zmq_recv (requester, buffer, 10, 0);
        std::cout<<"Received World " << request_nbr << "\n";
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}

TEST_CASE("Simple zmq req rep to test that anything's working and restore my sanity") {
    MESSAGE("Starting...\n");
	std::thread a(server);
	std::thread b(client);

    a.join();
	b.join();
}