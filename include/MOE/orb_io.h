#ifndef ORB_IO_H
#define ORB_IO_H

#include "MultiPlatform.h"

#include <opencv2/opencv.hpp>
#include <zmq.hpp>
#include <vector>
#include <string>

std::string DLLEXPORT topic_for_mat();
std::string DLLEXPORT topic_for_kp();
std::string DLLEXPORT topic_for_settings();

void DLLEXPORT write_mat(zmq::socket_t& publisher, cv::Mat& im, bool include_topic=true);
void DLLEXPORT write_kps(
    zmq::socket_t& publisher,
    std::vector<cv::KeyPoint>& kpts,
    cv::Mat& desc,
    bool include_topic=true
);

cv::Mat DLLEXPORT read_mat(zmq::socket_t& subscriber);
void DLLEXPORT read_kps(
    zmq::socket_t& subscriber,
    std::vector<cv::KeyPoint>& kpts,
    cv::Mat& desc
);

#endif