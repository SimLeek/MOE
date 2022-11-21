#include <thread>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "frame_writer.cc"
#include "orb_computer.h"

void testing_orb_loop(){
    auto img_share = Shared::SharedQueue("KPComputer", 10*1024*1024, "image");
    auto kp_share = Shared::SharedQueue("KPComputer", 10*1024*1024, "kp");
    auto orb_cpu = OrbComputer();

    std::vector<cv::KeyPoint> kpts;
    cv::Mat desc;

    for(int i=0; i<300; i++){
        auto cvimg = read_mat(img_share);

        orb_cpu.detect_and_compute(cvimg, cv::Mat(), kpts, desc);

        write_kps(kp_share, kpts, desc);
    }
}

int testing_write_frames(std::variant<int, std::string> cam_id=0){
    auto img_share = Shared::SharedQueue("KPComputer", 10*1024*1024, "image");
    auto kp_share = Shared::SharedQueue("KPComputer", 10*1024*1024, "kp");
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
    cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, 320); // todo: either a settings class or ORB_SLAM3::System should give the desired camera
    cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, 240);

    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    cv::Mat im2 = cv::Mat();
    for(int i=0; i<300; i++){
        cap.read(im);
        if(im.empty()){
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        write_mat(img_share, im);
        read_kps(kp_share, kpts, desc);

        cv::drawKeypoints(im, kpts, im2);
        cv::imshow("kpts received", im2);
    }
}

TEST_CASE("Simple image grabber and kp grabber") {
	std::thread a(testing_orb_loop);
    testing_write_frames(); // OpenCV videoreader needs to be in the main loop afaik
}