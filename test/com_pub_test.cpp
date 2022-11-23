#include <thread>
#include <variant>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "MOE/orb_computer.h"
#include "MOE/orb_io.h"
#include <zmq.hpp>

int testing_write_frames(std::variant<int, std::string> cam_id=0){
    zmq::context_t context;

    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind("tcp://127.0.0.1:6969");
    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect("tcp://127.0.0.1:6970");
    subscriber.set(zmq::sockopt::subscribe, topic_for_kp());

    MESSAGE("bound frame pubs and subs.");

    auto orb_cpu = OrbComputer();

    std::vector<cv::KeyPoint> kpts;
    cv::Mat desc;

    cv::Mat im = cv::Mat();
    cv::VideoCapture cap;
    int apiID = cv::CAP_ANY;
    if(std::holds_alternative<int>(cam_id)){
        cap.open(std::get<int>(cam_id), apiID);
    }else{
        cap.open(std::get<std::string>(cam_id), apiID);
    }

    cap.set(cv::VideoCaptureProperties::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G')); // usually speed up a lot
    cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, 1920); // todo: either a settings class or ORB_SLAM3::System should give the desired camera
    cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, 1080);

    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    cv::Mat im2 = cv::Mat();
    for(;;){
        cap.read(im);
        MESSAGE("Read Image from cam or vid.");
        if(im.empty()){
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        write_mat(publisher, im);
        MESSAGE("Wrote Mat.");
        read_kps(subscriber, kpts, desc);
        MESSAGE("Read kps.");

        if (kpts.size() > 0) {
            cv::drawKeypoints(im, kpts, im2, cv::Scalar::all(255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            cv::imshow("kpts received", im2);
        }
        else {
            cv::imshow("kpts received", im);
        }
        if (cv::waitKey(1)>1) {// needed for opencv to process events so we can see the image
            break;
        }
    }
}

TEST_CASE("Simple image grabber and kp grabber") {
    testing_write_frames(); // OpenCV videoreader needs to be in the main loop afaik
}