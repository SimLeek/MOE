#include <opencv2/opencv.hpp>
#include "MOE/orb_computer.h"
#include "MOE/flatbuffers/ocv_mat_generated.h"
#include "MOE/flatbuffers/ocv_kps_generated.h"
#include "MOE/orb_io.h"


void orb_loop(
    zmq::socket_t& subscriber, 
    zmq::socket_t& publisher
){
    auto orb_cpu = OrbComputer();

    std::vector<cv::KeyPoint> kpts;
    cv::Mat desc;
    for(;;){
        auto cvimg = read_mat(subscriber);

        orb_cpu.detect_and_compute(cvimg, cv::Mat(), kpts, desc);

        write_kps(publisher, kpts, desc);
    }
}

int main(int argc, char** argv){
    if(argc!=3){
        std::cerr<<"Wrong arg count.\n";
        exit(-1);
    }
    const char* sub_addr = argv[1];
    const char* pub_addr = argv[2];

    //zmq::context_t * context = static_cast<zmq::context_t*>(ctx);
    zmq::context_t context;

    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect(sub_addr);
    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind(pub_addr);

    orb_loop(subscriber, publisher);

}