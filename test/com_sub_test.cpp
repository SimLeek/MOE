#include <thread>
#include <variant>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "MOE/orb_computer.h"
#include "MOE/orb_io.h"
#include <zmq.hpp>

void testing_orb_loop(){
    zmq::context_t context;
    MESSAGE("copied orb context.");

    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect("tcp://127.0.0.1:6969");
    std::string topic = topic_for_mat();
    subscriber.set(zmq::sockopt::subscribe, topic);
    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind("tcp://127.0.0.1:6970");

    MESSAGE("bound orb pubs and subs.");

    auto orb_cpu = OrbComputer();
    MESSAGE("Made ORB computer.");

    std::vector<cv::KeyPoint> kpts;
    cv::Mat desc;
    MESSAGE("Made kp ans desc containers.");

    for(;;){ //todo: break out of this loop safely whenever closing the program
        auto cvimg = read_mat(subscriber);
        MESSAGE("Read Mat.");

        orb_cpu.detect_and_compute(cvimg, cv::Mat(), kpts, desc);
        MESSAGE("Detected ORBs.");

        write_kps(publisher, kpts, desc);
        MESSAGE("Wrote KPs.");
    }
}

TEST_CASE("Simple image grabber and kp grabber") {
	testing_orb_loop();
}