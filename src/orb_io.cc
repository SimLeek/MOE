#include "MOE/orb_io.h"
#include "MOE/flatbuffers/ocv_mat_generated.h"
#include "MOE/flatbuffers/ocv_kps_generated.h"
#include <iostream>
//TODO: create a polymorphic class that receives/sends data instead of using publishers/subscribers

void write_mat(zmq::socket_t& publisher, cv::Mat& im){
    flatbuffers::FlatBufferBuilder builder(10*1024*1024);

    size_t dataSize = im.rows * im.cols * (int)im.elemSize();

    std::vector<int8_t> mat_vec(reinterpret_cast<int8_t*>(im.data), reinterpret_cast<int8_t*>(im.data+dataSize));
    auto fbmat = cvfb::CreateOcvMatDirect(builder, im.rows, im.cols, im.type(), (int)im.elemSize(), &mat_vec);

    builder.Finish(fbmat);

    zmq::message_t message(reinterpret_cast<char *const>(builder.GetBufferPointer()), builder.GetSize());
    publisher.send(message, zmq::send_flags::none);
}

void read_kps(
    zmq::socket_t& subscriber,
    std::vector<cv::KeyPoint>& kpts,
    cv::Mat& desc){
        zmq::message_t msg;
        zmq::recv_result_t r = subscriber.recv(msg, zmq::recv_flags::dontwait);
        if(r==zmq::recv_result_t{}){
            return; // received nothing, so continue
        }
        char* buf_kp = static_cast<char*>(msg.data());

        auto fbimg = cvfb::GetOcvKpsAndDescs(buf_kp);

        desc.create(fbimg->descs()->rows(), fbimg->descs()->cols(), fbimg->descs()->elt_type());
        size_t dataSize = fbimg->descs()->rows() *  fbimg->descs()->cols() * fbimg->descs()->elt_size();

        std::copy(reinterpret_cast<unsigned char *>(
                const_cast<int8_t *>(fbimg->descs()->mat_data()->data())),
            reinterpret_cast<unsigned char *>(
                const_cast<int8_t *>(fbimg->descs()->mat_data()->data()) + dataSize),
            desc.data);

        kpts.clear();
        kpts.reserve(fbimg->kps()->size());

        for(auto i = fbimg->kps()->kps()->begin(); i!=fbimg->kps()->kps()->end(); i++){
            kpts.push_back(
                cv::KeyPoint(
                    cv::Point2f((*i)->pt().x(), (*i)->pt().y()), 
                    (*i)->size(), 
                    (*i)->angle(),
                    (*i)->response(),
                    (*i)->octave(),
                    (*i)->class_id()
                )
            );
        }
    }

/**
 * There's an argument for running the camera here only and giving the orb points 
 * as output. However, that argument fails if you plan on doing anything else at 
 * all with the camera, such as semantic slam, because this program would then 
 * hog the camera.
*/
cv::Mat read_mat(zmq::socket_t& subscriber){
    zmq::message_t msg;
    std::cout<< "read_mat: made msg_t\n";
    subscriber.recv(msg, zmq::recv_flags::none);
    std::cout<< "read_mat: received msg\n";
    char* buf_img = static_cast<char*>(msg.data());
    std::cout<< "read_mat: converted msg to char*\n";

    auto fbimg = cvfb::GetOcvMat(buf_img);

    cv::Mat out;

    out.create(fbimg->rows(), fbimg->cols(), fbimg->elt_type());
    size_t dataSize = fbimg->rows() *  fbimg->cols() * fbimg->elt_size();

    std::copy(reinterpret_cast<unsigned char *>(
            const_cast<int8_t *>(fbimg->mat_data()->data())),
        reinterpret_cast<unsigned char *>(
            const_cast<int8_t *>(fbimg->mat_data()->data()) + dataSize),
        out.data);

    return std::move(out);
}

void write_kps(
    zmq::socket_t& publisher,
    std::vector<cv::KeyPoint>& kpts,
    cv::Mat& desc
){
    flatbuffers::FlatBufferBuilder builder(10*1024*1024); //todo: modify this by checking size. 

        std::vector<cvfb::OcvKp> kp_vec;
        for (std::vector<cv::KeyPoint>::iterator it = kpts.begin();it!=kpts.end();it++){
            auto kp = cvfb::OcvKp(
                (*it).angle,
                (*it).class_id,
                (*it).octave,
                cvfb::Point2f((*it).pt.x, (*it).pt.y),
                (*it).response,
                (*it).size
            );
            kp_vec.push_back(std::move(kp));
        }
        auto kps_obj = cvfb::CreateOcvKpsDirect(builder, (int32_t)kp_vec.size(), &kp_vec);

        size_t dataSize;
        if (desc.dims == 0) {
            dataSize = 0;
        }
        else {
            dataSize = desc.rows * desc.cols * (int)desc.elemSize();
        }
        
        auto datavec = builder.CreateVector(reinterpret_cast<int8_t*>(desc.data), dataSize);

        flatbuffers::Offset<cvfb::OcvMat> mat_copy;
        if (desc.dims == 0) {
            mat_copy = cvfb::CreateOcvMat(builder, desc.rows, desc.cols, desc.type(), 0, datavec);
        }
        else {
            mat_copy = cvfb::CreateOcvMat(builder, desc.rows, desc.cols, desc.type(), (int)desc.elemSize(), datavec);
        }
        

        auto kpsAndDescs = cvfb::CreateOcvKpsAndDescs(builder, kps_obj, mat_copy);

        builder.Finish(kpsAndDescs);

        //auto builder_str = std::string(reinterpret_cast<char *const>(builder.GetBufferPointer()), builder.GetSize());

        zmq::message_t message(reinterpret_cast<char *const>(builder.GetBufferPointer()), builder.GetSize());
        publisher.send(message, zmq::send_flags::none);
       
}